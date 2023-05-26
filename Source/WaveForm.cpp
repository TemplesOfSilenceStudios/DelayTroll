/*
  ==============================================================================

    WaveForm.cpp
    Created: 26 Apr 2019 8:52:39am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "WaveForm.h"
#include "DTModulationDefines.h"

WaveForm::WaveForm(float _period, float _amplitude, float _phase, int _waveType, float _arraySize)
:   mPeriod(_period),
    mAmplitude(_amplitude),
    mPhase(_phase),
    mArraySize(_arraySize),
    mWaveType(_waveType)
{
    mWaveArray.ensureStorageAllocated(mArraySize);
    plotWave();
    
}
WaveForm::~WaveForm(){
    
}
void WaveForm::plotWave()
{
    switch(mWaveType) {
        case kLFO_Sine:   //sine wave
            plotWaveSine();
            break;
            
        case kLFO_Saw:   //saw wave
            plotWaveSaw();
            break;
            
        case kLFO_Triangle:  //triangle wave
            plotWaveTriangle();
            break;
            
        case kLFO_Square:   //square wave
            plotWaveSquare();
            break;
            
        case kLFO_Noise:   //square wave
            plotWaveNoise();
            break;
            
        default:
            break;
    }
}
void WaveForm::plotWaveSine()
{
    //we need a temp period to scale our periods to a number between 1 and zero
    float tempPeriod = 1.f/mPeriod;
    
    float alfa, beta, delta;
    
    //loop trhough array and set each x
    for(int i = 0; i < static_cast<int>(mArraySize); i++){
        
        //amplitude
        alfa = mAmplitude/2.f;
        
        //period
        beta = MathConstants<float>::twoPi/tempPeriod;
        beta *= (static_cast<float>(i)/mArraySize);
        
        //phase
        delta = mPhase*MathConstants<float>::pi;
        
        //combine period and phase
        beta = sin(beta + delta);
        
        //set y at i
        mWaveArray.set(i, alfa * beta);
        
    }
}
void WaveForm::plotWaveSaw()
{
    
    //we need a temp period to scale our periods to a number between 1 and zero
    float tempPeriod = 1.f/mPeriod;
    
    float alfa, beta, delta;
    
    //just setting this for now
    float inverter = -1.f;
    
    //loop trhough array and set each x
    for(int i = 0; i< static_cast<int>(mArraySize); i++)
    {
        
        //this is the amplitude section
        alfa = mAmplitude/MathConstants<float>::pi;
        
        //this determins period
        beta = (static_cast<float>(i)/mArraySize)*MathConstants<float>::pi;
        beta /= tempPeriod;
        
        //phase
        beta += mPhase*MathConstants<float>::pi;
        
        //bunching phase and period together
        delta = cos(beta)/sin(beta);
        delta = atan(delta);
        
        //set the array x=i
        mWaveArray.set(i, inverter * alfa * delta);
        
    }
}
void WaveForm::plotWaveTriangle()
{
    //we need a temp period to scale our periods to a number between 1 and zero
    float tempPeriod = 1.f/mPeriod;
    
    float alfa, beta, delta;
    
    //loop trhough array and set each x
    for(int i = 0; i< static_cast<int>(mArraySize); i++)
    {
        //amplitude
        alfa = mAmplitude/MathConstants<float>::pi;
        
        //period
        beta = MathConstants<float>::twoPi/tempPeriod;
        beta *= (static_cast<float>(i)/mArraySize);
        
        //phase
        delta = mPhase * MathConstants<float>::pi;
        
        //combine period and phase
        beta = asinf(sinf(beta + delta));
        
        mWaveArray.set(i, alfa * beta);
        
    }
}
void WaveForm::plotWaveSquare()
{
    //we need a temp period to scale our periods to a number between 1 and zero
    float tempPeriod = 1.f/mPeriod;
    
    //loop trhough array and set each x
    for(int i = 1; i < static_cast<int>(mArraySize)+1; i++){
        
        
        //this is calculated by asking a sine wave if its bigger or smaller than 0
        float xvalue = static_cast<float>(i)/mArraySize;
        double value = sin((xvalue / tempPeriod * MathConstants<float>::twoPi)+(mPhase*MathConstants<float>::pi))>=0.0 ? 0.5f:-0.5f;
        
        //apply amplitude
        value *= mAmplitude;
        
        
        mWaveArray.set(i-1, value);
        
    }
}
void WaveForm::plotWaveNoise()
{
    for(int i = 0; i < static_cast<int>(mArraySize); i++){
        Random TempNumber;
        
        Range<int> temRange(-500, 500);
        
        int tempInt = TempNumber.nextInt(temRange);
        
        float tempFloat = static_cast<float>(tempInt)/1000.0f;
        
        tempFloat *= mAmplitude;
        
        mWaveArray.set(i, tempFloat);
        
    }
}

void WaveForm::updateWaveform(float _period, float _amplitude, float _phase, int _waveType)
{
    mPeriod = _period;
    mAmplitude = _amplitude;
    mPhase = _phase;
    mWaveType = _waveType;
    plotWave();
}
