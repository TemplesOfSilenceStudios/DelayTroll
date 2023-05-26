/*
  ==============================================================================

    TrollPitchDisplay.cpp
    Created: 31 Jan 2020 6:34:05pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "TrollPitchDisplay.h"
#include "DTModulationDefines.h"
#include "DTUsedParameters.h"

//==============================================================================
TrollPitchDisplay::TrollPitchDisplay(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel):
TrollDisplayPanelBase(p, _ControlPanel, kNMod_PitchManuelMode, DTManualControl::kManMode_TopOnly, TrollParams::kTroll_PitchFactor),
mCurve(p, kMod_PitchBez, mInnerDisplayArea.toFloat().getWidth(),
mInnerDisplayArea.toFloat().getHeight()),
mBypass(p.tree, DTParameterID[kBParam_PitchBypass], DTParameterName[kBParam_PitchBypass], mBypassImageOff, mBypassImageOn),
mBypassImage(ImageCache::getFromMemory(BinaryData::Bypass_Large_png, BinaryData::Bypass_Large_pngSize))
{
    mProcessor.addListener(this);
    
    mMode.addItemList(DTChoiceParameterStrings[3], 1);
    mMode.setSelectedId(p.getNonModParamAsInt(kNMod_PitchManuelMode), dontSendNotification);
    mLastModeSelection = p.getNonModParamAsInt(kNMod_PitchManuelMode);
    
    mDisplayComponent.setAlwaysOnTop(false);
    
    setAlwaysOnTop(true);
    
    mCurve.setBounds(mInnerDisplayArea);
    mCurve.setAlwaysOnTop(true);
    mCurve.addChangeListener(this);
    addChildComponent(&mCurve);
    
    
    mMode.onChange = [this](){
        if(mMode.getSelectedId() == mLastModeSelection || !isVisible())
        {
            return;
        }
        else{
            mLastModeSelection = mMode.getSelectedId();
        }
        
        switch (mMode.getSelectedId()) {
            case kPitchOp_Bez:
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mCurve.setInterceptsMouseClicks(true, true);
                mCurve.setAlwaysOnTop(true);
                mCurve.setVisible(true);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
            case kPitchOp_Manual:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mManuelControler.setMode(DTManualControl::kManMode_TopOnly);
                mCurve.setInterceptsMouseClicks(false, false);
                mCurve.setAlwaysOnTop(false);
                mCurve.setVisible(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
            case kPitchOp_Quant:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mManuelControler.setMode(DTManualControl::kManMode_QuantizedY);
                mCurve.setInterceptsMouseClicks(false, false);
                mCurve.setAlwaysOnTop(false);
                mCurve.setVisible(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
            default:
                break;
        }
        
        timerCallback();
    };
    
    
    mReset.onClick = [this](){
        mCurve.getPlotter()->setNodes(DTBezierInitalizedNodes[kBez_Pitch]);
        mCurve.resetNodes();
    };
    addChildComponent(&mReset);
    addChildComponent(&mResetLable);
    
    
    //set visability based on what mode is selected
    switch (mMode.getSelectedId()) {
        case kPitchOp_Bez:
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mCurve.setInterceptsMouseClicks(true, true);
            mCurve.setAlwaysOnTop(true);
            mCurve.setVisible(true);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
        case kPitchOp_Manual:
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mManuelControler.setMode(DTManualControl::kManMode_TopOnly);
            mCurve.setInterceptsMouseClicks(false, false);
            mCurve.setAlwaysOnTop(false);
            mCurve.setVisible(false);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
        case kPitchOp_Quant:
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mManuelControler.setMode(DTManualControl::kManMode_QuantizedY);
            mCurve.setInterceptsMouseClicks(false, false);
            mCurve.setAlwaysOnTop(false);
            mCurve.setVisible(false);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
        default:
            break;
    }
    
    //mBypass.setSize(mMode.getWidth()/2, mMode.getHeight());
    mBypass.setTopRightPosition(mQuantizationAmount.getX()-3, 0);
    mBypass.setClickingTogglesState(true);
    mBypass.setToggleState(p.getParameters()[kBParam_PitchBypass]->getValue(), NotificationType::dontSendNotification);
    addAndMakeVisible(&mBypass);
    
    
    mBypassDisplay.setImage(mBypassImage);
    
    mBypassDisplay.setBounds(mDisplayComponent.getBounds());
    mBypassDisplay.setBoundingBox(mBypassDisplay.getBounds().toFloat());
    
    mBypassDisplay.setVisible(mBypass.getToggleState());
    
    addChildComponent(&mBypassDisplay);
    
    //mDisplayComponent.setVisible(false);
    //mCurve.setVisible(false);
    
    //mTest.setSize(100, 100);
    //mTest.setSliderStyle(Slider::SliderStyle::LinearBar);
    //mTest.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    //mTest.setCentrePosition(mInnerDisplayArea.getCentre().translated(-100, 0));
    //addAndMakeVisible(&mTest);
    
    
    
}

TrollPitchDisplay::~TrollPitchDisplay()
{
    mProcessor.removeListener(this);
}

void TrollPitchDisplay::paint (Graphics& g)
{
    float x = mInnerDisplayArea.toFloat().getX();
    float rightX = mInnerDisplayArea.toFloat().getRight();
    float scaler = mInnerDisplayArea.getHeight()/24.f;
    g.setFont(10.f);
    g.setOpacity(0.5f);
    for (int i = 1; i<24; i++) {
        float y = mInnerDisplayArea.getY() + static_cast<float>(i)*scaler;
        
        g.drawLine(x, y, rightX, y);
        mTextRect.setCentre(mTextRect.getCentreX(), static_cast<int>(y));
        g.drawFittedText(DTPitchLabels[i-1], mTextRect, Justification::centredLeft, 1);
    }
    
    TrollDisplayPanelBase::paint(g);
    mBypassDisplay.setVisible(mBypass.getToggleState());
}

void TrollPitchDisplay::loadActivated(DelayTrollAudioProcessor* _p)
{
    
    if (&mProcessor == _p){
        
        if(isVisible()){
            mControlPanel.setBPMModeAtomic(mProcessor.getSyncStatus(), false);
            mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
            mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
            mQuantizationAmount.setSelectedId(mLastQuantSelection, dontSendNotification);
            mLastModeSelection = mProcessor.getNonModParamAsInt(kNMod_PitchManuelMode);
            mMode.setSelectedId(mLastModeSelection, dontSendNotification);
            
            if(mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_Gravity ||
               mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_BPMSync) {
                mControlPanel.setGravetyEnabled(true);
            }
            else{
                mControlPanel.setGravetyEnabled(false);
            }
            
        }
        
        switch (mLastModeSelection) {
            case kPitchOp_Bez:
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mCurve.setInterceptsMouseClicks(true, true);
                mCurve.setAlwaysOnTop(true);
                mCurve.setVisible(true);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
            case kPitchOp_Manual:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mManuelControler.setMode(DTManualControl::kManMode_TopOnly);
                mCurve.setInterceptsMouseClicks(false, false);
                mCurve.setAlwaysOnTop(false);
                mCurve.setVisible(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
            case kPitchOp_Quant:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mManuelControler.setMode(DTManualControl::kManMode_QuantizedY);
                mCurve.setInterceptsMouseClicks(false, false);
                mCurve.setAlwaysOnTop(false);
                mCurve.setVisible(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
            default:
                break;
        }
    }// if processor address matches
}

void TrollPitchDisplay::setComponentVisability(int _controlMode)
{
    
    mLastModeSelection = _controlMode;
    mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
    mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
    mMode.setSelectedId(mLastModeSelection, dontSendNotification);
    
    switch (mLastModeSelection) {
        case kPitchOp_Bez:
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mCurve.setInterceptsMouseClicks(true, true);
            mCurve.setAlwaysOnTop(true);
            mCurve.setVisible(true);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
        case kPitchOp_Manual:
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mManuelControler.setMode(DTManualControl::kManMode_TopOnly);
            mCurve.setInterceptsMouseClicks(false, false);
            mCurve.setAlwaysOnTop(false);
            mCurve.setVisible(false);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
        case kPitchOp_Quant:
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mManuelControler.setMode(DTManualControl::kManMode_QuantizedY);
            mCurve.setInterceptsMouseClicks(false, false);
            mCurve.setAlwaysOnTop(false);
            mCurve.setVisible(false);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
        default:
            break;
    }
}
