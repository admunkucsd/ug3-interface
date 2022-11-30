//
//  UG3SimulatedInput.hpp
//  ug3-interface
//
//  Created by Allen Munk on 11/3/22.
//

#ifndef UG3SimulatedInput_h
#define UG3SimulatedInput_h

#include <stdio.h>
#include <DataThreadHeaders.h>
#include <vector>
#include <functional>
#include <map>
#include "../UG3InterfaceComponents.h"

namespace UG3Interface {
class UG3SimulatedInput : public UG3Input {
public:
    UG3SimulatedInput(bool& connected, int channelsX = 64, int channelsY = 64, int samples = 10, unsigned long long maxValue = 0xffff);
    ~UG3SimulatedInput() override;
    bool connect() override;
    bool reconnect() override;
    
    /**loads buffer in such a way that a wave effect is created covering
     the entire color theme's spectrum**/
    bool loadBuffer(void * destBuffer, int maxBytestoRead) override;
    
    std::vector<Component*> getEditorComponents() override;
    std::vector<Component*> getCanvasComponents() override;


    void bindComboBoxesToEditor(ComboBox::Listener* listener) override;
    void bindLabelsToEditor(Label::Listener* listener) override;
    void bindButtonsToEditor(Button::Listener* listener) override;
    
    void bindComboBoxesToCanvas(ComboBox::Listener* listener) override;
    void bindLabelsToCanvas(Label::Listener* listener) override;
    void bindButtonsToCanvas(Button::Listener* listener) override;
    
    bool onComboBoxChanged(ComboBox * comboBox) override;
    bool onLabelChanged(Label * label) override;
    bool onButtonPressed(Button * button) override;
    
    void saveCustomParametersToXml(XmlElement* parameters) override;
    void loadCustomParametersFromXml(XmlElement* parameters) override;
    
    void resizeEditorComponents(int startX, int startY) override;
    void resizeCanvasComponents(int startX, int startY) override;

    
    void makeColorWave();
    void makeSine();
    
private:
    
    class UG3SimulatedInputUI {
    public:
        UG3SimulatedInputUI(UG3SimulatedInput* simulator);
        
        std::vector<Component*> getEditorComponents();
        std::vector<Component*> getCanvasComponents();

        void bindComboBoxesToEditor(ComboBox::Listener* listener);
        void bindLabelsToEditor(Label::Listener* listener);
        void bindButtonsToEditor(Button::Listener* listener);
        
        void bindComboBoxesToCanvas(ComboBox::Listener* listener);
        void bindLabelsToCanvas(Label::Listener* listener);
        void bindButtonsToCanvas(Button::Listener* listener);
        
        bool onComboBoxChanged(ComboBox * comboBox);
        bool onLabelChanged(Label * label);
        bool onButtonPressed(Button * button);
        
        void saveCustomParametersToXml(XmlElement* parameters);
        void loadCustomParametersFromXml(XmlElement* parameters);
        
        void resizeEditorComponents(int startX, int startY);
        void resizeCanvasComponents(int startX, int startY);
        
    private:
        UG3SimulatedInput* simulator;
        ScopedPointer<ComboBox> simulationSelector;
    };
    
    std::map<String, std::function<void()>> simulationOptions;
    
    ScopedPointer<UG3SimulatedInputUI> ui;
    
    String simulationSelection;
    float const simPi = 3.1415;
    int const sinePhaseShiftConstant = 1000;
    uint16_t *simulatedValues;
    int channelsX;
    int channelsY;
    int counter;
    int samples;
    unsigned long long maxValue;
    int waveIndex = 0;
};
}
#endif /* UG3SimulatedInput_h */
