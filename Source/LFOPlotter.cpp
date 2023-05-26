/*
  ==============================================================================

    LFOPlotter.cpp
    Created: 19 Mar 2019 5:07:37pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "LFOPlotter.h"
#include "DTModulationDefines.h"

LFOPlotter::LFOPlotter(double* const& Array, float _arraySize, float _polar, int _managmentID, Value& _BPM, Value& _timeSigNumerator) : ModManagment(Array, _arraySize, _managmentID, _BPM, _timeSigNumerator)
{
    //clear the array for good measure
    clearPlotArray();
    mPolar.setValue(_polar);
    addWave(1.0f, 1.0f, 1.0f, kLFO_Sine);
}
LFOPlotter::~LFOPlotter()
{
}
void LFOPlotter::addWave(float _period, float _amplitude, float _phase, int _waveType)
{
    //DBG("adding Wave");
    switch(_waveType) {
        case kLFO_Sine:   //sine wave
           
            mWaveArray.add(new WaveForm( _period,  _amplitude,  _phase, kLFO_Sine, m_arraySize));
            compressWave();
            break;

        case kLFO_Saw:   //saw wave
            mWaveArray.add(new WaveForm( _period,  _amplitude,  _phase, kLFO_Saw, m_arraySize));
            compressWave();
            break;

        case kLFO_Triangle:  //triangle wave
            mWaveArray.add(new WaveForm( _period,  _amplitude,  _phase, kLFO_Triangle, m_arraySize));
            compressWave();
            break;

        case kLFO_Square:   //square wave
            mWaveArray.add(new WaveForm( _period,  _amplitude,  _phase, kLFO_Square, m_arraySize));
            compressWave();
            break;
            
        case kLFO_Noise:   //square wave
            mWaveArray.add(new WaveForm( _period,  _amplitude,  _phase, kLFO_Noise, m_arraySize));
            compressWave();
            break;

        default:
            break;
    }
    
}
void LFOPlotter::removeWave(int _index)
{
    //check to make sure we are not trying to remove a null pointer
    if(_index >= mWaveArray.size() || _index < 0)
    {
        DBG("removing null index");
        return;
    }

    //remove clear and recompress
    mWaveArray.remove(_index);
    clearPlotArray();
    compressWave();
}
void LFOPlotter::updateWave(int _index, float _period, float _amplitude, float _phase, int _waveType)
{
    //runn our update wave function, maybe should just do this through getabstract waves
    if(_index >= mWaveArray.size() || _index < 0)
    {
        DBG("updating null index");
        return;
    }
    else{
        clearPlotArray();
        mWaveArray[_index]->updateWaveform(_period, _amplitude, _phase, _waveType);
        compressWave();
    }
}
void LFOPlotter::typeCastUpdateWave(int _index, int _waveType)
{
    //mWaveArray[_index]->updateWaveform(_period, _amplitude, _phase, _waveType);
    
}
void LFOPlotter::compressWave()
{
    
    float top = 0;
    //add up all the waves set top to the highes absolute value
    for(int j = 0; j < mWaveArray.size(); j++)
    {
        for(int i = 0; i < static_cast<int>(m_arraySize); i++)
        {
            m_plotArray[i] += (*mWaveArray[j])[i];
            abs(m_plotArray[i]) > top ? top = abs(m_plotArray[i]) : top = top;
        }
    }

    //checks to see if top is greater than 0.5, if so then compress the wave by multipling it by the 0.5 devided by top
    //right now its set to 0.49 for painting puposes but will probably change that and move it to the painting function
    if(abs(top) >= 0.5f)
    {
        for(int i = 0; i < static_cast<int>(m_arraySize); i++)
        {
            m_plotArray[i] *= (0.49f/top);
        }
    }
    for(int i = 0 ; i < static_cast<int>(m_arraySize); i++){
        m_plotArray[i] += 0.5f;
    }
}
XmlElement* LFOPlotter::getXMLElement(int _plotNum)
{
    //set a name for our element and create a new element
    String plotNumberString = "LFOPlot";
    plotNumberString += _plotNum;
    StringRef plotNumberStringRef = plotNumberString;
    XmlElement* plotNumber = new XmlElement(plotNumberStringRef);
    
    //DBG("LFO SAVING: "<<mWaveArray.size());
    
    //add our attributes to the element for each waveform
    for(int i = 0; i < mWaveArray.size(); i++)
    {
        String ampString = "ampID";
        String periodString = "periodID";
        String phaseString = "phaseID";
        String waveTypeString = "waveTypeID";

        ampString += i;
        periodString += i;
        phaseString += i;
        waveTypeString += i;

        Identifier ampID = ampString;
        Identifier periodID = periodString;
        Identifier phaseID = phaseString;
        Identifier waveTypeID = waveTypeString;


        plotNumber->setAttribute(ampID, static_cast<double>(mWaveArray[i]->getAmp()));
        plotNumber->setAttribute(periodID, static_cast<double>(mWaveArray[i]->getPeriod()));
        plotNumber->setAttribute(phaseID, static_cast<double>(mWaveArray[i]->getPhase()));
        plotNumber->setAttribute(waveTypeID, mWaveArray[i]->getWaveType());
    }
    //return the element someones gonna have to clean this up
    return plotNumber;
}
void LFOPlotter::setXMLElement(juce::XmlElement *_xmlElement)
{
    //clears the waveform and plot array
    mWaveArray.clear();
    clearPlotArray();
    
    //DBG("LFO Number of attributes: "<< _xmlElement->getNumAttributes());
    
    //run though all our atribues and add waves for every 4 attribues
    for(int i = 0; i < _xmlElement->getNumAttributes()/4; i++)
    {
        String ampString = "ampID";
        String periodString = "periodID";
        String phaseString = "phaseID";
        String waveTypeString = "waveTypeID";

        ampString += i;
        periodString += i;
        phaseString += i;
        waveTypeString += i;

        Identifier ampID = ampString;
        Identifier periodID = periodString;
        Identifier phaseID = phaseString;
        Identifier waveTypeID = waveTypeString;

        clearPlotArray();

        addWave(_xmlElement->getDoubleAttribute(periodID), _xmlElement->getDoubleAttribute(ampID), _xmlElement->getDoubleAttribute(phaseID), _xmlElement->getIntAttribute(waveTypeID));
    }
    //DBG("LFO Number of waves: "<< mWaveArray.size());

}
