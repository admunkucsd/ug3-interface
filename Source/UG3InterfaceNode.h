#ifndef __EPHYSSOCKETH__
#define __EPHYSSOCKETH__

#include <DataThreadHeaders.h>
const int DEFAULT_PORT = 9001;
const float DEFAULT_SAMPLE_RATE = 30000.0f;
const float DEFAULT_DATA_SCALE = 0.195f;
const uint16_t DEFAULT_DATA_OFFSET = 32768;
const int DEFAULT_NUM_SAMPLES = 10;
const int DEFAULT_NUM_CHANNELS_X = 64;
const int DEFAULT_NUM_CHANNELS_Y = 64;
const uint8_t DEFAULT_CHANNEL_BITWIDTH = 16;

namespace UG3Interface
{
    class UG3Input;

    class ActivityDataContainer
    {
    public:
        /** Constructor */
        ActivityDataContainer(int numChannels, int updateInterval);

        /** Returns a pointer to the peak-to-peak values across channels*/
        const float* getPeakToPeakValues();

        /** Adds an incoming sample for a given channel */
        void addSample(float sample, int channel);

        /** Reset min/max values*/
        void reset();

    private:

        Array<float> minChannelValues;
        Array<float> maxChannelValues;
        Array<float> peakToPeakValues;

        int counter;
        int updateInterval;

    };

    class UG3InterfaceNode : public DataThread, public Timer
    {

    public:
        UG3InterfaceNode(SourceNode* sn);
        ~UG3InterfaceNode();

        // Interface fulfillment
        bool foundInputSource() override;

        void updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
            OwnedArray<EventChannel>* eventChannels,
            OwnedArray<SpikeChannel>* spikeChannels,
            OwnedArray<DataStream>* sourceStreams,
            OwnedArray<DeviceInfo>* devices,
            OwnedArray<ConfigurationObject>* configurationObjects);

        int getNumChannels() const;

        // User defined
        int port;
        float sample_rate;
        float data_scale;
        uint16_t data_offset;
        int num_samp;
        int num_channels;
        int num_channels_x;
        int num_channels_y;


        int64 total_samples;
        float relative_sample_rate;

        uint64 eventState;

        void resizeChanSamp();
        void tryToConnect();
        
        /**Used to calculate the maximum value for a given input bit width**/
        unsigned long long getInputMaxValue();

        std::unique_ptr<GenericEditor> createEditor(SourceNode* sn);
        static DataThread* createDataThread(SourceNode* sn);

        const float* getLatestValues() {
            return activityDataContainer->getPeakToPeakValues();
        }
                                
        std::vector<String> getInputNames();

            
    private:

        bool updateBuffer() override;
        bool startAcquisition() override;
        bool stopAcquisition()  override;
        void timerCallback() override;

        bool connected = false;

        ScopedPointer<UG3Input> input;
        
        std::unique_ptr<ActivityDataContainer> activityDataContainer;
        
        static const std::vector<String> inputNames; 
        uint16_t *recvbuf;
        float *convbuf;
        uint8_t bitWidth;

        Array<int64> sampleNumbers;
        Array<double> timestamps;
        Array<uint64> ttlEventWords;

        int64 currentTimestamp;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3InterfaceNode);
    };
}
#endif
