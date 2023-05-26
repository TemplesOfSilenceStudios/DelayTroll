/*
  ==============================================================================

    DTParamValueLabel.h
    Created: 23 Jul 2020 4:53:04pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class DTParamValueLabel  : public Label
{
public:
    DTParamValueLabel(String _suffex, Value& _underlyingValue, double _modifyer = 1.0);
    DTParamValueLabel(Value& _valuetoReferToo,  bool _isoverriden, int _paramType);
    ~DTParamValueLabel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setOverrideString(String _Override){mOverrideString = _Override;};
    
    void setOverriden(bool _isOverriden){mIsOverriden = _isOverriden;};
    
    void setSignificanDigits(int _numofsignificantdigits){ mSignificantDigits = _numofsignificantdigits < 0? 1.f: pow(10.f, static_cast<float>(_numofsignificantdigits));};
    
    void setUnerlyingValue(Value& _newValue);
    
private:
    
    String mSuffex;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> mAttachment;
    
    Value mUnderlyingValue;
    
    double mModifier;
    
    bool mIsOverriden;
    
    int mParamType;
    
    float mSignificantDigits;
    
    String mOverrideString;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTParamValueLabel)
};
