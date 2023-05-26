/*
  ==============================================================================

    DTAssignableParam.cpp
    Created: 17 Aug 2020 12:37:24pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTAssignableParam.h"
#include "DTModulationDefines.h"
#include "DTUsedParameters.h"
using namespace DTModulation;

DTAssignableParam::DTAssignableParam(int _Type, AudioParameterFloat& _value):
sharedOrigninalValue(_value)
{
    //const ScopedLock myScopedLock (objectLock);
    
   // juce::AudioProcessorParameter::setValue(static_cast<float>(_value.getValue()));
    currentValue = sharedOrigninalValue.get();
    currentTarget = sharedOrigninalValue.get();
    setValue(currentTarget);
    ofset = 0.f;
    //sharedModulatedValue.setValue(sharedOrigninalValue.getValue());
    paramID = DTAssignableParamsID[_Type];
    paramName = DTAssignableParamsName[_Type];
    //originalValue = DTAssignOriginalValues[_Type];
    modulatedValue = DTAssignOriginalValues[_Type];
    minDepthValue = DTAssignOriginalValues[_Type];
    maxDepthValue = DTAssignOriginalValues[_Type];
    range = DTAssignParamRanges[_Type];
    isAttached = false;
    typeIndex = _Type;
    modulationManagers.clear();
    modulatorDepths.clear();
    //quantizedValue = var(0.0);
    //isQuantized = false;
    stepsToTarget = 1;
    countdown = 0;
}

void DTAssignableParam::setValue(float _newValue)
{
    target.store(_newValue, std::memory_order_release);
}

void DTAssignableParam::updateTarget()
{
    float newTarget = target.load(std::memory_order_acquire);
    if(newTarget != currentTarget)
    {
        currentTarget = newTarget;
        countdown = stepsToTarget;
        
        if(countdown <= 0)
            currentValue = currentTarget;
        else
            step = (currentTarget - currentValue)/static_cast<float>(countdown);
    }
}

int DTAssignableParam::getParamIndex()
{
    int paramIndex = -1;
    switch (typeIndex) {
        case kAss_Feedback:
            paramIndex = kParam_Feedback;
            break;
        case KAss_InputLevel:
            paramIndex = kParam_InputLevel;
            break;
        case kAss_OutputLevel:
            paramIndex = kParam_OutputLevel;
            break;
        case kAss_WetDry:
            paramIndex = kParam_WetDry;
            break;
        case kAss_SmoothFactor:
            paramIndex = kParam_SmoothFactor;
            break;
        case kAss_MinTime:
            paramIndex = kParam_MinDelayTime;
            break;
        case kAss_MaxTime:
            paramIndex = kParam_MaxDelayTime;
            break;
        case kAss_Gravity:
            paramIndex = kParam_GravetyExpo;
            break;
            
        default:
            break;
    }
    jassert(paramIndex != -1);

    return paramIndex;
    
}

float DTAssignableParam::getNextValue()
{
    updateTarget();
    if(countdown <=0)
        return currentTarget;
    
    --countdown;
    currentValue += step;
    return currentValue;
    
}
