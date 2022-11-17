#include "UG3InterfaceEditor.h"

#include "UG3InterfaceNode.h"
#include "UG3InterfaceCanvas.h"

#include <string>
#include <iostream>

using namespace UG3Interface;

UG3InterfaceEditor::UG3InterfaceEditor(GenericProcessor* parentNode, UG3InterfaceNode *socket) : VisualizerEditor(parentNode, "UG3 Grid", 180)
{
    node = socket;

    desiredWidth = 400;

    
    //---
    bufferSizeMainLabel = new Label("BUFFER SIZE", "BUFFER SIZE");
    bufferSizeMainLabel->setFont(Font("Small Text", 12, Font::plain));
    bufferSizeMainLabel->setBounds(167, 30, 95, 15);
    bufferSizeMainLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(bufferSizeMainLabel);

    // Num chans
    channelCountLabel = new Label("CHANNELS", "CHANNELS");
    channelCountLabel->setFont(Font("Small Text", 10, Font::plain));
    channelCountLabel->setBounds(145, 48, 65, 8);
    channelCountLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(channelCountLabel);

    channelCountInput = new Label("Channel count", String(node->num_channels));
    channelCountInput->setFont(Font("Small Text", 10, Font::plain));
    channelCountInput->setBounds(153, 60, 50, 15);
    channelCountInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    channelCountInput->setEditable(true);
    channelCountInput->addListener(this);
    addAndMakeVisible(channelCountInput);

    xLabel = new Label("X", "X");
    xLabel->setFont(Font("Small Text", 15, Font::plain));
    xLabel->setBounds(202, 53, 30, 30);
    xLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(xLabel);


    // Num samples
    bufferSizeLabel = new Label("SAMPLES", "SAMPLES");
    bufferSizeLabel->setFont(Font("Small Text", 10, Font::plain));
    bufferSizeLabel->setBounds(217, 48, 65, 8);
    bufferSizeLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(bufferSizeLabel);

    bufferSizeInput = new Label ("Buffer Size", String(node->num_samp));
    bufferSizeInput->setFont(Font("Small Text", 10, Font::plain));
    bufferSizeInput->setBounds(223, 60, 50, 15);
    bufferSizeInput->setEditable(true);
    bufferSizeInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    bufferSizeInput->addListener(this);
    addAndMakeVisible(bufferSizeInput);

    //---

    // Scale
    scaleLabel = new Label("Scale", "Scale");
    scaleLabel->setFont(Font("Small Text", 10, Font::plain));
    scaleLabel->setBounds(145, 92, 65, 8);
    scaleLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(scaleLabel);

    scaleInput = new Label("Scale", String(node->data_scale));
    scaleInput->setFont(Font("Small Text", 10, Font::plain));
    scaleInput->setBounds(153, 105, 50, 15);
    scaleInput->setEditable(true);
    scaleInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    scaleInput->addListener(this);
    addAndMakeVisible(scaleInput);

    // Offset
    offsetLabel = new Label("Offset", "Offset");
    offsetLabel->setFont(Font("Small Text", 10, Font::plain));
    offsetLabel->setBounds(217, 92, 65, 8);
    offsetLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(offsetLabel);

    offsetInput = new Label("Offset", String(node->data_offset));
    offsetInput->setFont(Font("Small Text", 10, Font::plain));
    offsetInput->setBounds(223, 105, 50, 15);
    offsetInput->setEditable(true);
    offsetInput->setColour(Label::backgroundColourId, Colours::lightgrey);
    offsetInput->addListener(this);
    addAndMakeVisible(offsetInput);
    
    //Input
    inputLabel = new Label ("Input", "Input");
    inputLabel -> setFont(Font("Small Text", 10, Font::plain));
    inputLabel->setBounds (5, 48, 65, 8);
    inputLabel->setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible (inputLabel);
    
    inputSelector = new ComboBox ("Input Selector");
    inputSelector->setBounds (11, 60, 120, 20);
    inputSelector->addListener (this);
    addAndMakeVisible (inputSelector);

    populateInputs();
    
    node->bindInputActionComponentsToEditor(this);
    
    for(Component* inputEditorComponent: node->getInputEditorComponents()) {
        addAndMakeVisible(inputEditorComponent);
    }

}

void UG3InterfaceEditor::labelTextChanged(Label* label)
{

    if (label == channelCountInput)
    {

        std::cout << "Label text changed" << std::endl;

        int num_channels = channelCountInput->getText().getIntValue();

        if (num_channels > 0 && num_channels < 1000)
        {
            node->num_channels = num_channels;
            CoreServices::updateSignalChain(this);
        }
        else {
            channelCountInput->setText(String(node->num_channels), dontSendNotification);
        }
        
    }
    else if (label == bufferSizeInput)
    {
        int bufferSize = bufferSizeInput->getText().getIntValue();

        if (bufferSize > 0 && bufferSize < 2048)
        {
            node->num_samp = bufferSize;
        }
        else {
            bufferSizeInput->setText(String(node->num_samp), dontSendNotification);
        }
    }
    else if (label == scaleInput)
    {
        float scale = scaleInput->getText().getFloatValue();

        if (scale > 0.0f && scale < 9999.9f)
        {
            node->data_scale = scale;
        }
        else {
            scaleInput->setText(String(node->data_scale), dontSendNotification);
        }
    }
    else if (label == offsetInput)
    {
        int offset = offsetInput->getText().getIntValue();

        if (offset > 0 && offset < 65536)
        {
            node->data_offset = offset;
        }
        else {
            offsetInput->setText(String(node->data_offset), dontSendNotification);
        }
    }
    else if (node->onInputLabelChanged(label)) {
        CoreServices::updateSignalChain(this);
    }
}

void UG3InterfaceEditor::startAcquisition()
{
    // Disable the whole gui
    channelCountInput->setEnabled(false);
    bufferSizeInput->setEnabled(false);
    scaleInput->setEnabled(false);
    offsetInput->setEnabled(false);
    inputSelector->setEnabled(false);
    for(Component* inputEditorComponent: node->getInputEditorComponents()) {
        inputEditorComponent->setEnabled(false);
    }
    

    // Set the channels etc
    node->data_scale = scaleInput->getText().getFloatValue();
    node->data_offset = offsetInput->getText().getIntValue();

    node->resizeChanSamp();
    enable();
}

void UG3InterfaceEditor::stopAcquisition()
{
    // Reenable the whole gui
    channelCountInput->setEnabled(true);
    bufferSizeInput->setEnabled(true);
    scaleInput->setEnabled(true);
    offsetInput->setEnabled(true);
    inputSelector->setEnabled(true);
    for(Component* inputEditorComponent: node->getInputEditorComponents()) {
        inputEditorComponent->setEnabled(true);
    }
    disable();
}

void UG3InterfaceEditor::buttonClicked(Button* button)
{

    if (node->onInputButtonPressed(button)) {
        CoreServices::updateSignalChain(this);
    }
  
}

void UG3InterfaceEditor::comboBoxChanged (ComboBox* combo){
    if(combo == inputSelector) {
        for(Component* inputEditorComponent: node->getInputEditorComponents()) {
            removeChildComponent(inputEditorComponent);
        }
        node->changeInput(combo->getSelectedId() - 1);
        node->bindInputActionComponentsToEditor(this);
        for(Component* inputEditorComponent: node->getInputEditorComponents()) {
            addAndMakeVisible(inputEditorComponent);
        }
        CoreServices::updateSignalChain (this);
    }
    else if (node->onInputComboBoxChanged(combo)) {
        CoreServices::updateSignalChain(this);

    }
}

void UG3InterfaceEditor::populateInputs ()
{
    int i = 0;
    for (String input:node->getInputNames())
    {
        inputSelector->addItem(input, i + 1);
        i++;
    }
    inputSelector->setSelectedId (1, dontSendNotification);
}


Visualizer* UG3InterfaceEditor::createNewCanvas()
{
    return new UG3InterfaceCanvas(node, node->num_channels, node->getInputMaxValue());
}

void UG3InterfaceEditor::saveCustomParametersToXml(XmlElement* xmlNode)
{
    XmlElement* parameters = xmlNode->createNewChildElement("PARAMETERS");

    parameters->setAttribute("numchan", channelCountInput->getText());
    parameters->setAttribute("numsamp", bufferSizeInput->getText());
    parameters->setAttribute("scale", scaleInput->getText());
    parameters->setAttribute("offset", offsetInput->getText());
    //node->saveInputCustomParametersToXml(parameters);
}

void UG3InterfaceEditor::loadCustomParametersFromXml(XmlElement* xmlNode)
{
    forEachXmlChildElement(*xmlNode, subNode)
    {
        if (subNode->hasTagName("PARAMETERS"))
        {

            channelCountInput->setText(subNode->getStringAttribute("numchan", ""), dontSendNotification);
            node->num_channels = subNode->getIntAttribute("numchan", DEFAULT_NUM_CHANNELS_X*DEFAULT_NUM_CHANNELS_Y);

            bufferSizeInput->setText(subNode->getStringAttribute("numsamp", ""), dontSendNotification);
            node->num_samp = subNode->getIntAttribute("numsamp", DEFAULT_NUM_SAMPLES);

            scaleInput->setText(subNode->getStringAttribute("scale", ""), dontSendNotification);
            node->data_scale = subNode->getDoubleAttribute("scale", DEFAULT_DATA_SCALE);

            offsetInput->setText(subNode->getStringAttribute("offset", ""), dontSendNotification);
            node->data_offset = subNode->getIntAttribute("offset", DEFAULT_DATA_OFFSET);
            
            //node->loadInputCustomParametersFromXml(subNode);

        }
    }
}

