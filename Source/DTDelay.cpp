/*
  ==============================================================================

    DTDelay.cpp
    Created: 28 Apr 2019 3:29:52pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTDelay.h"

DTDelay::DTDelay()
: mSampleRate(-1),
mFeedbackSample{0.f, 0.f},
mDelayIndex(0)
{
    //mTimeSmoothed.calloc(1);
    reset();
}
DTDelay::~DTDelay()
{
    
}

void DTDelay::prep(double inSampleRate, int inNumOfChannels)
{
    mSampleRate = inSampleRate;
    mNumberofChannels = inNumOfChannels;
    mMaxBufferSize = static_cast<int>(inSampleRate) * 10;
    mNewBuffer.setSize(inNumOfChannels, mMaxBufferSize);
    mDelayIndex = 0;
    mTimeSmoothed = 0.f;
    mAmplidudeSmoothed = 0.f;
    mFeedbackSample[0] = 0.f;
    mFeedbackSample[1] = 0.f;
}

void DTDelay::reset()
{

    mNewBuffer.clear();
    mDelayIndex = 0;

    mTimeSmoothed = 0.f;
    mAmplidudeSmoothed = 0.f;
    
    mPanAtenuation = 1.f;
    mFeedbackSample[0] = 0.f;
    mFeedbackSample[1] = 0.f;
    
}

void DTDelay::process(dsp::AudioBlock<float>& inAudio,
                       float inTime,
                       float inFeedback,
                       float smoothFactor,
                       float inGain,
                       float _inLeftAttenuation,
                       float _inRightAttenuation,
                       bool _softClipperBypass,
                       AudioBuffer<float>& outAudio,
                       int inNumSamplesToRender)
{
    
    for(int channel = 0 ; channel < mNumberofChannels; channel++){
        channelData = mNewBuffer.getWritePointer(channel);
        outchannelData = outAudio.getWritePointer(channel);
        inaudioData = inAudio.getChannelPointer(channel);
        
        mPanAtenuation = channel == 0 ? _inLeftAttenuation : _inRightAttenuation;
        mCurrentDelayIndex = mDelayIndex;
        mCurrentTimeSmoothed = mTimeSmoothed;
        mCurrentAmplitudeSmoothed = mAmplidudeSmoothed;
        for(int i = 0; i < inNumSamplesToRender; i++)
        {
            
            mCurrentTimeSmoothed -= smoothFactor * (mCurrentTimeSmoothed - inTime);
            mCurrentAmplitudeSmoothed -= smoothFactor * (mCurrentAmplitudeSmoothed - inGain);
            
            delayTimeInSamples = mCurrentTimeSmoothed * mSampleRate;
        
            sample = getInterpolatedSample(delayTimeInSamples);
        
            channelData[mCurrentDelayIndex] = _softClipperBypass ?  jlimit(-1.f, 1.f, inaudioData[i] + mFeedbackSample[channel] * inFeedback)
                                                                 :  tanh_clip(inaudioData[i] + mFeedbackSample[channel] * inFeedback);
      
            sample *= mPanAtenuation;
            
            mFeedbackSample[channel] = sample;
            
            outchannelData[i] = _softClipperBypass ?    sample * mCurrentAmplitudeSmoothed + outchannelData[i] :
                                                        tanh_clip(sample * mCurrentAmplitudeSmoothed + outchannelData[i]);
            
            mCurrentDelayIndex = mCurrentDelayIndex + 1;
        
            if(mCurrentDelayIndex >= mMaxBufferSize){
                mCurrentDelayIndex = mCurrentDelayIndex - mMaxBufferSize;
            }
        
        }//end sample loop
    }//end channel loop
    mDelayIndex = mCurrentDelayIndex;
    mTimeSmoothed = mCurrentTimeSmoothed;
    mAmplidudeSmoothed = mCurrentAmplitudeSmoothed;
    
}

inline float DTDelay::getInterpolatedSample(float inDelayTimeInSamples)
{
    readPosition = static_cast<float>(mCurrentDelayIndex) - inDelayTimeInSamples;
    
    if(readPosition < 0.0f){
        readPosition = readPosition + mMaxBufferSize;
    }
    
    index_y0 = static_cast<int>(readPosition) - 1;
    
    if (index_y0 < 0){
        index_y0 += mMaxBufferSize;
    }
    
    index_y1 = static_cast<int>(readPosition);
    
    if(index_y1 >= mMaxBufferSize){
        index_y1 -= mMaxBufferSize;
    }
    
    //sample_y0 = channelData[index_y0];
    //sample_y1 = channelData[index_y1];
    
    
    //diff = readPosition - static_cast<int>(readPosition);
    
    return DT_linear_interp(channelData[index_y0], channelData[index_y1], readPosition - static_cast<int>(readPosition));
    
    
    
    
}
