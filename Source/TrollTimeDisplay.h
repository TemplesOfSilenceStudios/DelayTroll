/*
  ==============================================================================

    TrollTimeDisplay.h
    Created: 23 Apr 2019 9:45:55am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "TrollDisplayPanelBase.h"
#include "TrollTimeControlPanel.h"
#include "DTButton.h"
#include "DTParamCheckBox.h"
#include "DTParamComboBox.h"





//==============================================================================
/**repesents and manages interactive controls for the Trolls Temporal Location
*/
class TrollTimeDisplay    : public TrollDisplayPanelBase, public DelayTrollAudioProcessor::Listener, public DelayTrollAudioProcessor::BPMListener
{
public:
    TrollTimeDisplay(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel);
    ~TrollTimeDisplay();

    void paint (Graphics&) override;
    
    void loadActivated(DelayTrollAudioProcessor* p) override;
    
    void setComponentVisability(int _controlMode) override;
    
    void BPMUpdated(DelayTrollAudioProcessor* p) override;
    
    /**this is a dumb way to solve this problem*/
    void setConstraintVisable(bool _shouldConstraintsBeVisable){ mConstraintControl.setVisible(_shouldConstraintsBeVisable);}
    
    void timerCallback() override{
        if(isVisible()){
            mDisplayComponent.setLines();
        }
    };

private:
    
   // Slider mTest;


    DTVirticalConstraintDisplay mConstraintControl;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrollTimeDisplay)
};
