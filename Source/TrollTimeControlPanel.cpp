/*
  ==============================================================================

    TrollTimeControlPanel.cpp
    Created: 23 Apr 2019 9:59:50am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "TrollTimeControlPanel.h"
#include "DTUsedParameters.h"
#include "DTHelperFunctions.h"
#include "DTModulationDefines.h"
#include "TrollTimeDisplay.h"
#include "MainDisplayPanel.h"

//==============================================================================
TrollTimeControlPanel::TrollTimeControlPanel(DelayTrollAudioProcessor& p):
PanelBase(p),
mGravetyImage(ImageCache::getFromMemory(BinaryData::GearB2_png, BinaryData::GearB2_pngSize)),
mMinTimeImage(ImageCache::getFromMemory(BinaryData::GearB1_png, BinaryData::GearB1_pngSize)),
mMaxTimeImage(ImageCache::getFromMemory(BinaryData::GearB3_png, BinaryData::GearB3_pngSize)),
mGravety(p.tree, DTParameterID[kParam_GravetyExpo], DTParameterID[kParam_GravetyExpo], mGravetyImage),
mMinTime(p.tree, DTParameterID[kParam_MinDelayTime], DTParameterName[kParam_MinDelayTime], mMinTimeImage),
mMaxTime(p.tree, DTParameterID[kParam_MaxDelayTime], DTParameterName[kParam_MaxDelayTime], mMaxTimeImage),
mMinTimeLabel(" ms", mMinTime.getValueObject()),
mMaxTimeLabel(" ms", mMaxTime.getValueObject()),
mMinBPMManager(p.getBPMManager(0)),
mMaxBPMManager(p.getBPMManager(1)),
mMinSyncLabel(mMinBPMManager.mBPMLabelInfo),
mMaxSyncLabel(mMaxBPMManager.mBPMLabelInfo),
mLeftLabelImage(ImageCache::getFromMemory(BinaryData::SmallDisplay1_png, BinaryData::SmallDisplay1_pngSize)),
mRightLabelImage(ImageCache::getFromMemory(BinaryData::SmallDisplay2_png, BinaryData::SmallDisplay2_pngSize)),
mLeftLabelGlImage(ImageCache::getFromMemory(BinaryData::SmallDisplay1Gl_png, BinaryData::SmallDisplay1Gl_pngSize)),
mRightLabelGlImage(ImageCache::getFromMemory(BinaryData::SmallDisplay2Gl_png, BinaryData::SmallDisplay2Gl_pngSize))
{
    //set up gui divisions
    setSize(TROLL_CONTROL_PANEL_WIDTH, TROLL_CONTROL_PANEL_HEIGHT);
    Array<Rectangle<int>> boxes = DTsetSubBox(getLocalBounds(), 4, 3);
    
    bool syncMode = mProcessor.getSyncStatus();
    
    //set up mintime slider
    mMinTime.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    //mMinTime.setBounds(boxes[4].withBottom(boxes[8].getBottom()));
    mMinTime.setTopLeftPosition(1, getHeight() - mMinTimeImage.getWidth()-2);
    mMinTime.setModulatableParameter(true, p.getAssignableParameters()[kAss_MinTime], &p);
    mMinTime.setRotaryParameters(6.27f, 7.65f, true);
    mMinTime.setGhostTransform(-52.f, 60.f);
    mMinTime.setArcRadiusMult(1.67f);
    mMinTime.setDoubleClickReturnValue(false, 0.0);
    addAndMakeVisible(&mMinTime);
    
    
    //set up min time label
    mLeftLabelBack.setSize(mLeftLabelImage.getWidth(), mLeftLabelImage.getHeight());
    mLeftLabelBack.setImage(mLeftLabelImage);
    mLeftLabelBack.setTopLeftPosition(50, 0);
    mLeftLabelBack.setBoundingBox(mLeftLabelBack.getBounds().toFloat());
    addAndMakeVisible(&mLeftLabelBack);
    //mMinTimeLabel.setBounds(boxes[0].translated(30, 0));
    mMinTimeLabel.setBounds(mLeftLabelBack.getBoundsInParent());
    mMinTimeLabel.setEditable(false);
    mMinTimeLabel.setJustificationType(Justification::centred);
    mMinTimeLabel.setOverriden(false);
    mMinTimeLabel.setVisible(!syncMode);
    //mMinTimeLabel.setColour(juce::Label::outlineColourId, DTColourIds::TextGlowTrans);
    addChildComponent(&mMinTimeLabel);
    
    
    //mMinSyncLabel.setBounds(mMinTimeLabel.getBounds());
    mMinSyncLabel.setBounds(mMinTimeLabel.getBounds());
    mMinSyncLabel.resized();
    mMinSyncLabel.setVisible(syncMode);
    addChildComponent(&mMinSyncLabel);
    
    mLeftLabelGl.setSize(mLeftLabelGlImage.getWidth(), mLeftLabelGlImage.getHeight());
    mLeftLabelGl.setImage(mLeftLabelGlImage);
    mLeftLabelGl.setCentrePosition(mLeftLabelBack.getBoundsInParent().getCentre());
    mLeftLabelGl.setBoundingBox(mLeftLabelGl.getBounds().toFloat());
    addAndMakeVisible(&mLeftLabelGl);
    
    
    //set up gravety slider
    //mGravety.setSliderStyle(Slider::LinearHorizontal);
    //mGravety.setBounds(boxes[5].withBottom(getHeight()).withRight(boxes[6].getRight()));
    mGravety.setDoubleClickReturnValue(false, 1.0);
    mGravety.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mGravety.setCentrePosition(getLocalBounds().getCentreX()-1, getHeight()-(mGravetyImage.getWidth()/2)-2);
    mGravety.setModulatableParameter(true, p.getAssignableParameters()[kAss_Gravity], &p);
    mGravety.setIsGhostReversed(true);
    addAndMakeVisible(&mGravety);
    //DBG("Start"<<mGravety.getRotaryParameters().startAngleRadians);
    //DBG("end"<<mGravety.getRotaryParameters().endAngleRadians);
    mGravety.setRotaryParameters(5.f, 7.5f, true);
    mGravety.setGhostTransform(0.f,90.f);
        
    int mode = p.getNonModParamAsInt(kNMod_TimeControlMode);
    
    if(mode == kTimeOp_BPMSync || mode == kTimeOp_Gravity)
        mGravety.setEnabled(true);
    else
        mGravety.setEnabled(false);
    
    //at some point I need to get this to process  gravety
    mGravety.onDragStart = [this]()
    {
        mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);
        mGravety.startedDragging();
        dynamic_cast<MainDisplayPanel*>(getParentComponent())->setDisplayTimer(true, 20);
    };
    mGravety.onDragEnd = [this]()
    {
        mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);
        mGravety.stoppedDragging();
        dynamic_cast<MainDisplayPanel*>(getParentComponent())->setDisplayTimer(false, -1);
    };
    mGravety.onValueChange = [this]()
    {
        
        if(!mGravety.isMouseButtonDown()){
            //DBG("not Dragging");
            return;
        }
        
        if(mProcessor.getLoadingStatus().load(std::memory_order_acquire))
            return;
        
//        if(!(mProcessor.getNonModParamAsInt(kNMod_TimeManuelMode) == kTimeOp_Gravity ||
//           mProcessor.getNonModParamAsInt(kNMod_TimeManuelMode) == kTimeOp_BPMSync)){
//            return;
//        }
        
        if(mProcessor.getAssignableParameters()[kAss_Gravity]->isAttached)
        {
            mProcessor.updateModulations();
        }
        else{
            mProcessor.getAssignableParameters()[kAss_Gravity]->setValue(static_cast<float>(mGravety.getValue()));
            DTWaitForUpdate(mProcessor.getAtomicPauseStatus(), mProcessor.getAtomicUpdateStatus());
            mProcessor.getAtomicTimeProcessor().updateMult();
        }
    };
    
    //set up max time slider
    mMaxTime.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    //mMaxTime.setBounds(boxes[7].withBottom(getHeight()));
    mMaxTime.setTopRightPosition(getWidth()-5, getHeight() - mMaxTimeImage.getWidth()-2);
    mMaxTime.setModulatableParameter(true, p.getAssignableParameters()[kAss_MaxTime], &p);
    mMaxTime.setRotaryParameters(6.25f, 4.92f, true);
    mMaxTime.setGhostTransform(+52.f, 60.f);
  //  mMaxTime.setArcRadiusMult(1.67f);
    mMaxTime.setArcRadiusMult(1.6f);
    mMaxTime.setIsSliderReversed(true);
    mMaxTime.setDoubleClickReturnValue(false, 2000.0);
    //mMaxTime.setIsGhostReversed(true);
    addAndMakeVisible(&mMaxTime);
    
    //set up the max time lavel
    mRightLabelBack.setSize(mRightLabelImage.getWidth(), mRightLabelImage.getHeight());
    mRightLabelBack.setImage(mRightLabelImage);
    mRightLabelBack.setTopRightPosition(getWidth()-50, 0);
    mRightLabelBack.setBoundingBox(mRightLabelBack.getBounds().toFloat());
    addAndMakeVisible(&mRightLabelBack);
    
    mMaxTimeLabel.setJustificationType(Justification::centred);
    //mMaxTimeLabel.setBounds(boxes[3].translated(-30, 0));
    mMaxTimeLabel.setBounds(mRightLabelBack.getBoundsInParent());
    mMaxTimeLabel.setEditable(false);
    mMaxTimeLabel.setOverriden(false);
    mMaxTimeLabel.setVisible(!syncMode);
    addChildComponent(&mMaxTimeLabel);
    
    mMaxSyncLabel.setBounds(mMaxTimeLabel.getBounds());
    mMaxSyncLabel.resized();
    mMaxSyncLabel.setVisible(syncMode);
    addChildComponent(mMaxSyncLabel);
    
    mRightLabelGl.setSize(mRightLabelGlImage.getWidth(), mRightLabelGlImage.getHeight());
    mRightLabelGl.setImage(mRightLabelGlImage);
    mRightLabelGl.setCentrePosition(mRightLabelBack.getBoundsInParent().getCentre());
    mRightLabelGl.setBoundingBox(mRightLabelGl.getBounds().toFloat());
    addAndMakeVisible(&mRightLabelGl);
    
    
    //update bar fractions to determin how to display sliders
   // DBG("Time Sync Mode: "<<static_cast<int>(syncMode)<<" ======================================================================");
    
    if(syncMode){
        mIntervalValue = mMaxBPMManager.getNoteFractionsInMS();
        NormalisableRange<double> quantRange(DTParameterNormalRanges[kParam_MinDelayTime].getRange().getStart(), DTParameterNormalRanges[kParam_MinDelayTime].getRange().getEnd(), mIntervalValue);
        mMinTime.setNormalisableRange(quantRange);
        mMaxTime.setNormalisableRange(quantRange);
        mMinTime.setValue(static_cast<double>(mMinBPMManager.getQuantTime().getValue()), dontSendNotification);
        mMaxTime.setValue(static_cast<double>(mMaxBPMManager.getQuantTime().getValue()), dontSendNotification);
    }
    else{
        mIntervalValue = 0.0;
        NormalisableRange<double> normRange(DTParameterNormalRanges[kParam_MinDelayTime].getRange().getStart(), DTParameterNormalRanges[kParam_MinDelayTime].getRange().getEnd(), mIntervalValue, DTParameterNormalRanges[kParam_MinDelayTime].skew);
        mMinTime.setNormalisableRange(normRange);
        mMaxTime.setNormalisableRange(normRange);
        mMinTime.setValue(*p.tree.getRawParameterValue(DTParameterID[kParam_MinDelayTime]), dontSendNotification);
        mMaxTime.setValue(*p.tree.getRawParameterValue(DTParameterID[kParam_MaxDelayTime]), dontSendNotification);
    }
    
    mMinTime.onDragStart = [this](){mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);};
    mMinTime.onDragEnd = [this](){mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);};
    mMinTime.onValueChange = [this](){
        
        if(!mMinTime.isMouseButtonDown()){
            //DBG("not Dragging");
            return;
        }
        
        if(mProcessor.getLoadingStatus().load(std::memory_order_acquire))
            return;
        
        double minval;
        //DBG("GUI handle");
        //warning if the user trys to make a min time higher thena a max time
        if(mMinTime.getValue() >= mMaxTime.getValue()){
            mAttString.clear();
            mAttString.append("Cannot be greater than the last tap time!", mFont,  DTColourIds::PopUpText);
            mBubble.showAt(&mMinTime, mAttString, 150);
        }
        
        if(mProcessor.getSyncStatus()){
            //constrain the min value
            
            minval = mMinTime.getValue() >= mMaxTime.getValue() ? mMaxTime.getValue() - mIntervalValue : mMinTime.getValue();
            
            if(mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_QuantizeDelays){ //is in quant mode
                if(mMinTime.getValue() > mMaxTime.getValue() - mProcessor.findMinRange()){
                    minval = jmax(0.0, mMaxTime.getValue() - mProcessor.findMinRange());
                }
            }
            
            //convert to note fraction using the interval value
            int newnotfraction = roundToInt(static_cast<float>(minval/mIntervalValue));
            mMinBPMManager.setNoteFractions(static_cast<float>(newnotfraction));
            mMinTime.setValue(minval, sendNotificationSync);
            mMinSyncLabel.repaint();
            mProcessor.getAtomicHord().setHordNoteFractionRange(newnotfraction, mMaxBPMManager.getNoteFractionInt());
        }
        else{
            minval = mMinTime.getValue() >= mMaxTime.getValue() ? mMaxTime.getValue() - mProcessor.getAtomicHord().getAtomicHordSize().load(std::memory_order_acquire) : mMinTime.getValue();
            mMinTime.setValue(minval, sendNotificationSync);
            //mProcessor.getAssignableParameters()[kAss_MinTime]->sharedOrigninalValue = minval;
            mMinTimeLabel.repaint();
        }
        
        if(!mProcessor.getAssignableParameters()[kAss_MinTime]->isAttached)
            mProcessor.getAssignableParameters()[kAss_MinTime]->setValue(static_cast<float>(minval));
        
        
        
        updateTimes(minval, mMaxTime.getValue());
    };
    
    mMaxTime.onDragStart = [this](){mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);};
    mMaxTime.onDragEnd = [this](){mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);};
    mMaxTime.onValueChange = [this](){
      
        if(!mMaxTime.isMouseButtonDown()){
            return;
        }
        
        if(mProcessor.getLoadingStatus().load(std::memory_order_acquire))
            return;
        
        double maxval;
        
        //warning if the user trys to make a max time lower thena a min time
        if(mMaxTime.getValue() <= mMinTime.getValue()){
            mAttString.clear();
            mAttString.append("Cannot be less than the first tap time!", mFont,  DTColourIds::PopUpText);
            mBubble.showAt(&mMaxTime, mAttString, 150);
        }
        
        if(mProcessor.getSyncStatus()){
            maxval = mMaxTime.getValue() <= mMinTime.getValue() ? mMinTime.getValue() + mIntervalValue : mMaxTime.getValue();
            
            if(mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_QuantizeDelays){ //is in quant mode
                if(mMaxTime.getValue() < mMinTime.getValue() + mProcessor.findMinRange()){
                    maxval = jmin(mMaxTime.getMaximum(), mMinTime.getValue() + mProcessor.findMinRange());
                }
            }
            
            
            int newnotfraction = roundToInt(static_cast<float>(maxval/mIntervalValue));
            mMaxBPMManager.setNoteFractions(static_cast<float>(newnotfraction));
            mMaxTime.setValue(maxval, sendNotificationSync);
            mMaxSyncLabel.repaint();
            mProcessor.getAtomicHord().setHordNoteFractionRange(mMinBPMManager.getNoteFractionInt(), newnotfraction);
        }
        else{
            maxval = mMaxTime.getValue() <= mMinTime.getValue() ?
                     mMinTime.getValue() + mProcessor.getAtomicHord().getAtomicHordSize().load(std::memory_order_acquire) :
                     mMaxTime.getValue();
            //jassert(maxval >= 2.0);
            mMaxTime.setValue(maxval, sendNotificationSync);
            mMaxTimeLabel.repaint();
        }
        
        if(!mProcessor.getAssignableParameters()[kAss_MaxTime]->isAttached)
            mProcessor.getAssignableParameters()[kAss_MaxTime]->setValue(static_cast<float>(maxval));
        
        updateTimes(mMinTime.getValue(), maxval);
    };
    
    mBubble.setColour(BubbleMessageComponent::ColourIds::backgroundColourId, DTColourIds::PopUpBG);
    mBubble.setColour(BubbleMessageComponent::ColourIds::outlineColourId, DTColourIds::PopUpText);
    mFont = Font("Courier New", 18.00f, Font::bold);
    addChildComponent(&mBubble);
    
}

TrollTimeControlPanel::~TrollTimeControlPanel()
{
    
}


void TrollTimeControlPanel::setBPMModeAtomic(bool _shouldBPMModeActivate, bool _shouldTimeUpdate)
{
    if(_shouldBPMModeActivate)
    {
        //set the managers off the sliders
        mMinBPMManager.setNoteFractionsFromTime(mMinTime.getValue());
        mMaxBPMManager.setNoteFractionsFromTime(mMaxTime.getValue());
        
        checkSpacing();
        
        //get the current slider values
        double mintime = mMinTime.getValue();
        double maxTime = mMaxTime.getValue();
        
        //update the hord and interval values
        mProcessor.getAtomicHord().setHordNoteFractionRange(mMinBPMManager.getNoteFractionInt(), mMaxBPMManager.getNoteFractionInt());
        
        mIntervalValue = mMaxBPMManager.getNoteFractionsInMS();
        jassert(mMaxBPMManager.getNoteFractionsInMS() == mMinBPMManager.getNoteFractionsInMS());
        
        NormalisableRange<double> quantRange(DTParameterNormalRanges[kParam_MinDelayTime].getRange().getStart(),
                                             DTParameterNormalRanges[kParam_MinDelayTime].getRange().getEnd(),
                                             mIntervalValue);
        //set a new normalizable range
        mMinTime.setNormalisableRange(quantRange);
        mMaxTime.setNormalisableRange(quantRange);
        //get the target value for the sliders and set it
        double newMinTime = static_cast<double>(mMinBPMManager.getQuantTime().getValue());
        double newMaxTime = static_cast<double>(mMaxBPMManager.getQuantTime().getValue());
        
        mMinTime.setValue(newMinTime, dontSendNotification);
        mMaxTime.setValue(newMaxTime, dontSendNotification);
        
        //check to see if the time processor needs updating
        if(_shouldTimeUpdate &&
           (var(mintime) != mMinBPMManager.getQuantTime().getValue() ||
           var(maxTime) != mMaxBPMManager.getQuantTime().getValue())){
            updateTimes(newMinTime, newMaxTime);
        }
        
        //DBG("the notefraction of max at setting bpm Mode is: "<< mMaxTime.getValue()/mIntervalValue);
        
        mMinSyncLabel.setVisible(true);
        mMaxSyncLabel.setVisible(true);
        mMinTimeLabel.setVisible(false);
        mMaxTimeLabel.setVisible(false);
        
    }
    else{
        //DBG("BPM MODE SET TO FALSE: " << mMaxTime.getValue()<<" : "<< static_cast<float>(mMaxTime.getValueObject().getValue()));
        //DBG("Initial Slider Time: "<<mMinTime.getValue()<<" : "<<mMaxTime.getValue());
        //DBG("Initial Interval value: "<<mIntervalValue);
        mMinSyncLabel.setVisible(false);
        mMaxSyncLabel.setVisible(false);
        mMinTimeLabel.setVisible(true);
        mMaxTimeLabel.setVisible(true);
        mIntervalValue = 0.0;
        
        NormalisableRange<double> normalRange(DTParameterNormalRanges[kParam_MinDelayTime].getRange().getStart(),
                                              DTParameterNormalRanges[kParam_MinDelayTime].getRange().getEnd(),
                                              DTParameterNormalRanges[kParam_MinDelayTime].interval,
                                              DTParameterNormalRanges[kParam_MinDelayTime].skew);
        //sets a new normalizable range
        mMinTime.setNormalisableRange(normalRange);
        mMaxTime.setNormalisableRange(normalRange);
        float minVal = mProcessor.tree.getParameter(DTParameterID[kParam_MinDelayTime])->getValue();
        float maxVal = mProcessor.tree.getParameter(DTParameterID[kParam_MaxDelayTime])->getValue();
        //DBG("Slider position from processor tree: "<< minVal<<" : "<<maxVal);
        double newMinTime = normalRange.convertFrom0to1(static_cast<double>(minVal));
        double newMaxTime = normalRange.convertFrom0to1(static_cast<double>(maxVal));
        mMinTime.setValue(newMinTime, dontSendNotification);
        mMaxTime.setValue(newMaxTime, dontSendNotification);
        
        //DBG("Final Slider Time: "<<mMinTime.getValue()<<" : "<<mMaxTime.getValue());
        mMinTime.repaint();
        mMaxTime.repaint();
    }
}
void TrollTimeControlPanel::updateTimes(double _minTime, double _maxTime){
    if(mProcessor.getAssignableParameters()[kAss_MinTime]->isAttached || mProcessor.getAssignableParameters()[kAss_MaxTime]->isAttached){
        mProcessor.updateModulations();
    }
    else{
        DTWaitForUpdate(mProcessor.getAtomicPauseStatus(), mProcessor.getAtomicUpdateStatus());
        mProcessor.getAtomicTimeProcessor().updateRange(_minTime, _maxTime);
        if(mProcessor.getNonModParamAsInt(kNMod_TimeControlMode) == kTimeOp_QuantizeDelays){
            mProcessor.getAtomicTimeProcessor().hordSizeUpdate(mProcessor.getAtomicTimeProcessor().getMults().size());
        }
    }
}

void TrollTimeControlPanel::setAtomicDenominator()
{
 
    if(!mProcessor.getSyncStatus())
        return;
    
    int selection = static_cast<int>(mProcessor.getNonModParams()[kNMod_QuantizationSelection]->getValue());
    
    //update the managers and interval value
    mMinBPMManager.updateQuantizationMult(selection);
    mMaxBPMManager.updateQuantizationMult(selection);
    

    mIntervalValue = mMaxBPMManager.getNoteFractionsInMS();


    checkSpacing();
    
    double minVal, maxVal, newMinVal, newMaxVal;
    //get the current slider values
    minVal = mMinTime.getValue();
    maxVal = mMaxTime.getValue();
    //get the target slider values
    newMinVal = static_cast<double>(mMinBPMManager.getQuantTime().getValue());
    newMaxVal = static_cast<double>(mMaxBPMManager.getQuantTime().getValue());
    //calculate the note fractions from the target slider values
    int newMinNoteFraction = roundToInt(static_cast<float>(newMinVal/mIntervalValue));
    int newMaxNoteFraction = roundToInt(static_cast<float>(newMaxVal/mIntervalValue));
    //get the old and new range lengths
    int oldRangeLength = mProcessor.getAtomicHord().getHordNoteFractionRang().getLength();
    mProcessor.getAtomicHord().setHordNoteFractionRange(newMinNoteFraction, newMaxNoteFraction);
    int newRangeLength = mProcessor.getAtomicHord().getHordNoteFractionRang().getLength();
    
    //set a new normalizable range
    NormalisableRange<double> quantRange(mMinTime.getRange().getStart(), mMinTime.getRange().getEnd(), mIntervalValue);
    
    mMinTime.setNormalisableRange(quantRange);
    mMaxTime.setNormalisableRange(quantRange);
    
    mMinTime.setValue(newMinVal, dontSendNotification);
    mMaxTime.setValue(newMaxVal, dontSendNotification);
    
    //check to see if the times need updating
    int choice = mProcessor.getNonModParamAsInt(kNMod_TimeControlMode);
    if( choice != kTimeOp_QuantizeDelays){
        if( minVal != newMinVal ||
           maxVal != newMaxVal){
            updateTimes(newMinVal, newMaxVal);
        }
    }
    else if(choice == kTimeOp_QuantizeDelays && oldRangeLength != newRangeLength){
        if( minVal != newMinVal ||
           maxVal != newMaxVal){
            updateTimes(newMinVal, newMaxVal);
        }
        else{
            mProcessor.getAtomicTimeProcessor().resizeQuant(newRangeLength + 1);
            mProcessor.getAtomicTimeProcessor().hordSizeUpdate(mProcessor.getAtomicTimeProcessor().getMults().size());
        }
    }
   
    mMinSyncLabel.repaint();
    mMaxSyncLabel.repaint();
    
}
void TrollTimeControlPanel::setLabels()
{
    if(mProcessor.getSyncStatus()){
        mIntervalValue = mMaxBPMManager.getNoteFractionsInMS();
        jassert(mMaxBPMManager.getNoteFractionsInMS() == mMinBPMManager.getNoteFractionsInMS());
        
        NormalisableRange<double> quantRange(DTParameterNormalRanges[kParam_MinDelayTime].getRange().getStart(),
                                             DTParameterNormalRanges[kParam_MinDelayTime].getRange().getEnd(),
                                             mIntervalValue);
        
        mMinTime.setNormalisableRange(quantRange);
        mMaxTime.setNormalisableRange(quantRange);
        
        mMinSyncLabel.setVisible(true);
        mMaxSyncLabel.setVisible(true);
        mMinTimeLabel.setVisible(false);
        mMaxTimeLabel.setVisible(false);
        
    }
    else{
        mMinSyncLabel.setVisible(false);
        mMaxSyncLabel.setVisible(false);
        mMinTimeLabel.setVisible(true);
        mMaxTimeLabel.setVisible(true);
        mIntervalValue = 0.0;
        
        NormalisableRange<double> normalRange(DTParameterNormalRanges[kParam_MinDelayTime].getRange().getStart(),
                                              DTParameterNormalRanges[kParam_MinDelayTime].getRange().getEnd(),
                                              DTParameterNormalRanges[kParam_MinDelayTime].interval,
                                              DTParameterNormalRanges[kParam_MinDelayTime].skew);
        
        mMinTime.setNormalisableRange(normalRange);
        mMaxTime.setNormalisableRange(normalRange);
    }
    
    int mode = mProcessor.getNonModParamAsInt(kNMod_TimeControlMode);
    
    if(mode == kTimeOp_BPMSync || mode == kTimeOp_Gravity)
        setGravetyEnabled(true);
    else
        setGravetyEnabled(false);
}


void TrollTimeControlPanel::checkSpacing()
{
    if( mMaxBPMManager.getNoteFractions().getValue() <= mMinBPMManager.getNoteFractions().getValue())
           mMaxBPMManager.setNoteFractions(static_cast<float>(mMinBPMManager.getNoteFractions().getValue()) + 1.f);
       
       if( mMinBPMManager.getNoteFractions().getValue() >= mMaxBPMManager.getNoteFractions().getValue())
           mMinBPMManager.setNoteFractions(static_cast<float>(mMaxBPMManager.getNoteFractions().getValue()) - 1.f);
       
       if(mMinBPMManager.getNoteFractions().getValue() < var(0.0))
           mMinBPMManager.setNoteFractions(0.f);
}
