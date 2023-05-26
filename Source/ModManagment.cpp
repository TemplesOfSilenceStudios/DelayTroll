/*
  ==============================================================================

    ModManagment.cpp
    Created: 19 Mar 2019 5:16:55pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "ModManagment.h"
#include "DTModulationDefines.h"
#include "DTUsedParameters.h"

ModManagment::ModManagment(double* const& Array, float _arraySize, int _managmentID, Value& _BPM, Value& _timeSigNumerator):
m_plotArray(Array),
m_arraySize(_arraySize),
mArratIndexEnd(_arraySize - 1.f),
mBPMManager(_BPM, _timeSigNumerator)
{
    mBPMManager.setMaxNoteFractions(5.f * 1000.f);
    plotPossition = 0;
    mPosition = 0.f;
    mStepSize = 2.5f;
    mManagerID = _managmentID;
    m_ms = 5;
    mQuantizationSelection = 4;
    mBPMSyncMode = false;
    mMaxNumberOfBarFractions = 20.f;
    
    for (int i = 0; i< kAss_TotalNumOFAssignableParams; i++) {
        depthValues.add(new Value(var(0.5)));
    }
    clearAllValues();
}

XmlElement* ModManagment::getManagerXMLElement(int _managerNum)
{
    //DBG(_managerNum<<": Saving");
    
    //gives our xml element a name and creates it
    String managerNumberString = "Manager";
    managerNumberString += _managerNum;
    StringRef managerNumberStringRef = managerNumberString;
    XmlElement* managerNumber = new XmlElement(managerNumberStringRef);
    
    XmlElement* bpmManagerElement = mBPMManager.getManagerXMLElement(_managerNum);
    managerNumber->addChildElement(bpmManagerElement);
    
    Identifier polarID = "PolarID";
    Identifier timerIntervalID = "TimerIntervalID";
    Identifier stepSizeID = "StepSizeID";
    Identifier QuantizationSelectionID = "QuantizationSelectionID";
    Identifier BPMSyncID = "BPMSyncID";
    //Identifier BarFractionsID = "BarFractionsID";
    
    managerNumber->setAttribute(polarID, static_cast<float>(mPolar.getValue()));
    managerNumber->setAttribute(timerIntervalID, m_ms);
    managerNumber->setAttribute(stepSizeID, mStepSize);
    managerNumber->setAttribute(QuantizationSelectionID, mQuantizationSelection);
    managerNumber->setAttribute(BPMSyncID, static_cast<int>(mBPMSyncMode));
    //managerNumber->setAttribute(BarFractionsID, static_cast<float>(mBarFractions.getValue()));
    

    //passes it the depth and value index so when reloaded we can figgure our what value to attatch it to
    for(int i = 0; i < mParamTypes.size(); i++)
    {
        String depthValuesIDString = "depthValuesID";
        String paramTypeIDString = "paramTypeIDString";
        String isPlaceholderIDSTring = "isPlaceholderIDSTring";
        
        depthValuesIDString += i;
        paramTypeIDString += i;
        isPlaceholderIDSTring += i;
        
        Identifier depthValuesID = depthValuesIDString;
        Identifier paramTypeID = paramTypeIDString;
        Identifier isPlaceholderID = isPlaceholderIDSTring;
        
        
        if(mParamTypes[i] != -1){
            managerNumber->setAttribute(depthValuesIDString, static_cast<double>(depthValues[mParamTypes[i]]->getValue()));
//            if(_managerNum == 9)
//            {
//                DBG("Saving mod management");
//                DBG(_managerNum);
//                DBG(managerNumberString<<": "<<depthValues[mParamTypes[i]]->getValue().toString());
//            }
            managerNumber->setAttribute(paramTypeID, mParamTypes[i]);
            managerNumber->setAttribute(isPlaceholderID, static_cast<int>(mIsPlaceholder[i]));
        }
        else{
            managerNumber->setAttribute(depthValuesIDString, 0.5);
            managerNumber->setAttribute(paramTypeID, mParamTypes[i]);
            managerNumber->setAttribute(isPlaceholderID, static_cast<int>(mIsPlaceholder[i]));
        }

    
        
    }

    //returns the xml element, someone will have to clean this up
    return managerNumber;
}

void ModManagment::setManagerXMLElement(juce::XmlElement *_xmlElement)
{

    clearAllValues();
    
    mBPMManager.setManagerXMLElement(_xmlElement->getFirstChildElement());
    
    //DBG(mManagerID<<": Loading with size: "<< mParamTypes.size());
    mPolar.setValue(_xmlElement->getDoubleAttribute("PolarID"));
    m_ms = _xmlElement->getIntAttribute("TimerIntervalID");
    mStepSize = _xmlElement->getDoubleAttribute("StepSizeID");
    mQuantizationSelection = _xmlElement->getIntAttribute("QuantizationSelectionID");
    mBPMSyncMode = static_cast<bool>(_xmlElement->getIntAttribute("BPMSyncID"));
    //mBarFractions.setValue(var(_xmlElement->getDoubleAttribute("BarFractionsID")));
    
    //adds values for every 3 attribues -2 for the polar and ms
    for(int i = 0; i < (_xmlElement->getNumAttributes()-5)/3; i++)
    {
        String depthValuesIDString = "depthValuesID";
        String paramTypeIDString = "paramTypeIDString";
        String isPlaceholderIDSTring = "isPlaceholderIDSTring";
        
        depthValuesIDString += i;
        paramTypeIDString += i;
        isPlaceholderIDSTring += i;
        
        Identifier depthValuesID = depthValuesIDString;
        Identifier paramTypeID = paramTypeIDString;
        Identifier isPlaceholderID = isPlaceholderIDSTring;
        
        mParamTypes.add(_xmlElement->getIntAttribute(paramTypeIDString));
        
        //DBG(mManagerID <<": "<< _xmlElement->getIntAttribute(paramTypeIDString));
        
        if(_xmlElement->getIntAttribute(paramTypeIDString) != -1){
            depthValues[_xmlElement->getIntAttribute(paramTypeIDString)]->setValue(_xmlElement->getDoubleAttribute(depthValuesIDString));
//            if(_xmlElement->getTagName() == "Manager9")
//            {
//                DBG("Loading");
//                DBG(_xmlElement->getTagName()<<": "<<depthValues[_xmlElement->getIntAttribute(paramTypeIDString)]->getValue().toString());
//            }
        }
        
        mIsPlaceholder.add(static_cast<bool>(_xmlElement->getIntAttribute(isPlaceholderID)));
    }
    //DBG(mManagerID<<": type list size post load: "<< mParamTypes.size());
    
    
}

void ModManagment::updateStepSize(double _msPerCycle)
{
    if(mBPMSyncMode){
        if(static_cast<float>(mBPMManager.getQuantTime().getValue()) == 0.f)
            return;
        
        mStepSize = m_arraySize/static_cast<float>(mBPMManager.getQuantTime().getValue()) * static_cast<float>(DTModulationUpdateFrequency);
    }
    else{
        mStepSize = m_arraySize/_msPerCycle*static_cast<float>(DTModulationUpdateFrequency);
    }
    //DBG("stepsize by time: "<< mStepSize);
}

void ModManagment::updateStepSizeByNoteFraction()
{
    
    //DBG("Updating StepSize with quant time of: "<<static_cast<float>(mBPMManager.getQuantTime().getValue()));
    //DBG("Quant Time: "<< static_cast<float>(mBPMManager.getQuantTime().getValue()));
    mStepSize = m_arraySize/static_cast<float>(mBPMManager.getQuantTime().getValue()) * static_cast<float>(DTModulationUpdateFrequency);
    //DBG("stepsize by Fraction: "<< mStepSize);
}

double ModManagment::getStepSizeAsMsPerCycle()
{
    double updatesPerSecond = 1.0 / static_cast<double>(DTModulationUpdateFrequency);
    double stepsPerUpdate = mStepSize * updatesPerSecond;
    double MsPerCycle = (double)m_arraySize /stepsPerUpdate;
    return MsPerCycle;
}

void ModManagment::setPositionFromNotefractions(double _notefractionsSinceEdit)
{
    double cyclesperNoteFraction = mBPMManager.getNoteFractionsInMS()/getStepSizeAsMsPerCycle();
    double cyclesSinceEdit = _notefractionsSinceEdit * cyclesperNoteFraction;
    double positionInArray = cyclesSinceEdit - trunc(cyclesSinceEdit);
    mPosition = positionInArray * (m_arraySize - 1.f);
    
}

void ModManagment::setPosition(double _timeInMs)
{
    double cyclesSinceEdit = _timeInMs/getStepSizeAsMsPerCycle();
    double positionInArray = cyclesSinceEdit - trunc(cyclesSinceEdit);
    mPosition = positionInArray * (m_arraySize - 1.f);
    plotPossition = roundToInt(mPosition);
    //DBG("Setting Position of: "<< getManagerID()<<" with a position of: "<<mPosition);
}

void ModManagment::setBPMSyncMode(bool _HzShouldSync)
{
    mBPMSyncMode = _HzShouldSync;
    
    if(mBPMSyncMode)
        mBPMManager.setNoteFractionsFromTime(getStepSizeAsMsPerCycle());
}
