/*
  ==============================================================================

    LFOCurveDisplay.h
    Created: 26 Apr 2019 2:11:53pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "LFOPlotter.h"

//==============================================================================
/**Displays an LFO Plotter
*/
class LFOCurveDisplay    : public Component
{
public:
    LFOCurveDisplay(DelayTrollAudioProcessor& p, int _plotEditingIndex, float _width, float _height);
    ~LFOCurveDisplay();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    LFOPlotter& mLFOPlot;
    
    //required to draw a path without closing it, must be initialized
    PathStrokeType pathStroke;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOCurveDisplay)
};
