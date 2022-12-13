//
//  UG3Socket.cpp
//  ug3-interface
//
//  Created by Allen Munk on 11/2/22.
//

#include "UG3Socket.h"

using namespace UG3Interface;

UG3Socket::UG3SocketUI::UG3SocketUI(UG3Socket* socket): socket(socket){
    // Add connect button
    connectButton = new UtilityButton("CONNECT", Font("Small Text", 12, Font::bold));
    connectButton->setRadius(3.0f);

    // Port
    portLabel = new Label("Port Label", "Port");
    portLabel->setFont(Font("Small Text", 16, Font::plain));
    portLabel->setColour(Label::textColourId, Colours::white);

    portInput = new Label("Port Input", String(socket->port));
    portInput->setFont(Font("Small Text", 16, Font::plain));
    portInput->setColour(Label::backgroundColourId, Colours::white);
    portInput->setEditable(true);
    
    // Fs
    sampleRateLabel = new Label("Sample Rate Label", "FREQ (HZ)");
    sampleRateLabel->setFont(Font("Small Text", 16, Font::plain));
    sampleRateLabel->setColour(Label::textColourId, Colours::white);

    sampleRateInput = new Label("Sample Rate Input", String(int(socket->sampleRate)));
    sampleRateInput->setFont(Font("Small Text", 16, Font::plain));
    sampleRateInput->setEditable(true);
    sampleRateInput->setColour(Label::backgroundColourId, Colours::white);
}

std::vector<Component*> UG3Socket::UG3SocketUI::getEditorComponents(){
    std::vector<Component*> returnVector;
    return returnVector;

}

std::vector<Component*> UG3Socket::UG3SocketUI::getCanvasComponents(){
    std::vector<Component*> returnVector;
    returnVector.push_back(connectButton);
    returnVector.push_back(portLabel);
    returnVector.push_back(portInput);
    returnVector.push_back(sampleRateLabel);
    returnVector.push_back(sampleRateInput);
    return returnVector;

}

void UG3Socket::UG3SocketUI::bindComboBoxesToEditor(ComboBox::Listener* listener){}

void UG3Socket::UG3SocketUI::bindLabelsToEditor(Label::Listener* listener){}

void UG3Socket::UG3SocketUI::bindButtonsToEditor(Button::Listener* listener){}

void UG3Socket::UG3SocketUI::bindComboBoxesToCanvas(ComboBox::Listener* listener){}

void UG3Socket::UG3SocketUI::bindLabelsToCanvas(Label::Listener* listener){
    portInput->addListener(listener);
    sampleRateInput->addListener(listener);
}
void UG3Socket::UG3SocketUI::bindButtonsToCanvas(Button::Listener* listener){
    connectButton->addListener(listener);
}

bool UG3Socket::UG3SocketUI::onLabelChanged(Label * label){
    if (label == sampleRateInput)
    {
        float inSampleRate = sampleRateInput->getText().getFloatValue();
        
        if (inSampleRate > 0 && inSampleRate < 50000.0f)
        {
            socket->sampleRate = inSampleRate;
        }
        else {
            sampleRateInput->setText(String(socket->sampleRate), dontSendNotification);
        }
        return true;
        
    }
    else if (label == portInput)
    {
        int inPort = portInput->getText().getIntValue();
        
        if (inPort > 1023 && inPort < 65535)
        {
            socket->port = inPort;
        }
        else {
            portInput->setText(String(socket->port), dontSendNotification);
        }
        return false;
    }
    return false;
    
}

bool UG3Socket::UG3SocketUI::onComboBoxChanged(ComboBox *comboBox){
    return false;
}

bool UG3Socket::UG3SocketUI::onButtonPressed(Button *button){
    
    if (button == connectButton)
    {
        socket->port = portInput->getText().getIntValue();
        socket->reconnect();
    }
    return false;
}


void UG3Socket::UG3SocketUI::saveCustomParametersToXml(XmlElement* parameters){
    parameters->setAttribute("port", portInput->getText());
    parameters->setAttribute("fs", sampleRateInput->getText());
}
void UG3Socket::UG3SocketUI::loadCustomParametersFromXml(XmlElement* parameters){
    portInput->setText(parameters->getStringAttribute("port", ""), dontSendNotification);
    sampleRateInput->setText(parameters->getStringAttribute("fs", ""), dontSendNotification);
}

void UG3Socket::UG3SocketUI::resizeEditorComponents(int startX, int startY){}
void UG3Socket::UG3SocketUI::resizeCanvasComponents(int startX, int startY){
    int x = startX;
    int y = startY - socket->canvasYHeight - socket->canvasYSpacing;
    connectButton->setBounds(x, y, 70, 20);
    
    x = connectButton->getX() + connectButton->getWidth() + socket->canvasXSpacing;
    portLabel->setBounds(x, y, 65, 15);
    
    x = portLabel->getX() + portLabel->getWidth() + socket->canvasXSpacing;
    portInput->setBounds(x, y, 65, 15);
    
    x = portInput->getX() + portInput->getWidth() + socket->canvasXSpacing;
    sampleRateLabel->setBounds(x, y, 85, 15);
    
    x = sampleRateLabel->getX() + sampleRateLabel->getWidth() + socket->canvasXSpacing;
    sampleRateInput->setBounds(x, y, 65, 15);
}

UG3Socket::UG3Socket(bool& connected, int port, float sampleRate) : port(port), sampleRate(sampleRate), UG3Input(connected){
    ui = new UG3SocketUI(this);
    
}
UG3Socket::~UG3Socket(){}
bool UG3Socket::connect(){
    socket = new DatagramSocket();
    socket->bindToPort(port);
    return (socket->waitUntilReady(true, 500) == 1);
}
bool UG3Socket::reconnect(){
    socket->shutdown();
    socket = new DatagramSocket();
    bool bound = socket->bindToPort(port);
    if (bound)
    {
        std::cout << "Socket bound to port " << port << std::endl;
        connected = (socket->waitUntilReady(true, 500) == 1);
        if (connected)
        {
            std::cout << "Socket connected." << std::endl;
            return true;

        }
        else {
            std::cout << "Socket failed to connect" << std::endl;
            return false;
        }
    }
    else {
        std::cout << "Could not bind socket to port " << port << std::endl;
        return false;
    }
}
bool UG3Socket::loadBuffer(void * destBuffer, int maxBytestoRead){
    int rc = socket->read(destBuffer, maxBytestoRead, true);

    if (rc == -1)
    {
        CoreServices::sendStatusMessage("Ephys Socket: Data shape mismatch");
        return false;
    }
    return true;
}

std::vector<Component*> UG3Socket::getEditorComponents(){
    return ui->getEditorComponents();
}

std::vector<Component*> UG3Socket::getCanvasComponents(){
    return ui->getCanvasComponents();
}

void UG3Socket::bindComboBoxesToEditor(ComboBox::Listener* listener){
    ui->bindComboBoxesToEditor(listener);
}

void UG3Socket::bindLabelsToEditor(Label::Listener* listener){
    ui->bindLabelsToEditor(listener);
}

void UG3Socket::bindButtonsToEditor(Button::Listener* listener){
    ui->bindButtonsToEditor(listener);
}

void UG3Socket::bindComboBoxesToCanvas(ComboBox::Listener* listener){
    ui->bindComboBoxesToCanvas(listener);
}
void UG3Socket::bindLabelsToCanvas(Label::Listener* listener){
    ui->bindLabelsToCanvas(listener);
}
void UG3Socket::bindButtonsToCanvas(Button::Listener* listener){
    ui->bindButtonsToCanvas(listener);
}

bool UG3Socket::onLabelChanged(Label * label){
    return ui->onLabelChanged(label);
}

bool UG3Socket::onComboBoxChanged(ComboBox *comboBox){
    return ui->onComboBoxChanged(comboBox);
}

bool UG3Socket::onButtonPressed(Button *button){
    return ui->onButtonPressed(button);
}

void UG3Socket::saveCustomParametersToXml(XmlElement* parameters){
    ui->saveCustomParametersToXml(parameters);
}
void UG3Socket::loadCustomParametersFromXml(XmlElement* parameters){
    ui->loadCustomParametersFromXml(parameters);
    port = parameters->getIntAttribute("port", port);
    sampleRate = parameters->getDoubleAttribute("fs", sampleRate);
}

void UG3Socket::resizeEditorComponents(int startX, int startY) {ui->resizeEditorComponents(startX, startY);}
void UG3Socket::resizeCanvasComponents(int startX, int startY) {ui->resizeCanvasComponents(startX, startY);}

void UG3Socket::setIndexes(std::set<int> indexes){}


