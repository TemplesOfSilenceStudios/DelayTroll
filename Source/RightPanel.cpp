/*
  ==============================================================================

    RightPanel.cpp
    Created: 23 Apr 2019 9:12:03am
    Author:  Sam Ferguson

  ==============================================================================
*/


#include "RightPanel.h"
#include "DTUsedParameters.h"
#include "DTModulationDefines.h"
#include "DTHelperFunctions.h"
//==============================================================================
RightPanel::RightPanel(DelayTrollAudioProcessor& p):
PanelBase(p),
mWetDryImage(ImageCache::getFromMemory(BinaryData::GearA2_png, BinaryData::GearA2_pngSize)),
mSmoothFactorImage(ImageCache::getFromMemory(BinaryData::GearA1_png, BinaryData::GearA1_pngSize)),
mWetDry(p.tree, DTParameterID[kParam_WetDry], DTParameterName[kParam_WetDry], mWetDryImage),
mSmoothFactor(p.tree, DTParameterID[kParam_SmoothFactor], DTParameterName[kParam_SmoothFactor], mSmoothFactorImage)
{
    setSize(RIGHT_PANEL_WIDTH, RIGHT_PANEL_HEIGHT);
    
    Array<Rectangle<int>> subBoxes = DTsetSubBox(getLocalBounds(), 1, 2);
    
    //mSmoothFactor.setBounds(DTRemoveLabelBumper(subBoxes[0]));
    mSmoothFactor.setCentrePosition(subBoxes[1].getCentre().translated(0, 10));
    mSmoothFactor.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mSmoothFactor.setIsGhostReversed(true);
    mSmoothFactor.setDoubleClickReturnValue(false, 0.0);
    addAndMakeVisible(&mSmoothFactor);
    
    mSmoothFactor.onDragStart = [this](){mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);};
    mSmoothFactor.onValueChange = [this](){
        if(!mSmoothFactor.isMouseOverOrDragging())
            return;
        
        mAttString.clear();
        //DBG("SmoothFactor: "<< mSmoothFactor.getValue());
        float disVal = DTParameterNormalRanges[kParam_SmoothFactor].convertTo0to1(mSmoothFactor.getValue()) *100.f;
        mAttString.append(String(roundToInt(disVal)), mFont,  DTColourIds::PopUpText);
        mSuffix = "%";
        mAttString.append(mSuffix, mFont,  DTColourIds::PopUpText);
        mBubble.showAt(&mSmoothFactor, mAttString, 150);
        
        if(!mProcessor.getAssignableParameters()[kAss_SmoothFactor]->isAttached)
            mProcessor.getAssignableParameters()[kAss_SmoothFactor]->setValue(mSmoothFactor.getValue());
        else
            mProcessor.updateModulations();
    };
    
    
    mSmoothFactor.onDragEnd = [this](){mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);};
    mSmoothFactor.setModulatableParameter(true, p.getAssignableParameters()[kAss_SmoothFactor], &p);
    
    mWetDry.onDragStart = [this](){mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);};
    
    mWetDry.onValueChange = [this](){
        if(!mWetDry.isMouseOverOrDragging())
            return;
        
        mAttString.clear();
        mAttString.append(String(roundToInt(mWetDry.getValue()*100.0)), mFont,  DTColourIds::PopUpText);
        mSuffix = "% Wet";
        mAttString.append(mSuffix, mFont,  DTColourIds::PopUpText);
        mBubble.showAt(&mWetDry, mAttString, 150);
        
        if(!mProcessor.getAssignableParameters()[kAss_WetDry]->isAttached)
            mProcessor.getAssignableParameters()[kAss_WetDry]->setValue(mWetDry.getValue());
        else
            mProcessor.updateModulations();
    };
    
    mWetDry.onDragEnd = [this](){mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);};
    //mWetDry.setBounds(DTRemoveLabelBumper(subBoxes[1]));
    mWetDry.setCentrePosition(subBoxes[0].getCentre().translated(0, 10));
    mWetDry.setModulatableParameter(true, p.getAssignableParameters()[kAss_WetDry], &p);
    mWetDry.setDoubleClickReturnValue(false, 0.5);
    
    
    addAndMakeVisible(&mWetDry);
    
    mBubble.setColour(BubbleMessageComponent::ColourIds::backgroundColourId, DTColourIds::PopUpBG);
    mBubble.setColour(BubbleMessageComponent::ColourIds::outlineColourId, DTColourIds::PopUpText);
    mFont = Font("Vilane", 40.00f, Font::bold);
    addChildComponent(&mBubble);
    
}

RightPanel::~RightPanel()
{
}

void RightPanel::paint (Graphics& g)
{

}

void RightPanel::resized()
{

}
