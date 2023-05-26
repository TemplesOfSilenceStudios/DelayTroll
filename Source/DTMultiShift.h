/*
  ==============================================================================

    MultiShiftOnStackDSP.h
    Created: 28 Jan 2020 6:25:13pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
//#include "SIMDFastMath.h"
//#include "DelayTroll.h"
#include "AtomicTroll.h"
#include "DTAtomicHord.h"
#define USE_SSE2
#define STACKFFTSIZE 4096
#define STACKNUMBEROFSHIFTS 100



namespace DTDSP {


class DTMultiShift {
public:
    DTMultiShift(OwnedArray<AtomicTroll>& processorsDelayTrolls, DTAtomicHord& _AtomicHord);
    
    /**prepare the input and output blocks
     */
    void prep(int inputchannel, int inputblockSize, AudioBuffer<float>& inputprocessorbuffer);
    
    /** called when the blocksize is changed during live playback*/
    //void blockSizeChanged(int _newBlockSize);
    
    /**procsses the audio
     */
    void process(AudioBuffer<float>& buffer);
    
    /**sets the size of the FFT object
     */
    void updateFftSize();
    
    /**how many overlaps will be used
     */
    void updateHopSize();
    
    /**updates windows to instructed lengths
     */
    void updateWindow (float window[], const int windowLength);
    
    /**updates the scale factor based on how many overlaps are intersecting
     */
    void updateWindowScaleFactor();
    
private:
    
    /**wrapps the input to a phase adjust of -pi to pi
     */
    float phaseWrapper(const float phase);
    
    
    /**wraps a SIMDRegisteredInput to a phase adjust of -pi to pi
     */
    dsp::SIMDRegister<float> phaseWrapperSIMD(dsp::SIMDRegister<float> phase);
    
    
    //SIMDFastMath fastMathSIMD;
    
    
    //phase Wrapper Stuff
    //======================================
    dsp::SIMDRegister<float> mZerovec;
    dsp::SIMDRegister<float> mVecNA, mVecNB;
    dsp::SIMDRegister<float> mVecFmodA, mVecFmodB;
    
    dsp::SIMDRegister<float>::vMaskType mPhaseWrapperMask;
    
    dsp::SIMDRegister<float> mPiVec, mTwoPiVec, mNegTwoPieVec;
    
    //parameters
    //======================================
    
    int mNumOfShifts;
    float shift;
    float ratio;
    float ratioArr[STACKNUMBEROFSHIFTS];
    dsp::SIMDRegister<float> mRatioSIMD[STACKNUMBEROFSHIFTS];
    
    AudioBuffer<float>* processorOutput;
    AudioBuffer<float> dummybuffer;
    
    bool onetime;
    
    int mChannelNumber;
    OwnedArray<AtomicTroll>& mDelayTrolls;
    DTAtomicHord& mAtomicHordRef;
    
    //dsp in out stuff
    //======================================
    int mInputBufferLength;
    int mInputBufferWritePosition;
    int mCurrentInputBufferWritePosition;
    int mInputBufferIndex;
    float mInputBuffer[STACKFFTSIZE];
    
    int mOutputBufferLength;
    int mOutputBufferWritePosition;
    int mCurrentOutputBufferWritePosition;
    int mOutputBufferIndex;
    int mOutputBufferReadPosition;
    int mCurrentOutputBufferReadPosition;
    float mOutputBufferArr[STACKNUMBEROFSHIFTS][STACKFFTSIZE*2];
    
    float mInputSample;
    float* mChannelData;
    
    //fftStuff
    //======================================
    
    juce::dsp::FFT mFft;
    
    float mFftWindow[STACKFFTSIZE];

    float mFftTimeFreqData[STACKFFTSIZE*2];
    float mFftTimeFreqDataPhaseAdjusted[STACKNUMBEROFSHIFTS][STACKFFTSIZE*2];

    float* mFftTimeFreaqDataDeinterleaveWrite[8];
    const float* mFftTimeFreaqDataDeinterleaveRead[8];//may not need
    const float* mFftRealDataDeinterleaveRead[4];
    const float* mFftImagDataDeinterleaveRead[4];
    float* mFftRealDataDeinterleaveWrite[4];//may not need
    float* mFftImagDataDeinterleaveWrite[4];//may not need
    float mFftRealData[4][STACKFFTSIZE/4];
    float mFftImagData[4][STACKFFTSIZE/4];
    juce::dsp::SIMDRegister<float> mFftRealSIMD[STACKFFTSIZE/4];
    juce::dsp::SIMDRegister<float> mFftImagSIMD[STACKFFTSIZE/4];
    
    const float* mFftTimeFreaqDataDeinterleaveReadArr[STACKNUMBEROFSHIFTS][8];
    juce::dsp::SIMDRegister<float> mFftRealPhaseAdjustedSIMD[STACKFFTSIZE/4];
    juce::dsp::SIMDRegister<float> mFftImagPhaseAdjustedSIMD[STACKFFTSIZE/4];
    
    int mFftSize;
    float mFftSizeFloat;
    
    //overlap stuff
    //======================================
    float mOmega[STACKFFTSIZE];
    float mOmegaXhopsize[STACKFFTSIZE];
    dsp::SIMDRegister<float>  mOmegaXhopsizeSIMD[STACKFFTSIZE/4];

    int mOverlap;
    int mHopSize;
    int mSamplesSinceLastFFT;
    int mCurrentSamplesSinceLastFFT;
    
    //phase adjust stuff
    //======================================
    bool mNeedToResetPhases;
 
    dsp::SIMDRegister<float> mInputPhaseSIMD[STACKFFTSIZE/4];
    dsp::SIMDRegister<float> mOutputPhaseArrSIMD[STACKNUMBEROFSHIFTS][STACKFFTSIZE/4];
    dsp::SIMDRegister<float> mMagnitudeSIMD[STACKFFTSIZE/4];
    dsp::SIMDRegister<float> mPhaseSIMD;
    dsp::SIMDRegister<float> mPhaseDeviationSIMD;
    dsp::SIMDRegister<float> mDeltaPhiSIMD[STACKFFTSIZE/4];
    dsp::SIMDRegister<float> mNewPhaseSIMD;
    
    //resampler stuff
    //======================================
    float mSynthesisWindow[STACKFFTSIZE*2];
    float mSqrtSynthesisWindowArr[STACKNUMBEROFSHIFTS][STACKFFTSIZE*2];
    
    int mResamplerLengthArr[STACKNUMBEROFSHIFTS];
    float mResamplerLenghthXfftSize;
    float mResamplerX[STACKFFTSIZE*2];

    int mResamplerIXArr[STACKNUMBEROFSHIFTS][STACKFFTSIZE*2];
    int mResamplerIIXArr[STACKNUMBEROFSHIFTS][STACKFFTSIZE*2];
    float mResamplerDXArr[STACKNUMBEROFSHIFTS][STACKFFTSIZE*2];//may not need
    
    float mWindowScaleFactor;
    float mJIndexFloat;
    float mResamplerSample;
    
};
}
