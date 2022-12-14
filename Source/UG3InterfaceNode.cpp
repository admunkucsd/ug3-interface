#ifdef _WIN32
#include <Windows.h>
#endif

#include "UG3InterfaceNode.h"

#include "UG3InterfaceCanvas.h"
#include "UG3InterfaceEditor.h"

#include "UG3InterfaceComponents.h"
#include "Inputs/UG3Socket.h"
#include "Inputs/UG3SimulatedInput.h"

#include <thread>

using namespace UG3Interface;

ActivityDataContainer::ActivityDataContainer(int numChannels, int updateInterval_)
{
    for (int i = 0; i < numChannels; i++)
        peakToPeakValues.add(0);

    updateInterval = updateInterval_;

    reset();
}

const float* ActivityDataContainer::getPeakToPeakValues() {
    std::cout << "p2p[0]: " << peakToPeakValues[0] << std::endl;
    return peakToPeakValues.getRawDataPointer();
}

void ActivityDataContainer::addSample(float sample, int channel)
{
    //std::cout << "in addSample for sample/channel: " <<sample << "/" << channel <<std::endl;
    if (channel == 0)
    {
        if (counter == updateInterval)
            reset();

        counter++;
    }

    //if (counter % 10 == 0)
    //{
    if (sample < minChannelValues[channel])
    {
        minChannelValues.set(channel, sample);
        return;
    }

    if (sample > maxChannelValues[channel])
    {
        maxChannelValues.set(channel, sample);
    }
    //}

}

void ActivityDataContainer::reset()
{

    //std::cout << "Reset." << std::endl;

    for (int i = 0; i < peakToPeakValues.size(); i++)
    {

        peakToPeakValues.set(i, maxChannelValues[i] - minChannelValues[i]);
        
        minChannelValues.set(i, 999999.9f);
        maxChannelValues.set(i, -999999.9f);
    }

    counter = 0;

}

std::vector<String> UG3InterfaceNode::getInputNames() {
    return inputNames;
}

DataThread* UG3InterfaceNode::createDataThread(SourceNode *sn)
{
    return new UG3InterfaceNode(sn);
}

const std::vector<String> UG3InterfaceNode::inputNames = std::vector<String>{"Simulated", "Socket"};

UG3InterfaceNode::UG3InterfaceNode(SourceNode* sn) : DataThread(sn),
    num_channels(DEFAULT_NUM_CHANNELS_X*DEFAULT_NUM_CHANNELS_Y),
    num_samp(DEFAULT_NUM_SAMPLES),
    data_offset(DEFAULT_DATA_OFFSET),
    data_scale(DEFAULT_DATA_SCALE),
    num_channels_x(DEFAULT_NUM_CHANNELS_X),
    num_channels_y(DEFAULT_NUM_CHANNELS_Y),
    sample_rate(DEFAULT_SAMPLE_RATE),
    bitWidth(DEFAULT_CHANNEL_BITWIDTH),
    lastBufferUpdate(-1),
    lastTimerCallback(0)
{
    //FIXME: Use editor ComboBox to determine input
    //input = new UG3Socket(false, 0,0);
    input = new UG3SimulatedInput(connected, num_channels_x, num_channels_y, num_samp, getInputMaxValue());
    connected = input->connect();
    sourceBuffers.add(new DataBuffer(num_channels, 10000)); // start with 2 channels and automatically resize
    recvbuf = (uint16_t *) malloc(num_channels * num_samp * 2);
    convbuf = (float *) malloc(num_channels * num_samp * 4);

    
    activityDataContainer = std::make_unique<ActivityDataContainer>(num_channels, num_samp);
    //activityDataContainer.reset();
}

std::unique_ptr<GenericEditor> UG3InterfaceNode::createEditor(SourceNode* sn)
{

    std::unique_ptr<UG3InterfaceEditor> editor = std::make_unique<UG3InterfaceEditor>(sn, this);

    return editor;
}



UG3InterfaceNode::~UG3InterfaceNode()
{
    free(recvbuf);
    free(convbuf);
}

void UG3InterfaceNode::setInputIndexes(std::set<int> indexes) {
    input -> setIndexes(indexes);
}


void UG3InterfaceNode::resizeChanSamp()
{
    sourceBuffers[0]->resize(num_channels, 10000);
    recvbuf = (uint16_t *)realloc(recvbuf, num_channels * num_samp * 2);
    convbuf = (float *)realloc(convbuf, num_channels * num_samp * 4);
    sampleNumbers.resize(num_channels);
    timestamps.clear();
    timestamps.insertMultiple(0, 0.0, num_samp);
    ttlEventWords.resize(num_channels);
    
    //Needs to set processor output bus channel count
    juce::AudioProcessor::Bus* outputBus;
    for(int i = 0; i < sn ->getBusCount(false); i++) {
        juce::AudioProcessor::Bus* currentBus = sn->getBus(false, i);
        if(currentBus->getName() == "Output") {
            outputBus = currentBus;
            break;
        }
    }
    outputBus -> setNumberOfChannels(num_channels);
    //Needs to call update settings, dataStream channel count not set
    sn->updateSettings();
}

int UG3InterfaceNode::getNumChannels() const
{
    return num_channels;
}

void UG3InterfaceNode::updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
    OwnedArray<EventChannel>* eventChannels,
    OwnedArray<SpikeChannel>* spikeChannels,
    OwnedArray<DataStream>* sourceStreams,
    OwnedArray<DeviceInfo>* devices,
    OwnedArray<ConfigurationObject>* configurationObjects)
{

    continuousChannels->clear();
    eventChannels->clear();
    devices->clear();
    spikeChannels->clear();
    configurationObjects->clear();
    sourceStreams->clear();
    DataStream::Settings settings
    {
        "UG3InterfaceStream",
        "description",
        "identifier",
        sample_rate
    };

    sourceStreams->add(new DataStream(settings));
    sourceBuffers[0]->resize(num_channels, 10000);

    for (int ch = 0; ch < num_channels; ch++)
    {

        ContinuousChannel::Settings settings{
            ContinuousChannel::Type::ELECTRODE,
            "CH" + String(ch + 1),
            "description",
            "identifier",

            data_scale,

            sourceStreams->getFirst()
        };

        continuousChannels->add(new ContinuousChannel(settings));
    }

    EventChannel::Settings eventSettings{
           EventChannel::Type::TTL,
           "Events",
           "description",
           "identifier",
           sourceStreams->getFirst(),
           1
    };

    eventChannels->add(new EventChannel(eventSettings));

}

bool UG3InterfaceNode::foundInputSource()
{
    return connected;
}

bool UG3InterfaceNode::startAcquisition()
{
    resizeChanSamp();

    total_samples = 0;

    eventState = 0;

    startTimer(1000);

    startThread();
    
    lastTimerCallback = Time::getHighResolutionTicks();

    return true;
}


bool UG3InterfaceNode::stopAcquisition()
{
    if (isThreadRunning())
    {
        signalThreadShouldExit();
    }

    waitForThreadToExit(500);

    stopTimer();

    sourceBuffers[0]->clear();
    return true;
}

bool UG3InterfaceNode::updateBuffer()
{
    bool result = input -> loadBuffer((void*)recvbuf, num_channels*num_samp*2);
    if(!result)
        return false;
    //FIXME: Add back offset/scale
    int k = 0;
    for (int i = 0; i < num_samp; i++) {
        for (int j = 0; j < num_channels; j++) {
            convbuf[k] =(float)(recvbuf[i*num_channels + j]);
            activityDataContainer->addSample(convbuf[k],j);
            k+=1;
        }
        sampleNumbers.set(i, total_samples + i);
        ttlEventWords.set(i, eventState);

    }

    
    std::cout << "convbuf[0]: " << convbuf[0] << " convbuf[1]: "<< convbuf[1] << std::endl;
    
    sourceBuffers[0]->addToBuffer(convbuf,
        sampleNumbers.getRawDataPointer(),
        timestamps.getRawDataPointer(),
        ttlEventWords.getRawDataPointer(),
        num_samp, 
        1);
    
    
    int64 uSecElapsed = int64 (Time::highResolutionTicksToSeconds(Time::getHighResolutionTicks() - lastBufferUpdate) * 1e6);
    int64 timeLeftInSecond = int64((Time::highResolutionTicksToSeconds(lastTimerCallback - Time::getHighResolutionTicks()) + 1)*1e6);
    lastBufferUpdate = Time::getHighResolutionTicks();


    if(lastBufferUpdate > 0){
        if( total_samples >= sample_rate) {
            std::this_thread::sleep_for(std::chrono::microseconds(timeLeftInSecond));
            timeLeftInSecond = int64((Time::highResolutionTicksToSeconds(lastTimerCallback - Time::getHighResolutionTicks()) + 1)*1e6);
        }
        else if(uSecElapsed < (1/sample_rate*num_samp*1e6)){
            int samplesLeft = sample_rate - total_samples;
            std::this_thread::sleep_for(std::chrono::microseconds(timeLeftInSecond/samplesLeft-uSecElapsed));
        }
    }

    

    total_samples += num_samp;

    if(timeLeftInSecond < 1/sample_rate){
        total_samples = 0;
        lastTimerCallback = Time::getHighResolutionTicks();
    }
    return true;
}

void UG3InterfaceNode::timerCallback()
{
    //std::cout << "Expected samples: " << int(sample_rate) << ", Actual samples: " << total_samples << std::endl;
    
    
    //relative_sample_rate = (sample_rate * 5) / float(total_samples);

    //total_samples = 0;
}

unsigned long long UG3InterfaceNode::getInputMaxValue() {
    if(bitWidth > 64)
        return 0;
    else if (bitWidth == 64)
        return 0xffffffffffffffff;
    else
        return (1 << bitWidth) - 1;
}

void UG3InterfaceNode::changeInput(int index) {
    String inputName = getInputNames()[index];
    if(String("Simulated") == inputName) {
        input = new UG3SimulatedInput(connected, num_channels_x, num_channels_y, num_samp, getInputMaxValue());
    }
    else if (String("Socket") == inputName) {
        input = new UG3Socket(connected, DEFAULT_PORT, DEFAULT_SAMPLE_RATE);

    }
    //FIXME: changed to input -> connected() when actually using Socket (connected needs to be True or a reload callback will continually be called)
    //connected = input -> connected():
    connected = true;

}

std::vector<Component*> UG3InterfaceNode::getInputEditorComponents() {
    return input->getEditorComponents();
}

std::vector<Component*> UG3InterfaceNode::getInputCanvasComponents() {
    return input->getCanvasComponents();
}

void UG3InterfaceNode::bindInputActionComponentsToEditor(UG3InterfaceEditor* editor) {
    input -> bindLabelsToEditor((Label::Listener*)(editor));
    input -> bindComboBoxesToEditor((ComboBox::Listener*)(editor));
    input -> bindButtonsToEditor((Button::Listener*)(editor));


}

void UG3InterfaceNode::bindInputActionComponentsToCanvas(UG3InterfaceCanvas* canvas) {
    input -> bindLabelsToCanvas((Label::Listener*)(canvas));
    input -> bindComboBoxesToCanvas((ComboBox::Listener*)(canvas));
    input -> bindButtonsToCanvas((Button::Listener*)(canvas));
}


bool UG3InterfaceNode::onInputLabelChanged(Label * label){
    return input->onLabelChanged(label);
}
bool UG3InterfaceNode::onInputComboBoxChanged(ComboBox * comboBox){
    return input->onComboBoxChanged(comboBox);
}
bool UG3InterfaceNode::onInputButtonPressed(Button * button){
    return input->onButtonPressed(button);
}

void UG3InterfaceNode::saveInputCustomParametersToXml(XmlElement* parameters){
    input->saveCustomParametersToXml(parameters);
}
void UG3InterfaceNode::loadInputCustomParametersFromXml(XmlElement* parameters){
    input->loadCustomParametersFromXml(parameters);
}

void UG3InterfaceNode::resizeEditorComponents(int startX, int startY) {
    input->resizeEditorComponents(startX, startY);
}
void UG3InterfaceNode::resizeCanvasComponents(int startX, int startY) {
    input->resizeCanvasComponents(startX, startY);
}

