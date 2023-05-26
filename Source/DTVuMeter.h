/*
  ==============================================================================

    DTVuMeter.h
    Created: 5 Apr 2020 12:31:28pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//==============================================================================
/*
*/
class DTVuMeter    : public Component, public Timer
{
public:
    DTVuMeter(DelayTrollAudioProcessor& _inProcessor);
    ~DTVuMeter();

    void paint (Graphics&) override;
    void timerCallback() override;
    void resized() override;
    
    void setParameterID(int _inParameterID);

private:
    
    float mCh0Level;
    float mCh1Level;
    
    int mParameterID;
    
    DelayTrollAudioProcessor& mProcessor;
    
    Rectangle<int> mDrawrec;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTVuMeter)
};
