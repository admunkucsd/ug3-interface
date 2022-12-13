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
    
    void setIndexes(std::set<int> indexes);

private:
    ScopedPointer<DatagramSocket> socket;
    
    class UG3SocketUI{
    public:
        UG3SocketUI(UG3Socket* socket);
        
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
        //Editor Components
        ScopedPointer<UtilityButton> connectButton;
        ScopedPointer<Label> portLabel;
        ScopedPointer<Label> portInput;
        ScopedPointer<Label> sampleRateLabel;
        ScopedPointer<Label> sampleRateInput;
        UG3Socket* socket;
        
        
    };
    
    ScopedPointer<UG3SocketUI> ui;
    
    //Component Data Members
    int port;
    float sampleRate;
    bool connected;
    

    
};
}
#endif /* UG3Socket_h */
