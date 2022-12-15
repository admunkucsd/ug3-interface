#include "UG3InterfaceEditor.h"

#include "UG3InterfaceNode.h"
#include "UG3InterfaceCanvas.h"

#include <string>
#include <iostream>

using namespace UG3Interface;

UG3InterfaceEditor::UG3InterfaceEditor(GenericProcessor* parentNode, UG3InterfaceNode *socket) : VisualizerEditor(parentNode, "UG3 Grid", 180)
{
    node = socket;

    //desiredWidth = 400;

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
}

void UG3InterfaceEditor::startAcquisition()
{
    // Disable the whole gui
    inputSelector->setEnabled(false);
    for(Component* inputEditorComponent: node->getInputEditorComponents()) {
        inputEditorComponent->setEnabled(false);
    }
    

    node->resizeChanSamp();
    enable();
}

void UG3InterfaceEditor::stopAcquisition()
{
    // Reenable the whole gui
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
        UG3InterfaceCanvas* c = (UG3InterfaceCanvas*)(canvas.get());
        c->removeInputComponents();
        for(Component* inputEditorComponent: node->getInputEditorComponents()) {
            removeChildComponent(inputEditorComponent);
        }
        node->changeInput(combo->getSelectedId() - 1);
        node->bindInputActionComponentsToEditor(this);
        c->addInputComponents();
        for(Component* inputEditorComponent: node->getInputEditorComponents()) {
            addAndMakeVisible(inputEditorComponent);
        }
        CoreServices::updateSignalChain (this);
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
    return new UG3InterfaceCanvas(node, this, node->num_channels, node->getInputMaxValue());
}

void UG3InterfaceEditor::saveCustomParametersToXml(XmlElement* xmlNode)
{
    XmlElement* parameters = xmlNode->createNewChildElement("PARAMETERS");

    node->saveInputCustomParametersToXml(parameters);
}

void UG3InterfaceEditor::loadCustomParametersFromXml(XmlElement* xmlNode)
{
    forEachXmlChildElement(*xmlNode, subNode)
    {
        if (subNode->hasTagName("PARAMETERS"))
        {
            node->loadInputCustomParametersFromXml(subNode);

        }
    }
}

