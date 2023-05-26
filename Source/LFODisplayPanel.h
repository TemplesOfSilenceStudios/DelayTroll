/*
  ==============================================================================

    LFODisplayPanel.h
    Created: 24 Apr 2019 11:24:36am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ModulationDisplayPanelBase.h"
#include "LFOCurveDisplay.h"

//==============================================================================
/**displays the background for a LFOCurveDisplay Component
*/
class LFODisplayPanel    : public ModulationDisplayPanelBase
{
public:
    LFODisplayPanel(DelayTrollAudioProcessor& p, int _index, int _width, int _height);
    ~LFODisplayPanel();
    
    //void paint(Graphics& _g) override{ mPolar.paint(_g);};
    
    

private:
    
    LFOCurveDisplay mCurve;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFODisplayPanel)
};
