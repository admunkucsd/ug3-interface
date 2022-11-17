//
//  UG3InterfaceComponents.hpp
//  ug3-interface
//
//  Created by Allen Munk on 11/2/22.
//

#ifndef UG3InterfaceComponents_h
#define UG3InterfaceComponents_h

#include <DataThreadHeaders.h>

namespace UG3Interface {

class UG3Input {
public:
    
    /** Returns true if the input is able to connect to the source */
    virtual ~UG3Input() = default;
    
    /** Returns true if the input is able to connect to the source */
    virtual bool connect() = 0;
    
    /** Returns true if the input is able to reconnect to the source */
    virtual bool reconnect() = 0;
    
    /** Returns true if the input is able to load data from the source */
    virtual bool loadBuffer(void * destBuffer, int maxBytestoRead) = 0;
    
    virtual void bindComboBoxesToEditor(ComboBox::Listener* listener) = 0;
    virtual void bindLabelsToEditor(Label::Listener* listener) = 0;
    virtual void bindButtonsToEditor(Button::Listener* listener) = 0;
    
    virtual bool onComboBoxChanged(ComboBox * comboBox);
    virtual bool onLabelChanged(Label * label);
    virtual bool onButtonPressed(Button * button);

    virtual std::vector<Component*> getEditorComponents() = 0;
    virtual void saveCustomParametersToXml(XmlElement* parameters) = 0;
    virtual void loadCustomParametersFromXml(XmlElement* parameters) = 0;
};

}
#endif /* UG3InterfaceComponents_h */
