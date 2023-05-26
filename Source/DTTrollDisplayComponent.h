/*
  ==============================================================================

    DTTrollDisplayComponent.h
    Created: 11 Aug 2020 11:44:56am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DTAtomicTimeProcessor.h"
#include "AtomicTroll.h"
#include "PluginProcessor.h"
#include "PanelBase.h"

//==============================================================================
/*
*/
class DTTrollDisplayComponent  : public DTDSP::AtomicTimeProcessor::AtomicListener, public PanelBase
{
public:
    DTTrollDisplayComponent( OwnedArray<AtomicTroll>& _trollRef, DTDSP::AtomicTimeProcessor& _timeProcessor, DelayTrollAudioProcessor* _p);
    ~DTTrollDisplayComponent() override;

    void paint (juce::Graphics&) override;
    
    /**sets the lines length and position based off of what panel we are looking at*/
    void setLines();
    
    void atomicMultUpdated(DTDSP::AtomicTimeProcessor* _p) override;
    
    /**removes excess lines or adds new ones to match the time processors mult*/
    void ResetLines();
    
    inline void setTabSelection(int _newTabSelection){
        mTabSelection = _newTabSelection;
        setLines();
    };
    

private:
    
    bool mIsSetting;
    OwnedArray<Line<float>> mTrollLine;
    
    OwnedArray<AtomicTroll>& mAtomicTrollRefs;
    OwnedArray<Line<float>> mAtomicLines;
    OwnedArray<Line<float>> mBackgroundLines;
    Array<std::pair<int, float>>& mTimeMultRef;
    int mTabSelection;
    
    AffineTransform mScaleDown, mScaleUp;
    AffineTransform mTranslateForward, mTranslateBack;
    
    DTDSP::AtomicTimeProcessor& mTimeProcessor;
    
    Label mTestLabel;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTTrollDisplayComponent)
};
