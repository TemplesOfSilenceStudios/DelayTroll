/*
  ==============================================================================

    DTParamValueLabel.cpp
    Created: 23 Jul 2020 4:53:04pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DTParamValueLabel.h"
#include "DTUsedParameters.h"
#include "DTInterfaceDefines.h"

//==============================================================================
DTParamValueLabel::DTParamValueLabel(String _suffex, Value& _underlyingValue, double _modifyer) : mSuffex(_suffex), mUnderlyingValue(_underlyingValue), mModifier(_modifyer)
{
    mIsOverriden = false;
    mParamType = -1;
    mOverrideString = "";
    mSignificantDigits = 1.f;
}
DTParamValueLabel::DTParamValueLabel(Value& _valueToReferToo, bool _isoverriden, int _paramType) : mUnderlyingValue(_valueToReferToo), mIsOverriden(_isoverriden), mParamType(_paramType)
{
    mOverrideString = "";
    mSignificantDigits = 1.f;
}
DTParamValueLabel::~DTParamValueLabel()
{
}

void DTParamValueLabel::paint (juce::Graphics& g)
{
    //g.setColour (DTColourIds::SmlDisplayGlow);
   // g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
    //g.setColour (juce::Colours::white);

    if(mIsOverriden){
        //DBG("combopaint: "<< static_cast<float>(mUnderlyingValue.getValue()));
        //DBG(mOverrideString);
       
        g.setColour (DTColourIds::SmlDisplayGlow);
        g.drawFittedText(mOverrideString, getLocalBounds(), Justification::centred, 1);
        
    }
    else{
    
        double labelValue = static_cast<double>(mUnderlyingValue.getValue()) * mModifier;
        
        labelValue = trunc(labelValue * mSignificantDigits);
        
        labelValue /= mSignificantDigits;
        
        String LabelString(labelValue);
        
        LabelString += mSuffex;
              
       
        g.setColour (DTColourIds::SmlDisplayGlow);
       
        //g.drawFittedText(String(trunc( static_cast<double>(mUnderlyingValue.getValue()) * mModifier)) + mSuffex, getLocalBounds(), Justification::centred, 1);
        g.drawFittedText(LabelString, getLocalBounds(), Justification::centred, 1);
    }
}

void DTParamValueLabel::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
void DTParamValueLabel::setUnerlyingValue(Value &_newValue)
{
    mUnderlyingValue.referTo(_newValue);
}
