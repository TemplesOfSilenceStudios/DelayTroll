/*
  ==============================================================================

    DTVuMeter.cpp
    Created: 5 Apr 2020 12:31:28pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DTVuMeter.h"
#include "DTAudioFunctions.h"
#include "DTUsedParameters.h"
//==============================================================================
DTVuMeter::DTVuMeter(DelayTrollAudioProcessor& _inProcessor): mCh0Level(0.f), mCh1Level(0.f), mParameterID(-1), mProcessor(_inProcessor)
{
    setBufferedToImage(true);
}

DTVuMeter::~DTVuMeter()
{
}

void DTVuMeter::paint (Graphics& g)
{
    
    const int meter_height = getHeight() / 2;
    
    //draw empty meter
    g.setColour(juce::Colours::dimgrey);
   // g.fillRect(0, meter_height, getWidth(), meter_height);
    //g.fillRect(0, meter_height*2, getWidth(), meter_height);


    g.setColour(juce::Colours::green);
    
    int ch0fill, ch1fill;
    //fill meters
    switch (mParameterID) {
        case kParam_InputLevel:{
            ch0fill = getWidth() - (getWidth() * mCh0Level);
            ch1fill = getWidth() - (getWidth() * mCh1Level);
            
            if(ch0fill < 0)
                ch0fill = 0;
            
            if(ch1fill < 0)
                ch1fill = 0;
            
            g.fillRect(ch0fill, 0, getWidth(), meter_height);
            g.fillRect(ch1fill, meter_height, getWidth(), meter_height);
            
            
        }break;
            
        case kParam_OutputLevel:{
            ch0fill = getWidth() * mCh0Level;
            ch1fill = getWidth() * mCh1Level;
            
            if(ch0fill < 0)
                ch0fill = 0;
            
            if(ch1fill < 0)
                ch1fill = 0;
            
            g.fillRect(0, 0, ch0fill, meter_height);
            g.fillRect(0, meter_height, ch1fill, meter_height);
            
        }break;
            
        default:
            break;
    }
    
    
}

void DTVuMeter::timerCallback()
{
    float updatedCh0Level = 0.0f;
    float updatedCh1Level = 0.0f;
    
    switch (mParameterID) {
        case kParam_InputLevel:
            updatedCh0Level = mProcessor.getInputGainMeterLevel(0);
            updatedCh1Level = mProcessor.getInputGainMeterLevel(1);
            
        break;
            
        case kParam_OutputLevel:
            updatedCh0Level = mProcessor.getOutputGainMeterLevel(0);
            updatedCh1Level = mProcessor.getOutputGainMeterLevel(1);
            
        break;
            
        default:
            break;
    }
    
    if(updatedCh0Level > mCh0Level){
        mCh0Level = updatedCh0Level;
    }
    else{
        mCh0Level = kMeterSmoothingCoeff * (mCh0Level - updatedCh0Level) + updatedCh0Level;
    }
    
    if(updatedCh1Level > mCh1Level){
        mCh1Level = updatedCh1Level;
    }
    else{
        mCh1Level = kMeterSmoothingCoeff * (mCh1Level - updatedCh1Level) + updatedCh1Level;
    }
    //check to make sure the number is not so small it should just be 0
    mCh0Level = DT_denormalize(mCh0Level);
    mCh1Level = DT_denormalize(mCh1Level);
    
    if(mCh0Level || mCh1Level)
        repaint();
}

void DTVuMeter::setParameterID(int _inParameterID)
{
    mParameterID = _inParameterID;
    
    startTimer(100);
}

void DTVuMeter::resized()
{
    const int meter_height = getHeight() / 2;
    
    mDrawrec = Rectangle<int>(0, 0, getWidth(), meter_height);
}
