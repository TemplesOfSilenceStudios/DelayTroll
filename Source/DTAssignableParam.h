/*
  ==============================================================================

    DTAssignamableParam.h
    Created: 17 Aug 2020 12:18:10pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ModManagment.h"

namespace DTModulation {
struct DTAssignableParam//: public AudioParameterFloat
{
    DTAssignableParam(int _Type, AudioParameterFloat& _value);
    
    String paramID;
    String paramName;
    HashMap<int, Value> modulatorDepths;
    HashMap<int, Value> modulatorPolar;
    HashMap<int, ModManagment*> modulationManagers;
    //maybe change this to value
    var modulatedValue;
    NormalisableRange<float> range;
    bool isAttached;
    int typeIndex;
       
    var minDepthValue;
    var maxDepthValue;
       
    AudioParameterFloat& sharedOrigninalValue;
   
    
    float ofset;
    
    float currentValue, currentTarget;
    int stepsToTarget;
    
    void setValue(float _newValue);
    void updateTarget();
    
    int getParamIndex();
    
    float getNextValue();
    
    private:
    float step;
    int countdown;
    std::atomic<float> target;

};


}
