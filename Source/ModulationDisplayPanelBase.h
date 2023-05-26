/*
  ==============================================================================

    ModulationDisplayPanelBase.h
    Created: 24 Apr 2019 11:23:45am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PanelBase.h"

//==============================================================================
/**base class for modulation component background displays
*/
class ModulationDisplayPanelBase    : public PanelBase, public Timer
{
public:
    ModulationDisplayPanelBase(DelayTrollAudioProcessor& p, int _index);
    ~ModulationDisplayPanelBase();

    void paint (Graphics&) override;
    
    void timerCallback() override;

protected:
    Slider mPolar;
    BubbleMessageComponent mBubble;
    
private:
    //*******Solve clipping issues with this and get rid of child classes
    //std::unique_ptr<Component> mRealCurve;
    const Image mDisplayBGImage;
    const Image mDisplayGlImage;
    DrawableImage mDisplayBG;
    DrawableImage mDisplayGl;
    
    int mIndex;
    const float mDashLeng[2]{10.f, 5.f};
    Line<float> mDasher;
    bool mRepaintNeeded;
    float mHeight;
    Font mFont;
    AttributedString mAttString;
    String  mPolarity;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulationDisplayPanelBase)
};
