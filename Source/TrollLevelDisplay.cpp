/*
  ==============================================================================

    TrollLevelDisplay.cpp
    Created: 23 Apr 2019 9:44:19am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "TrollLevelDisplay.h"
#include "DTHelperFunctions.h"
#include "DTUsedParameters.h"
#include "DTModulationDefines.h"
//==============================================================================
TrollLevelDisplay::TrollLevelDisplay(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel)
:   TrollDisplayPanelBase(p, _ControlPanel, kNMod_LevelManuelMode, DTManualControl::kManMode_TopOnly, TrollParams::kTroll_Amplitude)
{
    
    mProcessor.addListener(this);
    
    mMode.addItemList(DTChoiceParameterStrings[2], 1);
    mMode.setSelectedId(p.getNonModParamAsInt(kNMod_LevelManuelMode), dontSendNotification);
    mLastModeSelection = p.getNonModParamAsInt(kNMod_LevelManuelMode);
    
    setAlwaysOnTop(true);
    mCurve = std::make_unique<BezierCurveDisplay>(p, kMod_LevelBez, mInnerDisplayArea.toFloat().getWidth(), mInnerDisplayArea.toFloat().getHeight());
    mCurve->setBounds(mInnerDisplayArea);
    mCurve->setAlwaysOnTop(true);
    mCurve->addChangeListener(this);
    addChildComponent(*mCurve);
    mCurve->setVisible(mMode.getSelectedItemIndex());
    mManuelControler.setVisible(!mMode.getSelectedItemIndex());
    
    mMode.onChange = [this](){
        if(mMode.getSelectedId() == mLastModeSelection || !isVisible())
        {
            return;
        }
        else{
            mLastModeSelection = mMode.getSelectedId();
        }
        
        switch (mMode.getSelectedId()) {
            case kDCOp_Bez:
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mCurve->setInterceptsMouseClicks(true, true);
                mCurve->setAlwaysOnTop(true);
                mCurve->setVisible(true);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
            case kDCOp_Manual:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mCurve->setInterceptsMouseClicks(false, false);
                mCurve->setAlwaysOnTop(false);
                mCurve->setVisible(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
            default:
                break;
        }
        
        timerCallback();
    };
    
    
    mReset.onClick = [this](){
        mCurve->getPlotter()->setNodes(DTBezierInitalizedNodes[kBez_Level]);
        mCurve->resetNodes();
    };
    
    if(mMode.getSelectedItemIndex()){
        addAndMakeVisible(mReset);
        addAndMakeVisible(mResetLable);
    }
    else{
        addChildComponent(mReset);
        addChildComponent(mResetLable);
    }
    
}

TrollLevelDisplay::~TrollLevelDisplay()
{
    stopTimer();
    mCurve->removeChangeListener(this);
    mProcessor.removeListener(this);
}

void TrollLevelDisplay::paint (Graphics& g)
{
    g.setOpacity(0.5f);
    for (int i = 0; i<DTNumberofDBMarkers; i++) {
        float y = 1.f - pow(2.f, static_cast<float>(DTDBMarkers[i])/12.f);
        y = mInnerDisplayArea.toFloat().getY() + y*mInnerDisplayArea.toFloat().getHeight();
        
        mTextRect.setCentre(mTextRect.getCentreX(), static_cast<int>(y));
        g.drawLine(mInnerDisplayArea.toFloat().getX(), y, mInnerDisplayArea.toFloat().getRight(), y);
        if(i != 0){
            g.drawFittedText(String(DTDBMarkers[i]) + String("dB"), mTextRect, Justification::centredLeft, 1);
        }
    }
    
    TrollDisplayPanelBase::paint(g);

}

void TrollLevelDisplay::loadActivated(DelayTrollAudioProcessor* _p)
{
    

    if (&mProcessor == _p){
        
        if(isVisible()){
            mControlPanel.setBPMModeAtomic(mProcessor.getSyncStatus(), false);
            mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
            mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
            mQuantizationAmount.setSelectedId(mLastQuantSelection, dontSendNotification);
            mLastModeSelection = mProcessor.getNonModParamAsInt(kNMod_LevelManuelMode);
            mMode.setSelectedId(mLastModeSelection, dontSendNotification);
            
            if(mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_Gravity ||
               mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_BPMSync) {
                mControlPanel.setGravetyEnabled(true);
            }
            else{
                mControlPanel.setGravetyEnabled(false);
            }
        }//end if visable
        
        switch (mLastModeSelection) {
            case kDCOp_Bez:
                mManuelControler.setVisible(false);
                mManuelControler.setAlwaysOnTop(false);
                mManuelControler.setInterceptsMouseClicks(false, false);
                mCurve->setInterceptsMouseClicks(true, true);
                mCurve->setAlwaysOnTop(true);
                mCurve->setVisible(true);
                mReset.setVisible(true);
                mResetLable.setVisible(true);
                break;
            case kDCOp_Manual:
                mManuelControler.setVisible(true);
                mManuelControler.setAlwaysOnTop(true);
                mManuelControler.setInterceptsMouseClicks(true, true);
                mCurve->setInterceptsMouseClicks(false, false);
                mCurve->setAlwaysOnTop(false);
                mCurve->setVisible(false);
                mReset.setVisible(false);
                mResetLable.setVisible(false);
                break;
            default:
                break;
        }
    }
}

void TrollLevelDisplay::setComponentVisability(int _controlMode)
{
    mLastModeSelection = _controlMode;
    mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
    mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
    mMode.setSelectedId(mLastModeSelection, dontSendNotification);
    
    switch (mLastModeSelection) {
        case kDCOp_Bez:
            mManuelControler.setVisible(false);
            mManuelControler.setAlwaysOnTop(false);
            mManuelControler.setInterceptsMouseClicks(false, false);
            mCurve->setInterceptsMouseClicks(true, true);
            mCurve->setAlwaysOnTop(true);
            mCurve->setVisible(true);
            mReset.setVisible(true);
            mResetLable.setVisible(true);
            break;
        case kDCOp_Manual:
            mManuelControler.setVisible(true);
            mManuelControler.setAlwaysOnTop(true);
            mManuelControler.setInterceptsMouseClicks(true, true);
            mCurve->setInterceptsMouseClicks(false, false);
            mCurve->setAlwaysOnTop(false);
            mCurve->setVisible(false);
            mReset.setVisible(false);
            mResetLable.setVisible(false);
            break;
        default:
            break;
    }
}
