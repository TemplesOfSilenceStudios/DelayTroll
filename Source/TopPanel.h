/*
  ==============================================================================

    TopPanel.h
    Created: 22 Apr 2019 4:44:50pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "DTButton.h"
#include "DTParamSlider.h"
#include "DTVuMeter.h"
#include "DTDialNeedleSlider.h"


//==============================================================================
/**Controls the presets and other menue items
*/
class TopPanel    : public PanelBase{
public:
    TopPanel(DelayTrollAudioProcessor& p);
    ~TopPanel();

    void paint (Graphics&) override;

private:
    
    DTDialNeedleSlider mInputGain;
    DTDialNeedleSlider mOutputGain;
    DTVuMeter mInputMeter;
    DTVuMeter mOutputMeter;
    
    const Image mUnderImage;
    const Image mBackgroundImage;
    const Image mLogoImage;
    DTButton mLogo;
    const Image mNameImage;
    DrawableImage mName;
    DrawableImage mBackground;
    DrawableImage mUnderlayer;
    DrawableRectangle mCentre;
    
    const Image mDialGlImage;
    DrawableImage mDialGl;
    
    DrawableText mInOutLabel;
    DrawableText m48Label;
    
    const Image mBypassImageOn, mBypassImageOff;
    
    DTButton mBypass;
    
    const Image mBypassLableImage;
    DrawableImage mBypassLable;
    
    Font mFont;
    AttributedString mAttString;
    String  mSuffix;
    BubbleMessageComponent mBubble;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TopPanel)
};
