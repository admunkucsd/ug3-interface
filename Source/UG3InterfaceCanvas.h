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

namespace UG3Interface {

class Electrode : public Component
{
public:
    Electrode() : c(Colours::grey) { }

    void setColour(Colour c);

    void paint(Graphics& g);

private:
    Colour c;
};

class UG3InterfaceCanvas : public Visualizer,
    public Label::Listener
{
public:

    /** Constructor */
    UG3InterfaceCanvas(class UG3InterfaceNode*, int numChannels);

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

    /**
     *  Overrides from juce::Component
     */

    void paint(Graphics& g) override;
    void resized() override;

    /** Custom method for updating settings */
    void updateDataStream(DataStream* stream);

    /** Update electrode display dimensions */
    void updateDisplayDimensions();

private:
    class UG3InterfaceNode* node;

    ScopedPointer<class UG3InterfaceViewport> viewport;
    OwnedArray<Electrode> electrodes;

    std::unique_ptr<Label> xDimInput;
    std::unique_ptr<Label> xDimLabel;
    std::unique_ptr<Label> yDimInput;
    std::unique_ptr<Label> yDimLabel;

    int numXPixels = 64;
    int numYPixels = 64;

    int numChannels;

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
    class ChannelViewCanvas* channelsView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UG3InterfaceViewport);
};

}

#endif /* __UG3InterfaceCANVAS_H__ */
