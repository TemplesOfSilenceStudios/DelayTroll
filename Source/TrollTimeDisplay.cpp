/*
  ==============================================================================

    TrollTimeDisplay.cpp
    Created: 23 Apr 2019 9:45:55am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "TrollTimeDisplay.h"
#include "DTHelperFunctions.h"
#include "MainDisplayPanel.h"
#include "DTUsedParameters.h"
#include "DTModulationDefines.h"

//==============================================================================
TrollTimeDisplay::TrollTimeDisplay(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel):
TrollDisplayPanelBase(p, _ControlPanel, kNMod_TimeControlMode, DTManualControl::kManMode_Xeditable, TrollParams::kTroll_DelayTime),
mConstraintControl(&p)
{

//    mTest.setSliderStyle(Slider::SliderStyle::IncDecButtons);
//    //mTest.setSize(200, 50);
//    mTest.setTextBoxStyle (Slider::TextBoxLeft, false, 50, 20);
//    mTest.setBounds(mMode.getBounds().expanded(10, 0));
//    mTest.setTopLeftPosition(mMode.getRight()+3, 0);
//    mTest.setRange(Range<double>(5.0, 420.0), 1.0);
//    mTest.setIncDecButtonsMode(Slider::incDecButtonsDraggable_Horizontal);
//    addAndMakeVisible(&mTest);
//    
//    mTest.onValueChange = [this](){
//        DBG("ValueCHanging");
//    };
    
    mProcessor.addListener(this);
 
    int modeSelection = p.getNonModParamAsInt(kNMod_TimeControlMode);
    
    if(modeSelection == kTimeOp_ManualControl || modeSelection == kTimeOp_QuantizeDelays)
        mManuelControler.setVisible(true);
    else
        mManuelControler.setVisible(false);

    
    mConstraintControl.setTopLeftPosition(mInnerDisplayArea.getX(), mInnerDisplayArea.getY());
    addChildComponent(&mConstraintControl);
    
    if(modeSelection == kTimeOp_Divisitions)
        mConstraintControl.setVisible(true);
    else
        mConstraintControl.setVisible(false);
    

    mMode.addItemList(DTChoiceParameterStrings[kNMod_TimeControlMode], 1);

    mMode.setSelectedId(modeSelection, dontSendNotification);
    mLastModeSelection = modeSelection;
    
    if(modeSelection == kTimeOp_QuantizeDelays)
        mManuelControler.setMode(DTManualControl::kManMode_QuantizedX);
    else
        mManuelControler.setMode(DTManualControl::kManMode_Xeditable);
    
    if(p.getAssignableParameters()[kAss_MinTime]->isAttached || p.getAssignableParameters()[kAss_MaxTime]->isAttached)
        mMode.setItemEnabled(kTimeOp_QuantizeDelays, false);
    

    mMode.onChange = [this](){
        
        if(mMode.getSelectedId() == mLastModeSelection || !isVisible())
        {
            return;
        }
        else{
            mLastModeSelection = mMode.getSelectedId();
        }
        
        mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);
        
        if(mProcessor.getLoadingStatus().load(std::memory_order_acquire))
            return;
        
        
        //mProcessor.getNonModParams()[kNMod_TimeManuelMode]->setValue(var(mMode.getSelectedId()));
        
        auto& atomicTimeProcessor = mProcessor.getAtomicTimeProcessor();
        
        switch (mMode.getSelectedId()) {
            case kTimeOp_Gravity:
                //DBG("SETTING BPM MODE FROM DROPDOWN");
                mControlPanel.setBPMModeAtomic(false);
                DTWaitForUpdate(mProcessor.getAtomicPauseStatus(), mProcessor.getAtomicUpdateStatus());
                atomicTimeProcessor.updateMult();
                timerCallback();
                
                mManuelControler.setVisible(false);
                mConstraintControl.setVisible(false);
                mControlPanel.repaint();
                mQuantizationAmount.setEnabled(false);
                mControlPanel.setGravetyEnabled(true);
                break;
            
            case kTimeOp_ManualControl:
                mControlPanel.setBPMModeAtomic(false);
                mManuelControler.setVisible(true);
                mManuelControler.setMode(DTManualControl::kManMode_Xeditable);
                mConstraintControl.setVisible(false);
                mControlPanel.repaint();
                mQuantizationAmount.setEnabled(false);
                mControlPanel.setGravetyEnabled(false);
                break;
                
            case kTimeOp_Divisitions:
                mControlPanel.setBPMModeAtomic(false);
                DTWaitForUpdate(mProcessor.getAtomicPauseStatus(), mProcessor.getAtomicUpdateStatus());
                atomicTimeProcessor.getConstraintSections().getLast()->setIndexEnd(atomicTimeProcessor.getMults().size()-1);
                mProcessor.getAtomicTimeProcessor().updateMult();
                timerCallback();
               

                mManuelControler.setVisible(false);
                mControlPanel.repaint();
                mConstraintControl.setVisible(true);
                mQuantizationAmount.setEnabled(false);
                mControlPanel.setGravetyEnabled(false);
                break;

            case kTimeOp_BPMSync:
                
                mControlPanel.setBPMModeAtomic(true);
                DTWaitForUpdate(mProcessor.getAtomicPauseStatus(), mProcessor.getAtomicUpdateStatus());
                mProcessor.getAtomicTimeProcessor().updateMult();
                timerCallback();
                

                mControlPanel.repaint();
                mManuelControler.setVisible(false);
                mConstraintControl.setVisible(false);
                mQuantizationAmount.setEnabled(true);
                mControlPanel.setGravetyEnabled(true);
                break;
                
            case kTimeOp_QuantizeDelays:
                if(mProcessor.getAssignableParameters()[kAss_MinTime]->isAttached || mProcessor.getAssignableParameters()[kAss_MaxTime]->isAttached){
                    DBG("switching User to manual: this should never run");
                    mMode.setSelectedId(kTimeOp_ManualControl);
                }
                else{
                    //DBG("Quant fireing");
                    mControlPanel.setBPMModeAtomic(true);
                    DTWaitForUpdate(mProcessor.getAtomicPauseStatus(), mProcessor.getAtomicUpdateStatus());
                    mProcessor.getAtomicTimeProcessor().resizeQuant(mProcessor.getAtomicHord().getHordNoteFractionRang().getLength() + 1);
                    mProcessor.getAtomicTimeProcessor().hordSizeUpdate(mProcessor.getAtomicTimeProcessor().getMults().size());
                    
                    mControlPanel.repaint();
                    mManuelControler.setMode(DTManualControl::kManMode_QuantizedX);
                    mManuelControler.setVisible(true);
                    mConstraintControl.setVisible(false);
                    mQuantizationAmount.setEnabled(true);
                    mControlPanel.setGravetyEnabled(false);
                }
                break;
                
            default:
                break;
        }
         mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);
    };// mode on change
    
    mProcessor.addBPMListener(this);

}

TrollTimeDisplay::~TrollTimeDisplay()
{
    mProcessor.removeListener(this);
    mProcessor.removeBPMListener(this);
}

void TrollTimeDisplay::paint (Graphics& g)
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

void TrollTimeDisplay::loadActivated(DelayTrollAudioProcessor *p)
{
    //DBG("load activated at display");

    mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);
    if (&mProcessor == p && isVisible())
    {
        mControlPanel.setBPMModeAtomic(mProcessor.getSyncStatus(), false);
        mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
        mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
        mQuantizationAmount.setSelectedId(mLastQuantSelection, dontSendNotification);
        mLastModeSelection = mProcessor.getNonModParamAsInt(kNMod_TimeControlMode);
        mMode.setSelectedId(mLastModeSelection, dontSendNotification);
        
        switch (mLastModeSelection) {
            case kTimeOp_Gravity:
                mControlPanel.setGravetyEnabled(true);
                mManuelControler.setVisible(false);
                mConstraintControl.setVisible(false);
                break;
                
            case kTimeOp_ManualControl:
                mManuelControler.setMode(DTManualControl::kManMode_Xeditable);
                mManuelControler.setVisible(true);
                mConstraintControl.setVisible(false);
                mControlPanel.setGravetyEnabled(false);
                break;
                
            case kTimeOp_Divisitions:
                mManuelControler.setVisible(false);
                mConstraintControl.setVisible(true);
                mConstraintControl.atomicMultUpdated(&mProcessor.getAtomicTimeProcessor());
                mControlPanel.setGravetyEnabled(false);
                break;
                
            case kTimeOp_BPMSync:
                mManuelControler.setVisible(false);
                mConstraintControl.setVisible(false);
                mControlPanel.setGravetyEnabled(true);
                break;
                
            case kTimeOp_QuantizeDelays:
                mManuelControler.setMode(DTManualControl::kManMode_QuantizedX);
                mManuelControler.setVisible(true);
                mConstraintControl.setVisible(false);
                mControlPanel.setGravetyEnabled(false);
                break;
                
            default:
                break;
        }
        mControlPanel.repaint();
    }
     mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);
}
void TrollTimeDisplay::setComponentVisability(int _controlMode)
{
    mLastModeSelection = _controlMode;
    mQuantizationAmount.setEnabled(mProcessor.getSyncStatus());
    mLastQuantSelection = mProcessor.getNonModParamAsInt(kNMod_QuantizationSelection);
    mMode.setSelectedId(mLastModeSelection, dontSendNotification);
    
    switch (mLastModeSelection) {
        case kTimeOp_Gravity:
            mControlPanel.setGravetyEnabled(true);
            mManuelControler.setVisible(false);
            mConstraintControl.setVisible(false);
            break;
            
        case kTimeOp_ManualControl:
            mManuelControler.setMode(DTManualControl::kManMode_Xeditable);
            mManuelControler.setVisible(true);
            mConstraintControl.setVisible(false);
            mControlPanel.setGravetyEnabled(false);
            break;
            
        case kTimeOp_Divisitions:
            mManuelControler.setVisible(false);
            mConstraintControl.setVisible(true);
            mConstraintControl.atomicMultUpdated(&mProcessor.getAtomicTimeProcessor());
            mControlPanel.setGravetyEnabled(false);
            break;
            
        case kTimeOp_BPMSync:
            mManuelControler.setVisible(false);
            mConstraintControl.setVisible(false);
            mControlPanel.setGravetyEnabled(true);
            break;
            
        case kTimeOp_QuantizeDelays:
            mManuelControler.setMode(DTManualControl::kManMode_QuantizedX);
            mManuelControler.setVisible(true);
            mConstraintControl.setVisible(false);
            mControlPanel.setGravetyEnabled(false);
            break;
            
        default:
            break;
    }
    
}
void TrollTimeDisplay::BPMUpdated(DelayTrollAudioProcessor *p)
{
    //DBG("bpm update");
    
    if(mMode.getSelectedId()== kTimeOp_QuantizeDelays &&
       (mProcessor.getModulationStatus(kAss_MinTime) ||
        mProcessor.getModulationStatus(kAss_MaxTime))){
        mMode.setSelectedId(kTimeOp_ManualControl);
    }
    
    if(mProcessor.getModulationStatus(kAss_MinTime) ||
       mProcessor.getModulationStatus(kAss_MaxTime)){
        mMode.setItemEnabled(kTimeOp_QuantizeDelays, false);
    }
    else{
        mMode.setItemEnabled(kTimeOp_QuantizeDelays, true);
    }
}
