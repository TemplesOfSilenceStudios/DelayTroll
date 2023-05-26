/*
  ==============================================================================

    BPMSyncLabel.h
    Created: 3 Aug 2020 2:25:11pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DTParamValueLabel.h"
#include "BPMManager.h"

//==============================================================================
/*
*/
class BPMSyncLabel  : public juce::Component
{
public:
    BPMSyncLabel(BPMManager::BPMLabelValues& _labels);
    ~BPMSyncLabel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setNewValues(BPMManager::BPMLabelValues& _labels);
private:
    
    //Value& mWholeNote, mDenominator, mNumerator;
    
    DTParamValueLabel mWholeNoteLabel, mDenominatorLabel, mNumeratorLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BPMSyncLabel)
};
