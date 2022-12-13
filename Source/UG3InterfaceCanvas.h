/*
 ------------------------------------------------------------------

 This file is part of the Open Ephys GUI
 Copyright (C) 2017 Open Ephys

 ------------------------------------------------------------------

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef __UG3InterfaceCANVAS_H__
#define __UG3InterfaceCANVAS_H__

#include "VisualizerWindowHeaders.h"
#include <set>

namespace UG3Interface {

class UG3GridDisplay;

class UG3InterfaceCanvas : public Visualizer,
    public Label::Listener,
    public ComboBox::Listener
{
public:

    /** Constructor */
    UG3InterfaceCanvas(class UG3InterfaceNode*, int numChannels, unsigned long long inputMaxValue = 0xffff);

    /** Destructor */
    virtual ~UG3InterfaceCanvas() override;

    /**
     *  Required overrides from Visualizer
     */

    /** Called when the component's tab becomes visible again.*/
    void refreshState() override;

    /** Called when parameters of underlying data processor are changed.*/
    void update() override;

    /** Called instead of "repaint" to avoid redrawing underlying components if not necessary.*/
    void refresh() override;

    /** Called when data acquisition is active.*/
    void beginAnimation() override;

    /** Called when data acquisition ends.*/
    void endAnimation() override;

    /** Label::Listener callback*/
    void labelTextChanged(Label* label);
    
    /** ComboBox::Listener callback*/
    void comboBoxChanged (ComboBox* combo);
    
    void addInputComponents();
    void removeInputComponents();
    void setNodeNumChannels(std::set<int> indexes);


    /**
     *  Overrides from juce::Component
     */

    void paint(Graphics& g) override;
    void resized() override;

    /** Custom method for updating settings */
    void updateDataStream(DataStream* stream);
    


private:
    class UG3InterfaceNode* node;

    std::unique_ptr<class UG3InterfaceViewport> viewport;
    std::unique_ptr<UG3GridDisplay> gridDisplay;
    
    int numChannels;
    unsigned long long inputMaxValue;
    
    int scrollBarThickness;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3InterfaceCanvas);
};

class UG3InterfaceViewport : public Viewport
{
public:
    UG3InterfaceViewport(UG3InterfaceCanvas*);
    virtual ~UG3InterfaceViewport() override;
    void visibleAreaChanged(const Rectangle<int>& newVisibleArea);

private:
    UG3InterfaceCanvas* canvas;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3InterfaceViewport);
};

}

#endif /* __UG3InterfaceCANVAS_H__ */
