/*
  ==============================================================================

    BPMManager.cpp
    Created: 2 Aug 2020 10:56:18am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "BPMManager.h"
#include "DTUsedParameters.h"
#include  "DTAudioFunctions.h"


BPMManager::BPMManager(Value& _BPM, Value& _timeSigNumerator):
    mBPMRef(_BPM), mTimeSigNumeratorRef(_timeSigNumerator)
{
    //0.5Hz at 120BPM in 4/4 Time quantized to 4
    mNoteFractions.setValue(var(4.f));
    mMaxtimeinMs = maxDelayTimeInSec * 1000.f;
    //mBPMInfo.mQuantizationSelectionID = DTIntParameterDefaultValue[kIParam_QuantizationSelection - kParam_TotalNumFloatParam] + 1;
    mBPMInfo.mQuantizationSelectionID = static_cast<int>(DTNonModulatableDefaultValues[kNMod_QuantizationSelection]);
    mBPMLabelInfo.mDenominatorValue.setValue(var(DTBPMQuantizationDenominators[mBPMInfo.mQuantizationSelectionID-1]));
    mBPMInfo.mQuantizationMult = DTBPMQuantizationMults[mBPMInfo.mQuantizationSelectionID - 1];
    mBeatsInMS = 60000.f / static_cast<float>(mBPMRef.getValue());
    mNoteFracttionsInMS = (mBeatsInMS * static_cast<float>(mTimeSigNumeratorRef.getValue()))/mBPMInfo.mQuantizationMult;
    mQuantizedTime.setValue(var(static_cast<float>(mNoteFractions.getValue()) * mNoteFracttionsInMS));
    mMaxNoteFractions = var(floor( mMaxtimeinMs/mNoteFracttionsInMS));
    mBPMInfo.mWholeNotes = static_cast<int>(floor(static_cast<float>(mNoteFractions.getValue())/mBPMInfo.mQuantizationMult));
    mBPMInfo.mNotesNumerator = static_cast<int>(static_cast<float>(mNoteFractions.getValue()) - static_cast<float>(mBPMInfo.mWholeNotes) * mBPMInfo.mQuantizationMult);
    mBPMLabelInfo.mWholeNoteValue.setValue(var(mBPMInfo.mWholeNotes));
    mBPMLabelInfo.mNumeratorValue.setValue(var(mBPMInfo.mNotesNumerator));
    //mBPMRef.addListener(this);
    //mTimeSigNumeratorRef.addListener(this);
    
}

void BPMManager::initalizeBPMInfo()
{
    //0.5Hz at 120BPM in 4/4 Time quantized to 4
    mNoteFractions.setValue(var(4.f));
    mMaxtimeinMs = maxDelayTimeInSec * 1000.f;
    //mBPMInfo.mQuantizationSelectionID = DTIntParameterDefaultValue[kIParam_QuantizationSelection - kParam_TotalNumFloatParam] + 1;
    mBPMInfo.mQuantizationSelectionID = static_cast<int>(DTNonModulatableDefaultValues[kNMod_QuantizationSelection]);
    mBPMLabelInfo.mDenominatorValue.setValue(var(DTBPMQuantizationDenominators[mBPMInfo.mQuantizationSelectionID-1]));
    mBPMInfo.mQuantizationMult = DTBPMQuantizationMults[mBPMInfo.mQuantizationSelectionID - 1];
    mBeatsInMS = 60000.f / static_cast<float>(mBPMRef.getValue());
    mNoteFracttionsInMS = (mBeatsInMS * static_cast<float>(mTimeSigNumeratorRef.getValue()))/mBPMInfo.mQuantizationMult;
    mQuantizedTime.setValue(var(static_cast<float>(mNoteFractions.getValue()) * mNoteFracttionsInMS));
    mMaxNoteFractions = var(floor( mMaxtimeinMs/mNoteFracttionsInMS));
    mBPMInfo.mWholeNotes = static_cast<int>(floor(static_cast<float>(mNoteFractions.getValue())/mBPMInfo.mQuantizationMult));
    mBPMInfo.mNotesNumerator = static_cast<int>(static_cast<float>(mNoteFractions.getValue()) - static_cast<float>(mBPMInfo.mWholeNotes) * mBPMInfo.mQuantizationMult);
    mBPMLabelInfo.mWholeNoteValue.setValue(var(mBPMInfo.mWholeNotes));
    mBPMLabelInfo.mNumeratorValue.setValue(var(mBPMInfo.mNotesNumerator));
}

BPMManager::~BPMManager()
{
    //mBPMRef.removeListener(this);
    //mTimeSigNumeratorRef.removeListener(this);
}
 
void BPMManager::setNoteFractions(float _noteFractions)
{
    if(var(_noteFractions) > mMaxNoteFractions)
        DBG("note Fractions Clipped set from fraction");
    
    var newNoteFractions = var(_noteFractions) > mMaxNoteFractions ? mMaxNoteFractions : var(_noteFractions);
    
    mNoteFractions.setValue(newNoteFractions);
    
    mBPMInfo.mWholeNotes = static_cast<int>(floor(static_cast<float>(mNoteFractions.getValue())/mBPMInfo.mQuantizationMult));
    
    mBPMInfo.mNotesNumerator = static_cast<int>(static_cast<float>(mNoteFractions.getValue()) - static_cast<float>(mBPMInfo.mWholeNotes) * mBPMInfo.mQuantizationMult);
    mBPMLabelInfo.mWholeNoteValue.setValue(var(mBPMInfo.mWholeNotes));
    mBPMLabelInfo.mNumeratorValue.setValue(var(mBPMInfo.mNotesNumerator));
    setQuantizeTime();
}

 void BPMManager::setNoteFractionsFromTime(double _timeInMs)
{
    var newNoteFractons = var(floor(_timeInMs/mNoteFracttionsInMS));
    
    if(newNoteFractons > mMaxNoteFractions)
        DBG("noteFractions Clipped set from time");
    
    newNoteFractons = newNoteFractons > mMaxNoteFractions ? mMaxNoteFractions : newNoteFractons;
    
    mNoteFractions.setValue(newNoteFractons);
    
    mBPMInfo.mWholeNotes = static_cast<int>(floor(static_cast<float>(mNoteFractions.getValue())/mBPMInfo.mQuantizationMult));
    
    mBPMInfo.mNotesNumerator = static_cast<int>(static_cast<float>(mNoteFractions.getValue()) - static_cast<float>(mBPMInfo.mWholeNotes) * mBPMInfo.mQuantizationMult);
    mBPMLabelInfo.mWholeNoteValue.setValue(var(mBPMInfo.mWholeNotes));
    mBPMLabelInfo.mNumeratorValue.setValue(var(mBPMInfo.mNotesNumerator));
    //DBG("max Num setting: "<< mBPMInfo.mNotesNumerator);
    //DBG("max mult setting: "<< mBPMInfo.mWholeNotes);
    //DBG("Beats in MS AT setter: "<< mNoteFracttionsInMS);
    
    
    setQuantizeTime();
    
}

void BPMManager::setQuantizeTime()
{
    //DBG("Setting quant Time");
    //DBG("noteFractions: "<< static_cast<float>(mNoteFractions.getValue()));
    //DBG("noteFractions in Ms: "<< mNoteFracttionsInMS); 
    
    mQuantizedTime.setValue(var(static_cast<float>(mNoteFractions.getValue()) * mNoteFracttionsInMS));
    //DBG("Setting Quant Time: "<<static_cast<float>(mQuantizedTime.getValue()));
    jassert(!isnan(static_cast<float>(mQuantizedTime.getValue())));
    //DBG("Setting QuantizedTime At Manager: "<< static_cast<float>(mQuantizedTime.getValue()));
}

void BPMManager::setMaxNoteFractions(float _maxTime)
{
    mMaxtimeinMs = _maxTime;
    
    mMaxNoteFractions =  var(floor(mMaxtimeinMs/mNoteFracttionsInMS));
    
    if(mNoteFractions.getValue() > mMaxNoteFractions){
        DBG("Note fractions Clipped");
        setNoteFractions(static_cast<float>(mMaxNoteFractions));
    }
    
}

float BPMManager::getNoteFractionsAsHz()
{
    return 1.f/static_cast<float>(mQuantizedTime.getValue()) * 1000.f;
}

void BPMManager::updatePositionInfo()
{
    //DBG("Updateing position info With BPM: "<<static_cast<float>(mBPMRef.getValue()));
    
    
    mBeatsInMS = 60000.f / static_cast<float>(mBPMRef.getValue());
    //DBG("Updating Position infor with a Beats in ms of: "<<mBeatsInMS);
    //DBG("Time sig numerator: "<<static_cast<float>(mTimeSigNumeratorRef.getValue()));
    //DBG("QuantizationMult: "<<mBPMInfo.mQuantizationMult);
    mNoteFracttionsInMS = (mBeatsInMS * static_cast<float>(mTimeSigNumeratorRef.getValue()))/mBPMInfo.mQuantizationMult;
    setMaxNoteFractions(mMaxtimeinMs);
    //DBG("Updateing position info: "<< mNoteFracttionsInMS);
    setQuantizeTime();
    
}

void BPMManager::updateQuantizationMult(int _newQuantizationSelection)
{
    float oldNumberofNotes = static_cast<float>(mNoteFractions.getValue())/mBPMInfo.mQuantizationMult;
    mBPMInfo.mQuantizationSelectionID = _newQuantizationSelection;
    mBPMLabelInfo.mDenominatorValue.setValue(var(DTBPMQuantizationDenominators[mBPMInfo.mQuantizationSelectionID-1]));
    mBPMInfo.mQuantizationMult = DTBPMQuantizationMults[mBPMInfo.mQuantizationSelectionID - 1];
    mNoteFracttionsInMS = (mBeatsInMS * static_cast<float>(mTimeSigNumeratorRef.getValue()))/mBPMInfo.mQuantizationMult;
    
    setMaxNoteFractions(mMaxtimeinMs);
    
    setNoteFractions(floor(oldNumberofNotes * mBPMInfo.mQuantizationMult));
    
    
    mBPMInfo.mWholeNotes = static_cast<int>(floor(static_cast<float>(mNoteFractions.getValue())/mBPMInfo.mQuantizationMult));
    mBPMInfo.mNotesNumerator = static_cast<int>(static_cast<float>(mNoteFractions.getValue()) - static_cast<float>(mBPMInfo.mWholeNotes) * mBPMInfo.mQuantizationMult);
    mBPMLabelInfo.mWholeNoteValue.setValue(var(mBPMInfo.mWholeNotes));
    mBPMLabelInfo.mNumeratorValue.setValue(var(mBPMInfo.mNotesNumerator));
}


void BPMManager::setManagerXMLElement(XmlElement *_xmlElement)
{
    mNoteFractions.setValue(_xmlElement->getDoubleAttribute("NoteFractionsID"));
    mBPMInfo.mQuantizationSelectionID = _xmlElement->getIntAttribute("QuantizationSelectionID");
    mBPMLabelInfo.mDenominatorValue.setValue(var(DTBPMQuantizationDenominators[mBPMInfo.mQuantizationSelectionID-1]));
    mBPMInfo.mQuantizationMult = DTBPMQuantizationMults[mBPMInfo.mQuantizationSelectionID - 1];
    mBeatsInMS = 60000.f / _xmlElement->getDoubleAttribute("BPMID");
    mNoteFracttionsInMS = (mBeatsInMS * _xmlElement->getDoubleAttribute("BPNID"))/mBPMInfo.mQuantizationMult;
    mQuantizedTime.setValue(var(static_cast<float>(mNoteFractions.getValue()) * mNoteFracttionsInMS));
    mMaxNoteFractions = var(floor( mMaxtimeinMs/mNoteFracttionsInMS));
    mBPMInfo.mWholeNotes = static_cast<int>(floor(static_cast<float>(mNoteFractions.getValue())/mBPMInfo.mQuantizationMult));
    mBPMInfo.mNotesNumerator = static_cast<int>(static_cast<float>(mNoteFractions.getValue()) - static_cast<float>(mBPMInfo.mWholeNotes) * mBPMInfo.mQuantizationMult);
    mBPMLabelInfo.mWholeNoteValue.setValue(var(mBPMInfo.mWholeNotes));
    mBPMLabelInfo.mNumeratorValue.setValue(var(mBPMInfo.mNotesNumerator));
}

XmlElement*  BPMManager::getManagerXMLElement(int _managerNum)
{
    //gives our xml element a name and creates it
    String managerNumberString = "BPMManager";
    managerNumberString += _managerNum;
    StringRef managerNumberStringRef = managerNumberString;
    XmlElement* managerNumber = new XmlElement(managerNumberStringRef);
   
    Identifier NoteFractionsID = "NoteFractionsID";
    Identifier QuantizationSelectionID = "QuantizationSelectionID";
    Identifier BPMID = "BPMID";
    Identifier BPNID = "BPNID";

   
    managerNumber->setAttribute(NoteFractionsID, static_cast<float>(mNoteFractions.getValue()));
    managerNumber->setAttribute(QuantizationSelectionID, mBPMInfo.mQuantizationSelectionID);
    managerNumber->setAttribute(BPMID, static_cast<float>(mBPMRef.getValue()));
    managerNumber->setAttribute(BPNID, static_cast<float>(mTimeSigNumeratorRef.getValue()));
    
    return managerNumber;
}

