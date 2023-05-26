/*
  ==============================================================================

    DTDialNeedleSlider.h
    Created: 20 Oct 2020 10:57:26am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DTAssignableParam.h"
//==============================================================================
/*
*/
class DTDialNeedleSlider  : public juce::Slider, public Timer
{
public:
    DTDialNeedleSlider(AudioProcessorValueTreeState& stateToControl,
                       const String& parameterID,
                       const String& parameterLabel,
                       DTModulation::DTAssignableParam& _param);
    ~DTDialNeedleSlider() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void valueChanged() override;
    
    void timerCallback() override;

private:
    
    const Image mNeedleImage;
    std::unique_ptr<juce::DrawableImage> mNeedle;
    juce::AffineTransform mRotate;
    
    juce::AffineTransform mDefalt;
    
    Rectangle<int> mImageBounds;
    
    DTModulation::DTAssignableParam& mModParamRef;
    
    Path mModRangePath;
    Point<float> mCentrePoint;
    Rectangle<float> mCentreRect;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTDialNeedleSlider)
};
