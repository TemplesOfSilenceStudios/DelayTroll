/*
  ==============================================================================

    DTParamSlider.h
    Created: 19 Apr 2019 1:02:12pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

class DTParameterSlider
:   public Slider, public Timer
{
public:
    DTParameterSlider(AudioProcessorValueTreeState& stateToControl,
                      const String& parameterID,
                      const String& parameterLabel,
                      const Image& _image);
    DTParameterSlider(const Image& _image):mIsModulatable(false),mIsReversed(false),mIsGhostReversed(false),mGearImage(_image){setSize(mGearImage.getWidth(), mGearImage.getWidth());};
    DTParameterSlider(NormalisableRange<double> _range, float _value, const String& parameterLabel, const Image& _image);
    DTParameterSlider(Value& _valueToReferTo, NormalisableRange<double> _range, const String& parameterLabel, const Image& _image);
    ~DTParameterSlider();
    
    void paint (Graphics& _g) override;
    
    /**sets the slider as modulatable and the addresses for the mModParamPtr and the mProcessorPtr*/
    void setModulatableParameter(bool _isModulated, DTModulation::DTAssignableParam* _param, DelayTrollAudioProcessor* _p);
    
    void timerCallback() override;
    
    void setGhostTransform(float _x, float _y){mTranslate = AffineTransform::translation(_x, _y);};
    
    void setArcRadiusMult(float _newMult){ mArcRadiusMult = _newMult;}
    
    void setIsSliderReversed(bool _isReveresed){ mIsReversed = _isReveresed;}
    
    void setIsGhostReversed(bool _isGhostReversed) {mIsGhostReversed = _isGhostReversed;}
private:
    
    Label mLabel;
    
    bool mIsModulatable;
    bool mIsReversed;
    bool mIsGhostReversed;
    
    DTModulation::DTAssignableParam* mModParamPtr;
    DelayTrollAudioProcessor* mProcessorPtr;
    
    const Image& mGearImage;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mAttachment;
    
    AffineTransform mTranslate;
    float mArcRadiusMult = 1.f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DTParameterSlider);
    
};
