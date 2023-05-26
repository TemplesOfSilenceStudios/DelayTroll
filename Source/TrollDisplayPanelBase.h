/*
  ==============================================================================

    TrollDisplayPanelBase.h
    Created: 23 Apr 2019 9:43:20am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "DTVirticalConstraintDisplay.h"
#include "DTManualControler.h"
#include "DTTrollDisplayComponent.h"
#include "PanelBase.h"
#include "DTParamCheckBox.h"
#include "TrollTimeControlPanel.h"
#include "DTButton.h"


//==============================================================================
/**Base class for our troll displays, creates and distributes lines representing our trolls
*/
class TrollDisplayPanelBase    : public PanelBase, public Timer
{
public:
    TrollDisplayPanelBase(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel, int _manualParamIndex, DTManualControl::DTManualModes _manualMode, int _topManualControlIndex, int _bottomManualControlIndex = -1);
    ~TrollDisplayPanelBase();

    virtual void paint (Graphics&) override;
    
    virtual void timerCallback() override = 0;
    
    virtual void setComponentVisability(int _controlMode) = 0;
    
    DTTrollDisplayComponent& getDisplayComponent(){return mDisplayComponent;};
    
    void setLabels(){mControlPanel.setLabels();};
    void setQuantEnabled(bool _isQuantEnabled){mQuantizationAmount.setEnabled(_isQuantEnabled);};
    
protected:
    
    Rectangle<int> mInnerDisplayArea;
    
    DTTrollDisplayComponent mDisplayComponent;
    
    const Image mModeImage, mQuantImage;
    const Image mModeGlImage, mQuantGlImage;
    
    DrawableImage mModeGl, mQuantGl;
    
    const Image mResetImageOn, mResetImageOff;
    
    DTButton mReset;
    
    const Image mResetLableImage;
    
    DrawableImage mResetLable;
    
    const Image mBypassImageOn, mBypassImageOff;
    
    DTParamComboBox mMode;
    
    DTManualControl::DTManualControler mManuelControler;
    
    DTParamComboBox mQuantizationAmount;
    
    TrollTimeControlPanel& mControlPanel;
    
    int mLastModeSelection;
    int mLastQuantSelection;
    
    Rectangle<int> mTextRect;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrollDisplayPanelBase)
};
