#ifndef __EPHYSSOCKETEDITORH__
#define __EPHYSSOCKETEDITORH__

#ifdef _WIN32
#include <Windows.h>
#endif

#include <VisualizerEditorHeaders.h>
#include <map>

namespace UG3Interface
{
    class UG3InterfaceNode;

    class UG3InterfaceEditor : public VisualizerEditor,
                              public Label::Listener,
                              public Button::Listener,
                              public ComboBox::Listener
    {

    public:

        UG3InterfaceEditor(GenericProcessor* parentNode, UG3InterfaceNode *node);
        
        /** Called by the base class VisualizerEditor to display the canvas
            when the user chooses to display one

            @see VisualizerEditor::buttonClicked
         */
        virtual Visualizer* createNewCanvas() override;

        /** Button listener callback, called by button when pressed. */
        void buttonClicked(Button* button);
        
        /** Responds to combo box selections */
        void comboBoxChanged (ComboBox* combo);

        /** Called by processor graph in beginning of the acqusition, disables editor completly. */
        void startAcquisition();

        /** Called by processor graph at the end of the acqusition, reenables editor completly. */
        void stopAcquisition();

        /** Called when configuration is saved. Adds editors config to xml. */
        void saveCustomParametersToXml(XmlElement* xml) override;

        /** Called when configuration is loaded. Reads editors config from xml. */
        void loadCustomParametersFromXml(XmlElement* xml) override;

        /** Called when label is changed */
        void labelTextChanged(Label* label);
        
        void populateInputs();


    private:
    
        ScopedPointer<ComboBox> inputSelector;
        ScopedPointer<Label>    inputLabel;




        // Parent node
        UG3InterfaceNode* node;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3InterfaceEditor);
    };
}

#endif
