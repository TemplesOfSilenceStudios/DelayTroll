/*
  ==============================================================================

    BPMManager.h
    Created: 2 Aug 2020 10:56:18am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
/**manages a quantized time based on the BPM and time signature provided*/
class BPMManager //:public Value::Listener
{
public:
    /**manages a quantized time based on the BPM and time signature provided*/
    BPMManager(Value& _BPM, Value& _timeSigNumerator);
    /**initalizes some BPM info*/
    void initalizeBPMInfo();
    ~BPMManager();
    
    /**sets the note fractions directly*/
    void setNoteFractions(float _noteFractions);
    /**sets the note fractions from the provided time*/
    void setNoteFractionsFromTime(double _timeInMs);
    
    /**sets the maximum number of note fractions*/
    void setMaxNoteFractions(float _maxTime);

    /**sets the quantized time off the notefraction*/
    void setQuantizeTime();
    
    /**updates the notfractions when the BPM, time signature or other contextual data changes*/
    void updatePositionInfo();
    
    /**updates the notefractions  based on the income notfraction selection*/
    void updateQuantizationMult(int _newQuantizationSelection);
    
    //Getters
    Value& getNoteFractions(){return mNoteFractions;};
    float getNoteFractionsAsHz();
    
    /**retruns a refferece tot the Value object that holds the current quantized time in BPM*/
    Value& getQuantTime(){return mQuantizedTime;};
    
    /**retruns the maximum number of note fractions available at the current quantization amount and BPM*/
    var getMaxNoteFraction(){ return mMaxNoteFractions;};
    
    /**returns the lenght of 1 beat in ms at the current BPM*/
    double& getBeatsInMs(){return mBeatsInMS;};
    double& getNoteFractionsInMS(){return mNoteFracttionsInMS;};
    /**ment to hold info about the BPM for latter use, probably more troube than it's worth*/
    struct BPMInfo{
        float mQuantizationMult;
        int mQuantizationSelectionID;
        
        int mWholeNotes, mNotesNumerator;
    };
    BPMInfo& getBPMInfo(){return mBPMInfo;};
    int getNoteFractionInt(){return static_cast<int>(mNoteFractions.getValue());};
    
    /**some values that lables can use to convienently display BPM relevent values*/
    struct BPMLabelValues{
        Value mNumeratorValue;
        Value mDenominatorValue;
        Value mWholeNoteValue;
    }mBPMLabelInfo;
    
    
    /**gets an xmlelement representing the Managers state*/
    XmlElement* getManagerXMLElement(int _managerNum);
    
    /**sets the Managers state via an xmlelement*/
    void setManagerXMLElement(XmlElement* _xmlElement);
    
private:
    double mBeatsInMS, mNoteFracttionsInMS;
    
    Value mQuantizedTime;
    
    //A notefraction represents 1 note divided by the quantization ammount so if the user selects 1/64 then 1 note will be 64 notFractions
    Value mNoteFractions;
    
    BPMInfo mBPMInfo;
    
    Value& mBPMRef, mTimeSigNumeratorRef;
    
    var mMaxNoteFractions;
    
    float mMaxtimeinMs;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BPMManager)
};
