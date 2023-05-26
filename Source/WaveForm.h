/*
  ==============================================================================

    WaveForm.h
    Created: 26 Apr 2019 8:52:39am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"


/**represents a single waveform*/
class WaveForm
{
public:
    /**creates new waveform with the given parameters type and size*/
    WaveForm(float _period, float _amplitude, float _phase, int _waveType, float _arraySize);
    ~WaveForm();
    
    /**plots the waveform to the internal array*/
    void plotWave();
    
    //various ploting algorythems
    void plotWaveSine();
    void plotWaveSaw();
    void plotWaveTriangle();
    void plotWaveSquare();
    void plotWaveNoise();
    
    /**Updates waveform with the incoming parameters*/
    void updateWaveform(float _period, float _amplitude, float _phase, int _waveType);
    
    //getters abd setters
    inline float getPeriod(){return mPeriod;};
    inline void setPeriod(float _period){mPeriod = _period;};
    inline float getAmp(){return mAmplitude;};
    inline void setAmp(float _amplitude){mAmplitude = _amplitude;};
    inline float getPhase(){return mPhase;};
    inline void setPhase(float _phase){mPhase = _phase;};
    inline int getWaveType(){return mWaveType;};
    
    /**overloaded [] operator to access the wave values*/
    const double operator[](int i) const{return mWaveArray[i];};
private:
    float mPeriod;
    float mAmplitude;
    float mPhase;
    float mArraySize;
    int mWaveType;
    
    //the array that holds each individual wave (per this object)
    Array<double> mWaveArray;
};
