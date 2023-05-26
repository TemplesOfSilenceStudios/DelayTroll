/*
  ==============================================================================

    RightPanel.h
    Created: 23 Apr 2019 9:12:03am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "DTParamSlider.h"
#include "DTVuMeter.h"

//==============================================================================
/**hosts mix and compression related parameters
*/
class RightPanel    : public PanelBase
{
public:
    RightPanel(DelayTrollAudioProcessor& p);
    ~RightPanel();
    
    void paint (Graphics&) override;
    void resized() override;

private:
    
    Font mFont;
    AttributedString mAttString;
    String  mSuffix;
    BubbleMessageComponent mBubble;

    const Image mWetDryImage;
    const Image mSmoothFactorImage;
    DTParameterSlider mWetDry;
    DTParameterSlider mSmoothFactor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RightPanel)
};
