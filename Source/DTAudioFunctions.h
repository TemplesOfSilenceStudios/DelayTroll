/*
  ==============================================================================

    DTAudioFunctions.h
    Created: 19 Apr 2019 12:59:50pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#define kMeterSmoothingCoeff 0.2

const int maxBufferSize = 196000;
const int maxNumberOfTrolls = 64;
const float maxDelayTimeInSec = 10.f;


inline float DT_Hz_to_ms(float _Hz)
{
    return 1.f/_Hz * 1000.f;
}
inline float DT_ms_to_HZ(float _ms)
{
    return 1.f/_ms * 1000.f;
}

/** Finds the position t between two points */
inline float DT_linear_interp(float v0, float v1, float t){
    return ((1.f-t) * v0) + (t * v1);
}

/** Soft clipping function */
inline float tanh_clip(float x)
{
    return x * (27.0f + x * x)/(27.0f + 9.0f * x * x);
}

/**hard clipping function*/
inline float hard_clip(float x)
{
    return jlimit(-1.f,1.f,x);
}

/** Returns 0 if the inValue is less than 1e-15 */
inline float DT_denormalize(float inValue)
{
    float absValue = std::fabs(inValue);
    
    return absValue < 1e-15 ? 0.0f : inValue;
    
}

/** turns 0-1 cofficent into a 0-1 value that corrisponds to a -100db = infinity log 10 DBFS scale  */
static inline float dBTonormalizedGain(float inValue)
{
    float inValuedB = Decibels::gainToDecibels(inValue + 0.00001f, -48.f);
    inValuedB = (inValuedB + 48.0f) / 48.0f;

    return inValuedB;
}
static inline float coeffGaintoDB(float _inValue)
{
    float inValuedB = dBTonormalizedGain(_inValue);
    inValuedB = Decibels::gainToDecibels(inValuedB + 0.00001f, -48.f);
    //return jmap(inValuedb, -100.f, 0.f, -48.f, 0.f);
    return inValuedB;
}
