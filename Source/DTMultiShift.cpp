/*
  ==============================================================================

    MultiShiftOnStackDSP.cpp
    Created: 28 Jan 2020 6:25:13pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTMultiShift.h"
#include "DTSIMDHelper.h"



using namespace DTDSP;

DTMultiShift::DTMultiShift(OwnedArray<AtomicTroll>& processorsDelayTrolls, DTAtomicHord& _AtomicHord): mDelayTrolls(processorsDelayTrolls), mAtomicHordRef(_AtomicHord), mFft(log2(STACKFFTSIZE))
{
    
    
    mNumOfShifts = STACKNUMBEROFSHIFTS;
  
    //point some stuff at the real/imagin data dump for interweaving
    for(int i = 0 ; i < 4; i++){
        mFftTimeFreaqDataDeinterleaveWrite[i*2] = mFftRealData[i];
        mFftTimeFreaqDataDeinterleaveWrite[(i*2)+1] = mFftImagData[i];
        mFftTimeFreaqDataDeinterleaveRead[i*2] = mFftRealData[i];
        mFftTimeFreaqDataDeinterleaveRead[(i*2)+1] = mFftImagData[i];
        mFftRealDataDeinterleaveRead[i] = mFftRealData[i];
        mFftImagDataDeinterleaveRead[i] = mFftImagData[i];
        mFftRealDataDeinterleaveWrite[i] = mFftRealData[i];
        mFftImagDataDeinterleaveWrite[i] = mFftImagData[i];
    }
    
    
    //set up some SIMD
    mZerovec = 0.0f;
    mPiVec = MathConstants<float>::pi;
    mTwoPiVec = MathConstants<float>::twoPi;
    mNegTwoPieVec = -MathConstants<float>::twoPi;
    
    //temp shift value
    shift = 0.5f;
    
    mNeedToResetPhases = true;
    onetime = true;
    
    dummybuffer.setSize(1, 512);
    
    //prep the fft
    prep(0, 512, dummybuffer);
    updateFftSize();
    updateHopSize();
    updateWindowScaleFactor();
    
}

void DTMultiShift::prep(int inputchannel, int inputblockSize, AudioBuffer<float>& inputprocessorbuffer)
{
    //shift stuff initalize channels hear
    mNeedToResetPhases = true;
    mChannelNumber = inputchannel;
    processorOutput = &inputprocessorbuffer;
    jassert(processorOutput->getNumSamples()==inputblockSize);
    
    mNumOfShifts = mAtomicHordRef.getCurrentHordSize();
    
}

void DTMultiShift::process(AudioBuffer<float>& buffer)
{
    
//calculate the resample lenght and some indexes if the shift value changes
//    if(shift != inputshift)
//    {
        //shift = inputshift;
        //ratio = roundf (inputshift * static_cast<float>(mHopSize)) / static_cast<float>(mHopSize);
    
    jassert(buffer.getNumSamples() == processorOutput->getNumSamples());
    
    mNumOfShifts = mAtomicHordRef.getCurrentHordSize();
    
        for(int shiftNum = 0 ; shiftNum < mNumOfShifts; shiftNum++){
            
            ratioArr[shiftNum] = roundf(mDelayTrolls[shiftNum]->mAttributes[TrollParams::kTroll_PitchFactor] * static_cast<float>(mHopSize))/static_cast<float>(mHopSize);
            mRatioSIMD[shiftNum] = ratioArr[shiftNum];
            mResamplerLengthArr[shiftNum] = floorf(mFftSizeFloat/ratioArr[shiftNum]);
            updateWindow(mSynthesisWindow, mResamplerLengthArr[shiftNum]);
            mResamplerLenghthXfftSize = mFftSizeFloat/static_cast<float>(mResamplerLengthArr[shiftNum]);
            mJIndexFloat = 0.0f;
            for(int j = 0; j < mResamplerLengthArr[shiftNum]; ++j)
            {
                mResamplerX[j] = mJIndexFloat++ * mResamplerLenghthXfftSize;
                mResamplerIXArr[shiftNum][j] = static_cast<int>(floorf(mResamplerX[j]));
                mResamplerIIXArr[shiftNum][j] = (mResamplerIXArr[shiftNum][j] + 1) % mFftSize;
                mResamplerDXArr[shiftNum][j] = mResamplerX[j] - static_cast<float>(mResamplerIXArr[shiftNum][j]);
                mSqrtSynthesisWindowArr[shiftNum][j] = std::sqrtf(mSynthesisWindow[j]) * mWindowScaleFactor;
            }

        }

        //onetime = false;
//    }
    
    //keep track of write/read positions and hopposition
    mCurrentInputBufferWritePosition = mInputBufferWritePosition;
    mCurrentOutputBufferWritePosition = mOutputBufferWritePosition;
    mCurrentOutputBufferReadPosition = mOutputBufferReadPosition;
    mCurrentSamplesSinceLastFFT = mSamplesSinceLastFFT;
    mChannelData = buffer.getWritePointer(mChannelNumber);
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        
        mInputSample =mChannelData [sample];
        
        for (int shift = 0; shift < mNumOfShifts; shift++){
            processorOutput->setSample(shift * 2 + mChannelNumber, sample, mOutputBufferArr[shift][mCurrentOutputBufferReadPosition]);
            mOutputBufferArr[shift][mCurrentOutputBufferReadPosition] = 0.0f;
        }
        
        
        
        if (++mCurrentOutputBufferReadPosition >= mOutputBufferLength)
            mCurrentOutputBufferReadPosition = 0;
        
        mInputBuffer[mCurrentInputBufferWritePosition] = mInputSample;
        if (++mCurrentInputBufferWritePosition >= mInputBufferLength)
            mCurrentInputBufferWritePosition = 0;
        //======================================
        
        //hopsize block
        if (++mCurrentSamplesSinceLastFFT >= mHopSize)
        {
            
            mCurrentSamplesSinceLastFFT = 0;
            
            //======================================
            //put real data into fftTimeDomainArray
            mInputBufferIndex = mCurrentInputBufferWritePosition;
            for (int index = 0; index < mFftSize; ++index) {
                
                mFftTimeFreqData[index] = sqrtf(mFftWindow[index]) * mInputBuffer[mInputBufferIndex];
                
                if (++mInputBufferIndex >= mInputBufferLength)
                    mInputBufferIndex = 0;
            }
            
            //======================================
            
            //wind up into the frequency domain and deinterleave into SIMDs
            mFft.performRealOnlyForwardTransform(mFftTimeFreqData, true);
            
            AudioDataConverters::deinterleaveSamples(mFftTimeFreqData, mFftTimeFreaqDataDeinterleaveWrite, mFftSize/4, 8);
            AudioDataConverters::interleaveSamples(mFftRealDataDeinterleaveRead, reinterpret_cast<float*>(mFftImagSIMD), mFftSize/4, 4);
            AudioDataConverters::interleaveSamples(mFftImagDataDeinterleaveRead, reinterpret_cast<float*>(mFftRealSIMD), mFftSize/4, 4);
            //======================================
            
            //clear the input phase
            if(mNeedToResetPhases)
            {
                for(int i = 0 ; i < mFftSize/4; i++){
                    mInputPhaseSIMD[i] = 0.0f;
                    
                    for (int shiftNum = 0; shiftNum < mNumOfShifts; shiftNum++) {
                        mOutputPhaseArrSIMD[shiftNum][i] = 0.0f;
                    }
                }
                mNeedToResetPhases = false;
            }
            
            //======================================
            //set the magnitude and deltaphi
            for (int index = 0; index < mFftSize/4; ++index) {
//#if JUCE_MAC
//                mMagnitudeSIMD[index] = DT_SIMD_sqrt_ps((mFftRealSIMD[index].value * mFftRealSIMD[index].value) + (mFftImagSIMD[index].value * mFftImagSIMD[index].value));
//#elif JUCE_WINDOWS
                mMagnitudeSIMD[index] = DT_SIMD_sqrt_ps((mFftRealSIMD[index] * mFftRealSIMD[index]) + (mFftImagSIMD[index] * mFftImagSIMD[index]));
//#endif
                mFftRealSIMD[index] = mFftRealSIMD[index] + 0.0000000001f;
                mPhaseSIMD = DT_SIMD_atan2_ps(mFftImagSIMD[index], mFftRealSIMD[index]);
                
                
                mPhaseDeviationSIMD = mPhaseSIMD - mInputPhaseSIMD[index] - mOmegaXhopsizeSIMD[index];
                mDeltaPhiSIMD[index] = mOmegaXhopsizeSIMD[index] + phaseWrapperSIMD(mPhaseDeviationSIMD);
                mInputPhaseSIMD[index] = mPhaseSIMD;
            }
            
            //adjust the phase
            for (int shiftNum = 0; shiftNum < mNumOfShifts; shiftNum++) {
            

                for (int index = 0; index < mFftSize/4; ++index) {
                    

                    mNewPhaseSIMD = phaseWrapperSIMD(dsp::SIMDRegister<float>::multiplyAdd(mOutputPhaseArrSIMD[shiftNum][index], mDeltaPhiSIMD[index], mRatioSIMD[shiftNum]));
                    
                    mOutputPhaseArrSIMD[shiftNum][index] = mNewPhaseSIMD;
                    
                    
                    mFftRealPhaseAdjustedSIMD[index] = mMagnitudeSIMD[index] * DT_SIMD_cos_ps(mNewPhaseSIMD);
                    mFftImagPhaseAdjustedSIMD[index] = mMagnitudeSIMD[index] * DT_SIMD_sin_ps(mNewPhaseSIMD);

                }//end index loop
                
                AudioDataConverters::deinterleaveSamples(reinterpret_cast<float*>(mFftRealPhaseAdjustedSIMD), mFftRealDataDeinterleaveWrite, mFftSize/4, 4);
                AudioDataConverters::deinterleaveSamples(reinterpret_cast<float*>(mFftImagPhaseAdjustedSIMD), mFftImagDataDeinterleaveWrite, mFftSize/4, 4);
                AudioDataConverters::interleaveSamples(mFftTimeFreaqDataDeinterleaveRead, mFftTimeFreqDataPhaseAdjusted[shiftNum], mFftSize/4, 8);
                mFft.performRealOnlyInverseTransform(mFftTimeFreqDataPhaseAdjusted[shiftNum]);
            }//end shift loop
            
            //======================================
            
            //resample the output to match the new ratio
            for(int shiftNum = 0 ; shiftNum < mNumOfShifts; shiftNum++){
            mOutputBufferIndex = mCurrentOutputBufferWritePosition;
            for (int index = 0; index < mResamplerLengthArr[shiftNum]; ++index)
                {

                    mResamplerSample = mFftTimeFreqDataPhaseAdjusted[shiftNum][mResamplerIXArr[shiftNum][index]];

                    mOutputBufferArr[shiftNum][mOutputBufferIndex] = mOutputBufferArr[shiftNum][mOutputBufferIndex] + (mResamplerSample + mResamplerDXArr[shiftNum][index] * (mFftTimeFreqDataPhaseAdjusted[shiftNum][mResamplerIIXArr[shiftNum][index]] - mResamplerSample)) * mSqrtSynthesisWindowArr[shiftNum][index];
                    
                    
                    if (++mOutputBufferIndex >= mOutputBufferLength)
                        mOutputBufferIndex = 0;
                }
            }
            
            mCurrentOutputBufferWritePosition += mHopSize;
            if (mCurrentOutputBufferWritePosition >= mOutputBufferLength)
                mCurrentOutputBufferWritePosition = 0;
            
        }//end hoploop

    }//end sample loop
    
    mInputBufferWritePosition = mCurrentInputBufferWritePosition;
    mOutputBufferWritePosition = mCurrentOutputBufferWritePosition;
    mOutputBufferReadPosition = mCurrentOutputBufferReadPosition;
    mSamplesSinceLastFFT = mCurrentSamplesSinceLastFFT;
    
    
}

void DTMultiShift::updateFftSize()
{
    
    //set fftSize and reset sample count
    mFftSize = STACKFFTSIZE;
    mFftSizeFloat = static_cast<float>(mFftSize);
    
    mSamplesSinceLastFFT = 0;
    
    //clear some of our c style arrays
    for(int i = 0 ; i < mFftSize; i++){
        mInputBuffer[i] = 0.0f;
        mOmega[i] = 0.0f;
        mOmegaXhopsize[i] = 0.0f;
        
        mFftTimeFreqData[i] = 0.0f;
        mFftTimeFreqData[(i*2)+1] = 0.0f;
        
        mSynthesisWindow[i] = 0.0f;
        mSynthesisWindow[(i*2)+1] = 0.0f;
        
    }

    mInputBufferLength = mFftSize;
    mInputBufferWritePosition = 0;
    
    float maxRatio = powf (2.0f, -12.0f / 12.0f);
    mOutputBufferLength = static_cast<int>(floorf (mFftSizeFloat / maxRatio));
    mOutputBufferWritePosition = 0;
    mOutputBufferReadPosition = 0;
    
    
    //precalculate omega
    for (int index = 0; index < mFftSize; ++index){
        mOmega[index] = MathConstants<float>::twoPi * static_cast<float>(index) / mFftSizeFloat;
        mOmegaXhopsize[index] = mOmega[index] * static_cast<float>(mHopSize);
    }

    

     for(int i = 0 ; i < mFftSize/4; i++){
         mInputPhaseSIMD[i] = 0.0f;
     }
    
}

void DTMultiShift::updateHopSize()
{
    mOverlap = 4;
    if (mOverlap != 0) {
        mHopSize = mFftSize / mOverlap;
        mOutputBufferWritePosition = mHopSize % mOutputBufferLength;
    }
    for (int index = 0; index < mFftSize; ++index){
        mOmegaXhopsize[index] = mOmega[index] * static_cast<float>(mHopSize);
    }
    
//********* LOGIC ERROR **************
    //float omegaHopArray[4][mFftSize/4];
    float omegaHopArray[4][STACKFFTSIZE/4];
    const float* omegaHopReadPtr[4];
    float* omegaHopWritePtr[4];
    for(int i = 0 ; i < 4; i++){
        omegaHopReadPtr[i] = omegaHopArray[i];
        omegaHopWritePtr[i] = omegaHopArray[i];
    }
    
    AudioDataConverters::deinterleaveSamples(mOmegaXhopsize, omegaHopWritePtr, mFftSize/4, 4);
    AudioDataConverters::interleaveSamples(omegaHopReadPtr, reinterpret_cast<float*>(mOmegaXhopsizeSIMD), mFftSize/4, 4);
 
    
    
    updateWindow(mFftWindow, mFftSize);
}

void DTMultiShift::updateWindow(float window[], const int windowLength)
{
    for (int sample = 0; sample < windowLength; ++sample)
    {
        window[sample] = 0.5f - 0.5f * cosf(2.0f * MathConstants<float>::pi * static_cast<float>(sample) / static_cast<float>(windowLength - 1));
    }
}
void DTMultiShift::updateWindowScaleFactor()
{
    float windowSum = 0.0f;
    for (int sample = 0; sample < mFftSize; ++sample)
        windowSum += mFftWindow[sample];
    
    mWindowScaleFactor = 0.0f;
    if (mOverlap != 0 && windowSum != 0.0f)
        mWindowScaleFactor = 1.0f / static_cast<float>(mOverlap) / windowSum * mFftSizeFloat;
}

float DTMultiShift::phaseWrapper(const float phase)
{
    if (phase >= 0.0f)
        return fmod (phase + MathConstants<float>::pi,  MathConstants<float>::twoPi) - MathConstants<float>::pi;
    else
        return fmod (phase + MathConstants<float>::pi,  -MathConstants<float>::pi) + MathConstants<float>::pi;
}

dsp::SIMDRegister<float> DTMultiShift::phaseWrapperSIMD(dsp::SIMDRegister<float> phase){
    
    //create a mask for if phase > 0
    mPhaseWrapperMask = dsp::SIMDRegister<float>::greaterThanOrEqual(phase, mZerovec);
    

    mVecNA = dsp::SIMDRegister<float>::truncate((phase + mPiVec)/ mTwoPiVec);
    mVecFmodA = ((phase + mPiVec) - (mVecNA * mTwoPiVec)) - mPiVec;
    
    mVecNB = dsp::SIMDRegister<float>::truncate((phase + mPiVec)/ mNegTwoPieVec);
    mVecFmodB = ((phase + mPiVec) - (mVecNB * mNegTwoPieVec)) + mPiVec;

    //implement mask
    return ((mVecFmodA & mPhaseWrapperMask) + (mVecFmodB & (~mPhaseWrapperMask)));
}
