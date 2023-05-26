/*
  ==============================================================================

    ModFreqControlPanel.h
    Created: 20 Sep 2020 10:36:04am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PanelBase.h"
#include "DTParamSlider.h"
#include "DTCheckBox.h"
#include "DTParamValueLabel.h"
#include "BPMSyncLabel.h"
#include "DTParamComboBox.h"
#include "DTButton.h"

//==============================================================================
/*
*/
class ModFreqControlPanel  : public PanelBase
{
public:
    ModFreqControlPanel(DelayTrollAudioProcessor& p, ModManagment& _inManager);
    ~ModFreqControlPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ModManagment& mModManagerRef;
    
    BPMManager& mBPMManagerRef;
    
    const Image mGearImage;
    const Image mFreqLableImage;
    
    DTParameterSlider mHertzSlider;
    DrawableImage mFreqLable;
    
    const Image mHertzMode, mSyncMode;
    
    DTButton  mBPMSyncMode;

    const Image mQuantDropDownImage;
    DTParamComboBox    mQuantizationDropDown;
    const Image mQuantGlImage;
    DrawableImage mQuantGl;
    
    Value& mBarFractionsRef;
    
    DTParamValueLabel mHzLabel;
    
    BPMSyncLabel mBPMSyncLabel;
    
    const Image mSmallDisplayImage;
    const Image mSmallDisplayGlImage;
    DrawableImage mSmallDisplay;
    DrawableImage mSmallDisplayGl;
    

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModFreqControlPanel)
};
