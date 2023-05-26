/*
  ==============================================================================

    ModulationDisplayPanelBase.cpp
    Created: 24 Apr 2019 11:23:45am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ModulationDisplayPanelBase.h"
#include "DTInterfaceDefines.h"

//==============================================================================
ModulationDisplayPanelBase::ModulationDisplayPanelBase(DelayTrollAudioProcessor& p, int _index):
PanelBase(p),
mDisplayBGImage(ImageCache::getFromMemory(BinaryData::LargeDisplay2_png, BinaryData::LargeDisplay2_pngSize)),
mDisplayGlImage(ImageCache::getFromMemory(BinaryData::LargeDisplay2Gl_png, BinaryData::LargeDisplay2Gl_pngSize)),
mIndex(_index)
{


    //mRealCurve = std::make_unique<Slider>();
    mPolar.setSliderStyle(juce::Slider::LinearVertical);
    mPolar.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    mPolar.setBounds(0, DISPLAY_PANEL_VERTICAL_BUMPER-5, DISPLAY_PANEL_HORIZONTAL_BUMPER +24, MOD_INNER_DISPLAY_PANEL_HEIGHT+14);
    //mPolar.setTopLeftPosition(0, DISPLAY_PANEL_VERTICAL_BUMPER-9);
    
    mDisplayBG.setImage(mDisplayBGImage);
    mDisplayBG.setSize(mDisplayBGImage.getWidth(), mDisplayBGImage.getHeight());
    mDisplayBG.setTopLeftPosition(mPolar.getRight() - 10, DISPLAY_PANEL_VERTICAL_BUMPER);
    mDisplayBG.setBoundingBox(mDisplayBG.getBounds().toFloat());
    addAndMakeVisible(&mDisplayBG);
    
    mDisplayGl.setImage(mDisplayGlImage);
    mDisplayGl.setSize(mDisplayGlImage.getWidth(), mDisplayGlImage.getHeight());
    mDisplayGl.setTopLeftPosition(mPolar.getRight() - 8, DISPLAY_PANEL_VERTICAL_BUMPER+2);
    mDisplayGl.setBoundingBox(mDisplayGl.getBounds().toFloat());
    mDisplayGl.setAlwaysOnTop(true);
    addAndMakeVisible(&mDisplayGl);
    
    mPolar.setRange(0.f, 1.f);
    mPolar.setValue(p.getModulationArray()[_index]->getPolarValue());
    mPolar.setAlwaysOnTop(false);
    
    //mPolar.setPaintingIsUnclipped(true);
    //mPolar.setAlwaysOnTop(true);
    //mPolar.setBroughtToFrontOnMouseClick(true);
    
    //mBubble.setSize(50, 50);

    //mBubble.setPaintingIsUnclipped(true);
    mBubble.setColour(BubbleMessageComponent::ColourIds::backgroundColourId, DTColourIds::PopUpBG);
    mBubble.setColour(BubbleMessageComponent::ColourIds::outlineColourId, DTColourIds::PopUpText);
    
    mHeight = mPolar.getPositionOfValue(mPolar.getValue()) + mPolar.getY();
    mDasher = Line<float>(DISPLAY_PANEL_HORIZONTAL_BUMPER, mHeight, getLocalBounds().toFloat().getRight() - 6.f, mHeight);
    
    //startTimer(150);
    
    mPolar.onDragStart = [this](){
        startTimer(40);
    };
    mPolar.onDragEnd = [this](){
        stopTimer();
        timerCallback();
    };
    mPolar.onValueChange = [this](){
        mProcessor.getModulationArray()[mIndex]->setPolarValue(static_cast<float>(mPolar.getValue()));
        mAttString.clear();
        mAttString.append(mPolarity, mFont, DTColourIds::PopUpText);
        mAttString.append(String(roundToInt(mPolar.getValue() * 100.0)),  DTColourIds::PopUpText);
        mBubble.showAt(&mPolar, mAttString, 150);
    };

    mFont = Font("Vilane", 24.f, Font::bold);
    
    mPolarity = "Polarity: ";

}

ModulationDisplayPanelBase::~ModulationDisplayPanelBase()
{
    stopTimer();
}

void ModulationDisplayPanelBase::paint (Graphics& g)
{
   
    //g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
    //g.drawDashedLine(mDasher, mDashLeng, 2, 2.f, 1);
    //g.drawLine(mDasher, 2.f);
    //paintOverChildren(g);

}
void ModulationDisplayPanelBase::timerCallback()
{
    if(!isVisible())
        return;
    
    //if(mPolar.isMouseOverOrDragging()){


    //}

    //mHeight = mPolar.getPositionOfValue(mPolar.getValue())+  mPolar.getY();
    //mDasher = Line<float>(DISPLAY_PANEL_HORIZONTAL_BUMPER, mHeight, getLocalBounds().toFloat().getRight()- 6.f, mHeight);
    
    repaint(getLocalBounds());
    //updateMouseCursor();
}

