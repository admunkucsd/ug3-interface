#ifndef __EPHYSSOCKETEDITORH__
#define __EPHYSSOCKETEDITORH__

#ifdef _WIN32
#include <Windows.h>
#endif

#include <VisualizerEditorHeaders.h>

namespace UG3Interface
{
    class UG3InterfaceNode;

    class UG3InterfaceEditor : public VisualizerEditor,
                              public Label::Listener,
                              public Button::Listener
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

    private:

        // Button that tried to connect to client
        ScopedPointer<UtilityButton> connectButton;

        // Port
        ScopedPointer<Label> portLabel;
        ScopedPointer<Label> portInput;

        // Buffer size
        ScopedPointer<Label> bufferSizeMainLabel;

        // x label
        ScopedPointer<Label> xLabel;

        // Chans
        ScopedPointer<Label> channelCountLabel;
        ScopedPointer<Label> channelCountInput;

        // Samples
        ScopedPointer<Label> bufferSizeLabel;
        ScopedPointer<Label> bufferSizeInput;

        // Fs
        ScopedPointer<Label> sampleRateLabel;
        ScopedPointer<Label> sampleRateInput;

        // Scale
        ScopedPointer<Label> scaleLabel;
        ScopedPointer<Label> scaleInput;

        // Offset
        ScopedPointer<Label> offsetLabel;
        ScopedPointer<Label> offsetInput;


        // Parent node
        UG3InterfaceNode* node;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3InterfaceEditor);
    };
}

#endif
