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
    
    UG3Input(bool& connected);
    
    /** Returns true if the input is able to connect to the source */
    virtual ~UG3Input() = default;
    
    /** Returns true if the input is able to connect to the source */
    virtual bool connect() = 0;
    
    /** Returns true if the input is able to reconnect to the source */
    virtual bool reconnect() = 0;
    
    /** Returns true if the input is able to load data from the source */
    virtual bool loadBuffer(void * destBuffer, int maxBytestoRead) = 0;
    virtual std::vector<Component*> getEditorComponents() = 0;
    virtual std::vector<Component*> getCanvasComponents() = 0;

    
    virtual void bindComboBoxesToEditor(ComboBox::Listener* listener) = 0;
    virtual void bindLabelsToEditor(Label::Listener* listener) = 0;
    virtual void bindButtonsToEditor(Button::Listener* listener) = 0;
    
    virtual void bindComboBoxesToCanvas(ComboBox::Listener* listener) = 0;
    virtual void bindLabelsToCanvas(Label::Listener* listener) = 0;
    virtual void bindButtonsToCanvas(Button::Listener* listener) = 0;
    
    virtual bool onComboBoxChanged(ComboBox * comboBox)=0;
    virtual bool onLabelChanged(Label * label)=0;
    virtual bool onButtonPressed(Button * button)=0;
    
    virtual void resizeEditorComponents(int startX, int startY)=0;
    virtual void resizeCanvasComponents(int startX, int startY)=0;

   
    virtual void saveCustomParametersToXml(XmlElement* parameters) = 0;
    virtual void loadCustomParametersFromXml(XmlElement* parameters) = 0;
protected:
    bool& connected;
    const int canvasXSpacing = 10;
    const int canvasYSpacing = 5;
    const int canvasYHeight = 20;
};

}
#endif /* UG3InterfaceComponents_h */
