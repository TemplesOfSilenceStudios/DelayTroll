/*
  ==============================================================================

    TrollDisplayPanelBase.cpp
    Created: 23 Apr 2019 9:43:20am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TrollDisplayPanelBase.h"
#include "DTUsedParameters.h"
#include "DTHelperFunctions.h"

//==============================================================================
TrollDisplayPanelBase::TrollDisplayPanelBase(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel, int _manualParamIndex, DTManualControl::DTManualModes _manualMode, int _topManualControlIndex, int _bottomManualControlIndex):
PanelBase(p),
mDisplayComponent(p.getAtomicTrolls(), p.getAtomicTimeProcessor(), &p),
mModeImage(ImageCache::getFromMemory(BinaryData::DropDownA1_png, BinaryData::DropDownA1_pngSize)),
mQuantImage(ImageCache::getFromMemory(BinaryData::DropDownA2_png, BinaryData::DropDownA2_pngSize)),
mModeGlImage(ImageCache::getFromMemory(BinaryData::DropDownA1Gl_png, BinaryData::DropDownA1_pngSize)),
mQuantGlImage(ImageCache::getFromMemory(BinaryData::DropDownA2Gl_png, BinaryData::DropDownA2_pngSize)),
mResetImageOn(ImageCache::getFromMemory(BinaryData::DeleteDownSmall_png, BinaryData::DeleteDownSmall_pngSize)),
mResetImageOff(ImageCache::getFromMemory(BinaryData::DeleteUpSmall_png, BinaryData::DeleteUpSmall_pngSize)),
mReset("Reset",mResetImageOn,mResetImageOff),
mResetLableImage(ImageCache::getFromMemory(BinaryData::ResetBezLabel_png, BinaryData::ResetBezLabel_pngSize)),
mBypassImageOn(ImageCache::getFromMemory(BinaryData::SwitchAOff_png, BinaryData::SwitchAOff_pngSize)),
mBypassImageOff(ImageCache::getFromMemory(BinaryData::SwitchAOn_png, BinaryData::SwitchAOn_pngSize)),
mMode(p.getNonModParams()[_manualParamIndex], DTNonModParamNames[_manualParamIndex], mModeImage),
mManuelControler(&p, _topManualControlIndex, _manualMode, _bottomManualControlIndex),
mQuantizationAmount(p.getNonModParams()[kNMod_QuantizationSelection], DTNonModParamNames[kNMod_QuantizationSelection], mQuantImage),
mControlPanel(_ControlPanel)
{
    
    setSize(TROLL_DISPLAY_PANEL_WIDTH, TROLL_DISPLAY_PANEL_HEIGHT);
    

    //inner box to display the trolls
    mDisplayComponent.setTopLeftPosition(TROLL_DISPLAY_COMP_HORZ_BUMPER, TROLL_DISPLAY_COMPONENT_BUMPER);
    addAndMakeVisible(mDisplayComponent);
    
//    mControlPanel->setTopLeftPosition(0, mDisplayComponent.getBottom());
//    addAndMakeVisible(*mControlPanel);
    
    //mProcessor.updateTotalFractionSlots();
 
    mInnerDisplayArea.setBounds(TROLL_DISPLAY_COMP_HORZ_BUMPER, TROLL_DISPLAY_COMPONENT_BUMPER, mDisplayComponent.getWidth(), mDisplayComponent.getHeight());
    
    mTextRect = Rectangle<int>(mInnerDisplayArea.getRight(), 0, TROLL_DISPLAY_GRID_LABEL_BUMPER,6);

    setInterceptsMouseClicks(true, true);
    
    Rectangle<int> topBar(getX(), getY(), TROLL_DISPLAY_PANEL_WIDTH, mInnerDisplayArea.getY());
    
    Array<Rectangle<int>> topButtons = DTsetSubBox(topBar, 3, 1);
    
    mMode.setTopLeftPosition(20, 2);
    
    addAndMakeVisible(&mMode);
    
    mModeGl.setImage(mModeGlImage);
    mModeGl.setSize(mModeGlImage.getWidth(), mModeGlImage.getHeight());
    mModeGl.setCentrePosition(mMode.getBounds().getCentre());
    mModeGl.setBoundingBox(mModeGl.getBounds().toFloat());
    addAndMakeVisible(&mModeGl);
    
    //mReset.setSize(25, 25);
    mReset.setTopLeftPosition(mMode.getRight()+3, 3);
    
    mResetLable.setImage(mResetLableImage);
    mResetLable.setSize(mResetLableImage.getWidth(), mResetLableImage.getHeight());
    mResetLable.setCentrePosition(mReset.getBounds().getCentreX() + mReset.getWidth()/2 + mResetLable.getWidth()/2 + 3, mReset.getBounds().getCentreY()-1);
    mResetLable.setBoundingBox(mResetLable.getBounds().toFloat());
    
    
   
    
    mQuantizationAmount.setTopRightPosition(getWidth() -20, 2);
    
    mQuantizationAmount.addItemList(DTChoiceParameterStrings[kNMod_QuantizationSelection], 1);
    mQuantizationAmount.setSelectedId(static_cast<int>(mProcessor.getNonModParams()[kNMod_QuantizationSelection]->getValue()), dontSendNotification);
    mLastQuantSelection = mQuantizationAmount.getSelectedId();
    
    if(p.getSyncStatus())
        mQuantizationAmount.setEnabled(true);
    else
        mQuantizationAmount.setEnabled(false);
    
    mQuantizationAmount.onChange = [this]() {
        
        if(mQuantizationAmount.getSelectedId() == mLastQuantSelection)
        {
            return;
        }
        else{
            mLastQuantSelection = mQuantizationAmount.getSelectedId();
        }
        
        if(mProcessor.getLoadingStatus().load(std::memory_order_acquire))
            return;
        
        mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);
        
        mControlPanel.setAtomicDenominator();
        mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);
    };
    
    addAndMakeVisible(&mQuantizationAmount);
    
    mQuantGl.setImage(mQuantGlImage);
    mQuantGl.setSize(mQuantGlImage.getWidth(), mQuantGlImage.getHeight());
    mQuantGl.setCentrePosition(mQuantizationAmount.getBounds().getCentre());
    mQuantGl.setBoundingBox(mQuantGl.getBounds().toFloat());
    addAndMakeVisible(&mQuantGl);

    mManuelControler.setTopLeftPosition(mInnerDisplayArea.getX(), mInnerDisplayArea.getY());

    mManuelControler.setInterceptsMouseClicks(true, true);
    
    addChildComponent(mManuelControler);
    
    //startTimer(20);
    
}

TrollDisplayPanelBase::~TrollDisplayPanelBase()
{
    stopTimer();
}

void TrollDisplayPanelBase::paint (Graphics& g)
{
   // g.setColour (Colours::grey);
   // g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

   // g.setColour (Colours::mediumvioletred);
//draw an outline arround the display area
    //g.drawRect(mInnerDisplayArea.expanded(4, 0).withRight(mInnerDisplayArea.getRight() + TROLL_DISPLAY_COMPONENT_BUMPER +2) , 2);

}



