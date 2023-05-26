/*
  ==============================================================================

    LFOPlotter.h
    Created: 19 Mar 2019 5:07:37pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
//#include "../JuceLibraryCode/JuceHeader.h"
#include "ModManagment.h"
#include "WaveForm.h"

//**Plotter class that "plots" an array by combining LFO Waveforms*/
class LFOPlotter : public ModManagment
{
    public:
    /**Creates a new LFOPlotter to plot the provided array with the provided size*/
    LFOPlotter(double* const& Array, float _arraySize, float _polar, int _managmentID, Value& _BPM, Value& _timeSigNumerator);
    ~LFOPlotter();
    
    /**adds a new wave to the array*/
    void addWave(float _period, float _amplitude, float _phase, int _waveType);
    
    /**removes a wave*/
    void removeWave(int _index);
    
    /**Combines all the waves and compresses it to a value between 0.5 and -0.5*/
    void compressWave();
    
    /**updates a wave with new peramiters*/
    void updateWave(int _index, float _period, float _amplitude, float _phase, int _waveType);
    
    /**changes the type of wave we are looking at*/
    void typeCastUpdateWave(int _index, int _waveType);
    
    /**retuns how many waves are in the plotter*/
    inline int getNumWaves(){return mWaveArray.size();};
    
    /**clear the plot and waves*/
    void clearWaves(){
        mWaveArray.clear();
        clearPlotArray();
    };
    
    /**gets and XMLElement representing the state of the plotter*/
    XmlElement* getXMLElement(int _plotNum);
    
    /**sets the plotters pararameters based off the incomeing XMLElement*/
    void setXMLElement(XmlElement*);
    
    /**returns a waveform*/
    WaveForm* getWaveForm(int _index){return mWaveArray[_index];};
    
    /**overloads the [] operator for easy access to the plot*/
    const double& operator[](int i) const{return m_plotArray[i];};
    
private:
    //a class of waveforms with 4 plotting functions
    OwnedArray<WaveForm> mWaveArray;
    
};
