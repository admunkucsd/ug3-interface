//
//  UG3Socket.hpp
//  ug3-interface
//
//  Created by Allen Munk on 11/2/22.
//

#ifndef UG3Socket_h
#define UG3Socket_h

#include <stdio.h>
#include <DataThreadHeaders.h>
#include <VisualizerEditorHeaders.h>
#include "../UG3InterfaceComponents.h"

namespace UG3Interface {
class UG3Socket : public UG3Input {
public:
    UG3Socket(bool& connected, int port=9001, float sampleRate=30000.0);
    ~UG3Socket() override;
    bool connect() override;
    bool reconnect() override;
    bool loadBuffer(void * destBuffer, int maxBytestoRead) override;
    
    std::vector<Component*> getEditorComponents() override;
        
    void bindComboBoxesToEditor(ComboBox::Listener* listener) override;
    void bindLabelsToEditor(Label::Listener* listener) override;
    void bindButtonsToEditor(Button::Listener* listener) override;
    
    bool onComboBoxChanged(ComboBox * comboBox) override;
    bool onLabelChanged(Label * label) override;
    bool onButtonPressed(Button * button) override;
    
    void saveCustomParametersToXml(XmlElement* parameters) override;
    void loadCustomParametersFromXml(XmlElement* parameters) override;

private:
    ScopedPointer<DatagramSocket> socket;
    
    //Component Data Members
    int port;
    float sampleRate;
    bool connected;
    
    //Editor Components
    ScopedPointer<UtilityButton> connectButton;
    ScopedPointer<Label> portLabel;
    ScopedPointer<Label> portInput;
    ScopedPointer<Label> sampleRateLabel;
    ScopedPointer<Label> sampleRateInput;
    
};
}
#endif /* UG3Socket_h */
