/*
  ==============================================================================

    TrollTimeControlPanel.h
    Created: 23 Apr 2019 9:59:50am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

//#include "TrollControlPanelBase.h"
#include "PanelBase.h"
#include "DTParamSlider.h"
#include "DTParamValueLabel.h"
#include "DTParamComboBox.h"
#include "BPMSyncLabel.h"

//==============================================================================
/**Controls Troll Time related Parameters
*/
class TrollTimeControlPanel    : public PanelBase, public DelayTrollAudioProcessor::Listener
{
public:
    TrollTimeControlPanel(DelayTrollAudioProcessor& p);
    ~TrollTimeControlPanel();
    
    /**sets sliders and labels to appropriate sync mode, optionaly updating the atomic time processor */
    void setBPMModeAtomic(bool _isBPMModeActivated, bool shouldTimeUpdate = true);
    
    /**updates the demominator in sync mode and adjusts the sliders and labels appropriatly then updates the time processor*/
    void setAtomicDenominator();
    
    void setGravetyEnabled(bool _isGravetyEnabled){mGravety.setEnabled(_isGravetyEnabled);};
    
    /**sets the labels and updates the interval values*/
    void setLabels();
    
    /**updates the time processor with a new range or triggures a modulation update if one of them is modulating*/
    void updateTimes(double _minTime, double _maxTime);
    
    /**makes sure their is sufficent spacing between the min and max time bpm managers in sync mode*/
    void checkSpacing();
    
    void loadActivated(DelayTrollAudioProcessor*)override{};
    
private:

    const Image mGravetyImage, mMinTimeImage, mMaxTimeImage;
    
    DTParameterSlider mGravety;
    DTParameterSlider mMinTime;
    DTParameterSlider mMaxTime;
    DTParamValueLabel mMinTimeLabel, mMaxTimeLabel;
    BPMManager& mMinBPMManager;
    BPMManager& mMaxBPMManager;
    BPMSyncLabel mMinSyncLabel, mMaxSyncLabel;
    Array<Rectangle<int>> mQuarts;
    
    const Image mLeftLabelImage, mRightLabelImage;
    const Image mLeftLabelGlImage, mRightLabelGlImage;
    DrawableImage mLeftLabelBack, mRightLabelBack;
    DrawableImage mLeftLabelGl, mRightLabelGl;
    
    double mIntervalValue;
    
    Font mFont;
    AttributedString mAttString;
    //String  mSuffix;
    BubbleMessageComponent mBubble;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrollTimeControlPanel)
};
