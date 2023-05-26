/*
  ==============================================================================

    DTDelay.h
    Created: 28 Apr 2019 3:29:52pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "DTAudioFunctions.h"
#include "../JuceLibraryCode/JuceHeader.h"
//#include "DelayTroll.h"


class DTDelay
{
public:
    DTDelay();
    ~DTDelay();
    
    /** sets the sample rate */
    void prep(double inSampleRate, int inNumOfChannels);
    
    /** zeros out the buffer and mTimeSmoothed */
    void reset();
    
    
    /** delays a block of audio */
    void process(dsp::AudioBlock<float>& inAudio,
                 float inTime,
                 float inFeedback,
                 float smoothFactor,
                 float inGain,
                 float _inLeftAttenuation,
                 float _inRightAttenuation,
                 bool _softClipperBypass,
                 AudioBuffer<float>& outAudio,
                 int inNumSamplesToRender);
    
    float mCurrentTimeSmoothed;
    float mCurrentAmplitudeSmoothed;
    
private:
    
    /** INTERNAL */
    inline float getInterpolatedSample(float inDelayTimeInSamples);
    //interpolate samples variables
    int index_y0, index_y1;
    float readPosition;
    float sample_y0, sample_y1;
    float diff;
    
    
    //process variables
    float delayTimeInSamples;
    float sample;
    float* channelData;
    float* outchannelData;
    float* inaudioData;
    
    float mSampleRate;
    float mFeedbackSample[2];
    
    float mPanAtenuation;
    

    float mTimeSmoothed;
    float mAmplidudeSmoothed;
    
    int mDelayIndex;
    int mCurrentDelayIndex;
    int mMaxBufferSize;
    int mNumberofChannels;
    AudioBuffer<float> mNewBuffer;
    
};
