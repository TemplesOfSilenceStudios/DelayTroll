/*
  ==============================================================================

    TopPanel.cpp
    Created: 22 Apr 2019 4:44:50pm
    Author:  Sam Ferguson

  ==============================================================================
*/


#include "TopPanel.h"
#include "DTUsedParameters.h"
#include "DTModulationDefines.h"
#include "DTHelperFunctions.h"
#include "DTAudioFunctions.h"

//==============================================================================
TopPanel::TopPanel(DelayTrollAudioProcessor& p): PanelBase(p),
mInputGain(p.tree, DTParameterID[kParam_InputLevel], DTParameterName[kParam_InputLevel], *p.getAssignableParameters()[KAss_InputLevel]),
mOutputGain(p.tree, DTParameterID[kParam_OutputLevel], DTParameterName[kParam_OutputLevel], *p.getAssignableParameters()[kAss_OutputLevel]),
mInputMeter(p),
mOutputMeter(p),
mUnderImage(ImageCache::getFromMemory(BinaryData::background_black_png, BinaryData::background_black_pngSize)),
mBackgroundImage(ImageCache::getFromMemory(BinaryData::Top2_png, BinaryData::Top2_pngSize)),
mLogoImage(ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize)),
mLogo("Logo", mLogoImage, mLogoImage),
mNameImage(ImageCache::getFromMemory(BinaryData::DelayTrollName_png, BinaryData::DelayTrollName_pngSize)),
mDialGlImage(ImageCache::getFromMemory(BinaryData::DIalGl_png, BinaryData::DIalGl_pngSize)),
mBypassImageOn(ImageCache::getFromMemory(BinaryData::SwitchAOff_png, BinaryData::SwitchAOff_pngSize)),
mBypassImageOff(ImageCache::getFromMemory(BinaryData::SwitchAOn_png, BinaryData::SwitchAOn_pngSize)),
mBypass(p.tree, DTParameterID[kBParam_SoftClipperBypass], DTParameterName[kBParam_SoftClipperBypass], mBypassImageOff, mBypassImageOn),
mBypassLableImage(ImageCache::getFromMemory(BinaryData::Soft_Clipper_png, BinaryData::Soft_Clipper_pngSize))
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize(TOP_PANEL_WIDTH, TOP_PANEL_HEIGHT);
    
    mUnderlayer.setImage(mBackgroundImage);
    mUnderlayer.setSize(TOP_PANEL_WIDTH, TOP_PANEL_HEIGHT);
    mUnderlayer.setTopLeftPosition(0, 0);
    addAndMakeVisible(&mUnderlayer);

    Array<Rectangle<int>> subBox = DTsetSubBox(getBounds(), 5, 1);
    
    mBypass.setTopLeftPosition(subBox[4].getX(),subBox[4].getCentreY()-(mBypass.getHeight()/2)+4);
    mLogo.setTopRightPosition(subBox[4].getRight(), subBox[4].getCentreY()-(mLogo.getHeight()/2));
    
    
    mInputGain.setTopRightPosition(getWidth()/2 +6, 8);
    mInputGain.setInterceptsMouseClicks(true, true);
    mInputGain.setDoubleClickReturnValue(false, 0.0);
    
    mOutputGain.setTopLeftPosition(getWidth()/2 -6, 8);
    mOutputGain.setInterceptsMouseClicks(true, true);
    mOutputGain.setDoubleClickReturnValue(false, 0.0);
   
    
    mOutputGain.onDragStart = [this](){mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);};
    mOutputGain.onValueChange = [this](){
        if(!mOutputGain.isMouseOverOrDragging())
            return;
        
        if(!mProcessor.getAssignableParameters()[kAss_OutputLevel]->isAttached)
            mProcessor.getAssignableParameters()[kAss_OutputLevel]->setValue(mOutputGain.getValue());
        else
            mProcessor.updateModulations();
        
        mAttString.clear();
        mAttString.append(String(roundToInt(Decibels::gainToDecibels(mOutputGain.getValue(), -48.0))), mFont,  DTColourIds::PopUpText);
        mAttString.append(mSuffix, mFont,  DTColourIds::PopUpText);
        mBubble.showAt(&mOutputGain, mAttString, 150);
    };
    mOutputGain.onDragEnd = [this](){mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);};
    mInputGain.onDragStart = [this](){mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);};
    mInputGain.onValueChange = [this](){
        if(!mInputGain.isMouseOverOrDragging())
            return;
        
        if(!mProcessor.getAssignableParameters()[KAss_InputLevel]->isAttached)
            mProcessor.getAssignableParameters()[KAss_InputLevel]->setValue(mInputGain.getValue());
        else
            mProcessor.updateModulations();
        
        mAttString.clear();
        mAttString.append(String(roundToInt(Decibels::gainToDecibels(mInputGain.getValue(),-48.0))), mFont,  DTColourIds::PopUpText);
        mAttString.append(mSuffix, mFont,  DTColourIds::PopUpText);
        mBubble.showAt(&mInputGain, mAttString, 150);
    };
    mInputGain.onDragEnd = [this](){mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);};
    
    
    //mInputMeter.setBounds(subBox[0].getX(), subBox[0].getY(), subBox[0].getWidth()*3, subBox[0].getHeight());
    mInputMeter.setBounds(getLocalBounds().getCentreX() - 40 - 240, 0, 240, getHeight());
    mInputMeter.setParameterID(kParam_InputLevel);
    addAndMakeVisible(&mInputMeter);
    
    //mOutputMeter.setBounds(subBox[5].getX(), subBox[5].getY(), subBox[5].getWidth()*3, subBox[5].getHeight());
    mOutputMeter.setBounds(getLocalBounds().getCentreX() + 40, 0, 240, getHeight());
    mOutputMeter.setParameterID(kParam_OutputLevel);
    addAndMakeVisible(&mOutputMeter);
    
    mBackground.setImage(mBackgroundImage);
    mBackground.setSize(TOP_PANEL_WIDTH, TOP_PANEL_HEIGHT);
    mBackground.setTopLeftPosition(0, 0);
    addAndMakeVisible(&mBackground);
    
    mInOutLabel.setText("In    Out");
    //mInOutLabel.setSize(10, 10);
    mInOutLabel.setFontHeight(11);
    mInOutLabel.setCentrePosition(subBox[2].getCentre().translated(8, 9));
    mInOutLabel.setBoundingBox(mInOutLabel.getBounds().toFloat());
    mInOutLabel.setColour(DTColourIds::DialText);
    addAndMakeVisible(&mInOutLabel);
    
    m48Label.setText("-48   -48");
    //mInOutLabel.setSize(10, 10);
    m48Label.setFontHeight(11);
    m48Label.setCentrePosition(subBox[2].getCentre().translated(2, 18));
    m48Label.setBoundingBox(m48Label.getBounds().toFloat());
    m48Label.setColour(DTColourIds::DialText);
    addAndMakeVisible(&m48Label);
    
    
    
    addAndMakeVisible(&mInputGain);
    addAndMakeVisible(&mOutputGain);
    
    Parallelogram<float> steve(Rectangle<int>(getWidth()/2 - 7,getHeight()/2 - 10,15,15).toFloat());
    mCentre.setRectangle(steve);
    
    mCentre.setCornerSize(Point<float>(8.f, 8.f));
    addAndMakeVisible(&mCentre);
    
    addAndMakeVisible(&mLogo);
    
    mBypass.setClickingTogglesState(true);
    mBypass.setToggleState(p.getParameters()[kBParam_SoftClipperBypass]->getValue(), NotificationType::dontSendNotification);
   
    //DBG("Bypass Toggle state: "<<static_cast<int>(mBypass.getToggleState()));
    //DBG("Processor Soft clipper: "<<p.getParameters()[kBParam_SoftClipperBypass]->getValue());
    
    
    addAndMakeVisible(&mBypass);
    
    mBypassLable.setImage(mBypassLableImage);
    mBypassLable.setSize(mBypassLableImage.getWidth(), mBypassLableImage.getHeight());
    mBypassLable.setTopLeftPosition(subBox[4].getX()+12, subBox[4].getY()+18);
    mBypassLable.setBoundingBox(mBypassLable.getBounds().toFloat());
    addAndMakeVisible(&mBypassLable);
    
    mName.setImage(mNameImage);
    mName.setSize(mNameImage.getWidth(), mNameImage.getHeight());
    mName.setCentrePosition(subBox[0].getCentre());
    mName.setBoundingBox(mName.getBounds().toFloat());
    //addAndMakeVisible(&mName);
    
    
    mDialGl.setImage(mDialGlImage);
    mDialGl.setSize(mDialGlImage.getWidth(), mDialGlImage.getHeight());
    mDialGl.setCentrePosition(getBounds().getCentre().translated(-1,-4));
    mDialGl.setBoundingBox(mDialGl.getBounds().toFloat());
    addAndMakeVisible(&mDialGl);
    
    mBubble.setColour(BubbleMessageComponent::ColourIds::backgroundColourId, DTColourIds::PopUpBG);
    mBubble.setColour(BubbleMessageComponent::ColourIds::outlineColourId, DTColourIds::PopUpText);
    mFont = Font("Vilane", 30.00f, Font::bold);
    mSuffix = " dB";
    addChildComponent(&mBubble);
    
    
}

TopPanel::~TopPanel()
{
}

void TopPanel::paint (Graphics& g)
{
}


