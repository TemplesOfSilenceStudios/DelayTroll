/*
  ==============================================================================

    DTAtomicTimeProcessor.cpp
    Created: 27 Aug 2020 2:23:14pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTAtomicTimeProcessor.h"
#include "DTUsedParameters.h"
#include "DTModulationDefines.h"
#include "DTAudioFunctions.h"

using namespace DTDSP;


AtomicTimeProcessor::AtomicTimeProcessor(OwnedArray<AtomicTroll>& _AtomicTrolls,  DTAtomicHord& _AtomicHord, std::atomic_bool& _updatePaused, OwnedArray<DTModulation::DTAssignableParam>& _assParam, OwnedArray<Value>& _nonModParams, OwnedArray<ModManagment>& _modManagers, OwnedArray<dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>>& _IIRFilters, double& _lastSampleRate):
mAtomicTrollRef(_AtomicTrolls),
mAtomicHordRef(_AtomicHord),
mUpdatePausedRef(_updatePaused),
mAssignableParamsRef(_assParam),
mNonModParamRef(_nonModParams),
mModManagers(_modManagers),
mIIRFiltersRef(_IIRFilters),
mLastSampleRate(_lastSampleRate)
{
    mMults.clear();
    
    mConstraintSections.add(new DTConstraintSection(0,
                                                    Range<float>(0.f, 0.5f),
                                                    Range<int>(0, static_cast<int>(DTNonModulatableDefaultValues[kNMod_NumbOfDels]) -( static_cast<int>(DTNonModulatableDefaultValues[kNMod_NumbOfDels])/2))));
    mConstraintSections.add(new DTConstraintSection(1,
                                                    Range<float>(0.5f, 1.f),
                                                    Range<int>(mConstraintSections[0]->getIndexEnd(), static_cast<int>(DTNonModulatableDefaultValues[kNMod_NumbOfDels]) - 1)));
 
}


AtomicTimeProcessor::~AtomicTimeProcessor(){
    
}

void AtomicTimeProcessor::resizeMult(int _newSize){
    
    jassert(_newSize != mMults.size());
    
    mAtomicHordRef.setHordSizeTarget(_newSize);
    
    int oldSize = mMults.size();
   
    if(mMults.size() < _newSize){
        for (int i = mMults.size(); i < _newSize; i++) {
            mMults.add(std::pair<int, float>(i, -1.f));
        }
    }
    else{
        while(mMults.size() > _newSize){
            mMults.removeLast();
        }
    }
    
    updateMult(_newSize, oldSize);
    
    if(_newSize != oldSize)
        hordSizeUpdate(_newSize);
    
}

void AtomicTimeProcessor::resizeQuant(int _newSize)
{
    //first create new slot positions]
    //DBG("resizing quants");
    
    mQuantSlots.clear();
    float mult = 0.f;
    for (int i = 0; i<_newSize; i++) {
        mult = static_cast<float>(i) * 1.f/static_cast<float>(_newSize-1);
        mQuantSlots.add(std::pair<int, float>(-1,mult));
    }
    
    if (mMults.size() >= mQuantSlots.size()) {
        //shrink the mults if nessiary
        while (mMults.size() > mQuantSlots.size()) {
            mMults.removeLast();
        }
        //assign the mults to the quant slots
        for (int i = 0; i<mQuantSlots.size(); i++) {
            auto& quant = mQuantSlots.getReference(i);
            quant.first = i;
            mMults.getReference(i).second = quant.second;
        }
        mAtomicHordRef.getAtomicHordSize().store(mMults.size(), std::memory_order_release);
        //hordSizeUpdate(mMults.size());
    }
    else{
        //reset times and slots by evenly spreading them out if the number of mults is less than the number of quant slots
        double noteFractionSlotsPerTroll = static_cast<double>(mQuantSlots.size()-1)/static_cast<double>(mMults.size()-1);
        
        double noteFractionSlot = 0.0;
        int noteFractionSlotIndex = 0;
        int lastindex = -1;
        //DBG("Mult SIze: "<<mMults.size()<<" Quant Size: " << mQuantSlots.size()<<" note Fraction Slots Per Troll: "<<noteFractionSlotsPerTroll);
        for (auto & mult : mMults) {
            jassert(lastindex != noteFractionSlotIndex);
            //DBG("fraction slot: "<< noteFractionSlot<<" fraction slot truncated: "<< noteFractionSlotIndex);
            auto& quant = mQuantSlots.getReference(noteFractionSlotIndex);
            mult.second = quant.second;
            quant.first = mult.first;
            noteFractionSlot += noteFractionSlotsPerTroll;
            noteFractionSlotIndex = roundToIntAccurate(noteFractionSlot);
            //DBG("NEXT: fraction slot: "<< noteFractionSlot<<" fraction slot truncated: "<< noteFractionSlotIndex);
        }
        

        
    }
    processCombo();
    updateTimes();
}

void AtomicTimeProcessor::updateRange(double _newMin, double _newMax)
{
    //DBG("Updating Range with values of: ");
    //DBG("newMax: "<< _newMax);
    jassert(_newMin < _newMax);
    mAtomicHordRef.setTimeTarget(static_cast<float>(_newMin), static_cast<float>(_newMax));
    
    //int index = mSelection.getIndex();
    int index = static_cast<int>(mNonModParamRef[kNMod_TimeControlMode]->getValue());
    
    if(index == kTimeOp_QuantizeDelays){
        resizeQuant(mAtomicHordRef.getHordNoteFractionRang().getLength()+1);
    }
    else{
        updateTimes();
    }
    
}

void AtomicTimeProcessor::modulateRange(){
    DTModulation::DTAssignableParam* minParam = mAssignableParamsRef[kAss_MinTime];
    DTModulation::DTAssignableParam* maxParam = mAssignableParamsRef[kAss_MaxTime];
    
    //This produced errors because the shared original value was taking too long to update, temp solution is just to use getNextValue but not sure if this is safe
    if(!minParam->isAttached && !maxParam->isAttached){
        return;
    }
    else if (minParam->isAttached && maxParam->isAttached){
        mAtomicHordRef.setTimeTarget(minParam->getNextValue(), maxParam->getNextValue());
    }
    else if (minParam->isAttached && !maxParam->isAttached){
        mAtomicHordRef.setTimeTarget(minParam->getNextValue(), maxParam->getNextValue());
    }
    else{
        jassert(maxParam->isAttached);
        mAtomicHordRef.setTimeTarget(minParam->getNextValue(), maxParam->getNextValue());
    }
    
    
    if( static_cast<int>(mNonModParamRef[kNMod_TimeControlMode]->getValue()) != kTimeOp_QuantizeDelays){
        updateTimes();
    }
    else{
        mUpdatePausedRef.store(false, std::memory_order_release);
    }
    
}

void AtomicTimeProcessor::updateMult(int _newSize, int _oldSize){
   
    //DBG("UpdatingTimeMults");
    int index = static_cast<int>(mNonModParamRef[kNMod_TimeControlMode]->getValue());
    float fIndex = 0.f;
    float gravety = mAssignableParamsRef[kAss_Gravity]->getNextValue();
    float rGravity = 1.f - (gravety - 1.f);
    Array<float> ConstraintMults;

    
    switch (index) {
        case kTimeOp_Gravity:
            for (auto& mult : mMults)
            {
                fIndex = static_cast<float>(mult.first);
                if(gravety > 1.f){
                    mult.second =1.f-pow(1.f-fIndex/static_cast<float>(mMults.size()-1),rGravity);
                }
                else{
                    mult.second = pow(fIndex/static_cast<float>(_newSize-1), gravety);
                }
            }
            break;
            
        case kTimeOp_ManualControl:
            if(_newSize > _oldSize){
                
                float minSpacer = 1.f/static_cast<float>(DTTotalNumberOfTrolls);
                int numberOfNewTrolls = _newSize - _oldSize;
                //so the idea hear is that new delays will build from the last delay to the position of the critical mult
                float criticalMult = 1.f - minSpacer * static_cast<float>(numberOfNewTrolls);
                int numberOfOutOfBoundsMults = numberOfNewTrolls;
                for (int i = mMults.size()-1; i >= 0; i--)
                {
                    auto& mult = mMults.getReference(i);
                    //if a mult is above the critical mult then that means the critical mult is one spacer back
                    if(mult.second > criticalMult){
                        criticalMult -= minSpacer;
                        numberOfOutOfBoundsMults++;
                    }
                }
                float backwardsCounter = 0;
                for (int i = mMults.size()-1; i >= mMults.size() - numberOfOutOfBoundsMults; i--) {
                    auto& mult = mMults.getReference(i);
                    mult.second = 1.f - minSpacer * backwardsCounter++;
                    jassert(mult.second > 0.f);
                }
            }
            else{
                mMults.getReference(mMults.size() - 1).second = 1.f;
            }
            break;
            
        case kTimeOp_Divisitions:
            mConstraintSections.getLast()->setIndexEnd(_newSize-1);
            for(auto Sec : mConstraintSections){
                ConstraintMults.addArray(Sec->getMultipliers());
            }
            
            for(auto& mult: mMults){
                mult.second = ConstraintMults[mult.first];
            }
            break;
            
        case kTimeOp_BPMSync:
            for (auto& mult : mMults)
            {
                fIndex = static_cast<float>(mult.first);
                if(gravety > 1.f){
                    mult.second =1.f-pow(1.f-fIndex/static_cast<float>(mMults.size()-1),rGravity);
                }
                else{
                    mult.second = pow(fIndex/static_cast<float>(_newSize-1), gravety);
                }
            }
            break;
            
        case kTimeOp_QuantizeDelays:
            //assume new number of delays is valid
            if(_newSize > _oldSize && _newSize != mQuantSlots.size() && _oldSize !=0){
                Array<int> openQuants;
               //find some open quantslots to put the new delays into
                for(int i = 1; i<mQuantSlots.size();i++)
                {
                    int first = mQuantSlots[i].first;
                    if(first < 0)
                    {
                        openQuants.add(i);
                        //DBG("adding quantslot: "<<i);
                    }
                }
                
                jassert(_newSize - _oldSize <= openQuants.size());
                //set the mults to the open quant positions mults with a vlue of -1 have not been set yet
                int openQuantIndex = 0;
                for(auto& mult: mMults){
                    //DBG("mult: "<<mult.first<<" time: "<<mult.second);
                    if(mMults.getLast().first == mult.first)
                    {
                        mQuantSlots.getReference(mQuantSlots.size()-1).first = mult.first;
                        mult.second = 1.f;
                    }
                    else if(mult.second < 0.f || mult.second == 1.f){
                        auto& quant = mQuantSlots.getReference(openQuants[openQuantIndex]);
                        quant.first = mult.first;
                        mult.second = quant.second;
                        openQuantIndex++;
                        //DBG("Quant: "<<openQuants[openQuantIndex - 1]<<" set to: "<<quant.first<<" giving a value Of: "<<mult.second);
                    }
                }
            }
            else if (_newSize > _oldSize && _newSize == mQuantSlots.size()){
                for (int i = 0; i<mQuantSlots.size(); i++) {
                    auto& quant = mQuantSlots.getReference(i);
                    quant.first = i;
                    mMults.getReference(i).second = quant.second;
                }
            }
            else if (_oldSize == 0){
                
                for (int i = 1; i>_newSize; i++) {
                    auto& quant = mQuantSlots.getReference(i);
                    quant.first = i;
                    mMults.getReference(i).second = quant.second;
                }
                    
            }
            else{
                mMults.getReference(mMults.size()-1).second = 1.f;
                
                for (auto& quant: mQuantSlots){
                    if(quant.first >= mMults.size() - 1)
                    {
                        quant.first = -1;
                    }
                   // DBG("post quant subtraction: "<<quant.first);
                }
                
            }
            mQuantSlots.getReference(mQuantSlots.size()-1).first = mMults.size() - 1;
            mQuantSlots.getReference(0).first = 0;

            break;
            
        default:
            break;
    }
    mMults.getReference(0).second = 0.f;
    mMults.getReference(mMults.size()-1).second = 1.f;
    processCombo();
    updateTimes();
}

void AtomicTimeProcessor::updateMult()
{
    int index = static_cast<int>(mNonModParamRef[kNMod_TimeControlMode]->getValue());
    float fIndex = 0.f;
    float gravety = mAssignableParamsRef[kAss_Gravity]->getNextValue();
    float rGravity = 1.f - (gravety - 1.f);
    Array<float> ConstraintMults;
    //DBG("Updatind Mult");
       
       switch (index) {
           case kTimeOp_Gravity:
               for (auto& mult : mMults)
               {
                   fIndex = static_cast<float>(mult.first);
                   if(gravety > 1.f){
                       mult.second =1.f-pow(1.f-fIndex/static_cast<float>(mMults.size()-1),rGravity);
                   }
                   else{
                       mult.second = pow(fIndex/static_cast<float>(mMults.size()-1), gravety);
                   }
               }
               break;
               
           case kTimeOp_ManualControl:
               break;
               
           case kTimeOp_Divisitions:
               for(auto Sec : mConstraintSections){
                   ConstraintMults.addArray(Sec->getMultipliers());
               }
               
               for(auto& mult: mMults){
                   mult.second = ConstraintMults[mult.first];
               }
               break;
               
           case kTimeOp_BPMSync:
               for (auto& mult : mMults)
               {
                   fIndex = static_cast<float>(mult.first);
                   if(gravety > 1.f){
                       mult.second =1.f-pow(1.f-fIndex/static_cast<float>(mMults.size()-1),rGravity);
                   }
                   else{
                       mult.second = pow(fIndex/static_cast<float>(mMults.size()-1), gravety);
                   }
               }
               break;
               
           case kTimeOp_QuantizeDelays:{
               //assume new number of delays is valid
               double noteFractionSlotsPerTroll = static_cast<double>(mQuantSlots.size()-1)/static_cast<double>(mMults.size()-1);
               
               double noteFractionSlot = 0.0;
               int noteFractionSlotIndex = 0;
               for (auto & mult : mMults) {
                   auto& quant = mQuantSlots.getReference(noteFractionSlotIndex);
                   mult.second = quant.second;
                   quant.first = mult.first;
                   noteFractionSlot += noteFractionSlotsPerTroll;
                   noteFractionSlotIndex = roundToIntAccurate(noteFractionSlot);
               }
           }
               break;
               
           default:
               break;
       }
    mMults.getReference(0).second = 0.f;
    mMults.getReference(mMults.size()-1).second = 1.f;
    
    processCombo();
    updateTimes();
}

void AtomicTimeProcessor::updateTimes(){
    
    //DBG("Updating Time Targets");
    mAtomicHordRef.updateTrollRange();
    for (auto mult : mMults){
        //DBG("troll time from mult: "<<mult.first<<" : "<< mAtomicHordRef.getTrollTimeFromMult(mult.second));
        mAtomicTrollRef[mult.first]->setTrollTarget(TrollParams::kTroll_DelayTime, mAtomicHordRef.getTrollTimeFromMult(mult.second));
    }
    //DBG("unpausing at atomic time");
    mUpdatePausedRef.store(false, std::memory_order_release);
    
}

void AtomicTimeProcessor::processLevel()
{
    if(static_cast<int>(mNonModParamRef[kNMod_LevelManuelMode]->getValue()) == kDCOp_Manual){
        return;
    }
    
    double* curveArray = static_cast<BezierPlotter*>(mModManagers[kMod_LevelBez])->getArray();
    float v0, v1, dist;
    int index;
    float value;
    
    for(auto mult: mMults)
    {
        v0 = mult.second * static_cast<float>(DTmodArraySize - 1);
        //mModManagers[kMod_LevelBez]->getarraySize();
        index = truncatePositiveToUnsignedInt(v0);
        dist = v0 - static_cast<float>(index);
        v0 = curveArray[index];
        v1 = index == DTmodArraySize-1 ? curveArray[DTmodArraySize-1] : curveArray[index + 1];
        value = DT_linear_interp(v0, v1, dist);
        //DBG("Processing: "<<mult.first<<" Level: "<<value);
        
        mAtomicTrollRef[mult.first]->setTrollTarget(TrollParams::kTroll_Amplitude, value);
        mAtomicTrollRef[mult.first]->setTrollGUITarget(TrollParams::kTroll_Amplitude, value);
    }
}

void AtomicTimeProcessor::processEQ(int _TrollID)
{
    float lowvalue, highvalue, centerfreq, Q;
    
    if(static_cast<int>(mNonModParamRef[kNMod_EQManuelMode]->getValue()) == kDCOp_Manual){
        
        if(_TrollID == -1){//-1 means process all trolls (when adding new delays to the mult for example)
            
            for(auto mult: mMults){
                lowvalue = mAtomicTrollRef[mult.first]->mGUIAttributes[TrollParams::kTroll_LowPassFreq];
                highvalue = mAtomicTrollRef[mult.first]->mGUIAttributes[TrollParams::kTroll_HighPassFreq];
                
                centerfreq = std::sqrtf(lowvalue*highvalue);
                Q = centerfreq/(lowvalue - highvalue);
                Q= Q <= 0.f? 0.0001f: Q;
                //DBG("Mult: "<<mult.first<<" Processing EQ: "<< highvalue<<" : "<<lowvalue);
                *mIIRFiltersRef[mult.first]->state = *dsp::IIR::Coefficients<float>::makeBandPass(mLastSampleRate, centerfreq, Q);
            }
        }
        else{
            jassert(_TrollID < mMults.size());
            lowvalue = mAtomicTrollRef[_TrollID]->mGUIAttributes[TrollParams::kTroll_LowPassFreq];
            highvalue = mAtomicTrollRef[_TrollID]->mGUIAttributes[TrollParams::kTroll_HighPassFreq];
            
            centerfreq = std::sqrtf(lowvalue*highvalue);
            Q = centerfreq/(lowvalue - highvalue);
            Q= Q <= 0.f? 0.0001f: Q;
            //DBG("Mult: "<<mult.first<<" Processing EQ: "<< highvalue<<" : "<<lowvalue);
            *mIIRFiltersRef[_TrollID]->state = *dsp::IIR::Coefficients<float>::makeBandPass(mLastSampleRate, centerfreq, Q);
        }
        
        return;
    }//if manual mode
    
    double* lowPassCurveArray = static_cast<BezierPlotter*>(mModManagers[kMod_EQLowPassBez])->getArray();

    double* highPassCurveArray = static_cast<BezierPlotter*>(mModManagers[kMod_EQHighPassBez])->getArray();
    
    NormalisableRange<float> tempRange = TrollParams::TrollAttributeNormalRanges[TrollParams::kTroll_LowPassFreq];
    //tempRange.setSkewForCentre(600.f);
    
    //DBG("skew for eq: "<< tempRange.skew);
    
    float v0, v1, dist;
    int index;

    for(auto mult: mMults)
    {
        v0 = mult.second * static_cast<float>(DTmodArraySize-1);
        index = truncatePositiveToUnsignedInt(v0);
        dist = v0 - static_cast<float>(index);
        v0 = lowPassCurveArray[index];
        v1 = index == DTmodArraySize-1 ? lowPassCurveArray[DTmodArraySize-1] : lowPassCurveArray[index+1];
        lowvalue = DT_linear_interp(v0, v1, dist);
        v0 = highPassCurveArray[index];
        v1 = index == DTmodArraySize-1 ? highPassCurveArray[DTmodArraySize-1] : highPassCurveArray[index+1];
        highvalue = DT_linear_interp(v0, v1, dist);
        
        highvalue = tempRange.convertFrom0to1(highvalue);
        lowvalue = tempRange.convertFrom0to1(lowvalue);
        
        mAtomicTrollRef[mult.first]->setTrollTarget(TrollParams::kTroll_LowPassFreq, lowvalue);
        mAtomicTrollRef[mult.first]->setTrollGUITarget(TrollParams::kTroll_LowPassFreq, lowvalue);
        mAtomicTrollRef[mult.first]->setTrollTarget(TrollParams::kTroll_HighPassFreq, highvalue);
        mAtomicTrollRef[mult.first]->setTrollGUITarget(TrollParams::kTroll_HighPassFreq, highvalue);
    
        centerfreq = std::sqrtf(lowvalue*highvalue);
        Q = centerfreq/(lowvalue - highvalue);
        Q= Q <= 0.f? 0.0001f: Q;
        //DBG("Mult: "<<mult.first<<" Processing EQ: "<< highvalue<<" : "<<lowvalue);
        *mIIRFiltersRef[mult.first]->state = *dsp::IIR::Coefficients<float>::makeBandPass(mLastSampleRate, centerfreq, Q);
    }
    
}

void AtomicTimeProcessor::processPitch()
{
    
    if(static_cast<int>(mNonModParamRef[kNMod_PitchManuelMode]->getValue()) != kPitchOp_Bez){
        return;
    }
    
    double* curveArray =  static_cast<BezierPlotter*>(mModManagers[kMod_PitchBez])->getArray();
    
    NormalisableRange<float> tempRange = TrollParams::TrollAttributeNormalRanges[TrollParams::kTroll_PitchFactor];
    //tempRange.setSkewForCentre(1.f);
    //DBG("skew for pitch: "<< tempRange.skew);
    float v0, v1, dist;
    int index;
    float value;
    
    for (auto mult  : mMults) {
        v0 = mult.second * static_cast<float>(DTmodArraySize-1);
        index = truncatePositiveToUnsignedInt(v0);
        dist = v0 - static_cast<float>(index);
        v0 = curveArray[index];
        v1 = index == DTmodArraySize-1 ? curveArray[DTmodArraySize-1] : curveArray[index + 1];
        value = DT_linear_interp(v0, v1, dist);
        value = tempRange.convertFrom0to1(value);
        mAtomicTrollRef[mult.first]->setTrollTarget(TrollParams::kTroll_PitchFactor, value);
        mAtomicTrollRef[mult.first]->setTrollGUITarget(TrollParams::kTroll_PitchFactor, value);
    }
    
}

void AtomicTimeProcessor::processPan()
{
    if(!(static_cast<int>(mNonModParamRef[kNMod_PanManuelMode]->getValue()) == kPanOp_Bez ||
       static_cast<int>(mNonModParamRef[kNMod_PanManuelMode]->getValue()) == kPanOP_BezBal)){
        return;
    }
    
    double* curveArrayLeft =  static_cast<BezierPlotter*>(mModManagers[kMod_LeftPanBez])->getArray();
    double* curveArrayRight = static_cast<BezierPlotter*>(mModManagers[kMod_RightPanBez])->getArray();
    double* curveArrayBal =static_cast<BezierPlotter*>(mModManagers[kMod_BalPanBez])->getArray();
    
    float v0, v1, dist;
    int index;
    float valueLeft, valueRight;
    
    for (auto mult  : mMults) {
        v0 = mult.second * static_cast<float>(DTmodArraySize-1);
        index = truncatePositiveToUnsignedInt(v0);
        dist = v0 - static_cast<float>(index);
        if(static_cast<int>(mNonModParamRef[kNMod_PanManuelMode]->getValue()) == kPanOp_Bez){
            v0 = curveArrayLeft[index];
            v1 = index == DTmodArraySize-1 ? curveArrayLeft[DTmodArraySize-1] : curveArrayLeft[index + 1];
            valueLeft = DT_linear_interp(v0, v1, dist);
            v0 = curveArrayRight[index];
            v1 = index == DTmodArraySize-1 ? curveArrayRight[DTmodArraySize-1] : curveArrayRight[index + 1];
            valueRight = DT_linear_interp(v0, v1, dist);
            valueRight = 1.f - valueRight;
        }
        else{
            v0 = curveArrayBal[index];
            v1 = index == DTmodArraySize-1 ? curveArrayBal[DTmodArraySize-1] : curveArrayBal[index + 1];
            valueLeft = DT_linear_interp(v0, v1, dist);
            valueRight = valueLeft;
            valueRight = valueRight <= 0.5f? 1.f:1.f - abs(0.5f - valueRight)*2.f;
            valueLeft = valueLeft >= 0.5f ? 1.f: 1.f -(0.5f - valueLeft)*2.f;
            
        }
        mAtomicTrollRef[mult.first]->setTrollTarget(TrollParams::kTroll_LeftAtenuation, valueLeft);
        mAtomicTrollRef[mult.first]->setTrollGUITarget(TrollParams::kTroll_LeftAtenuation, valueLeft);
        mAtomicTrollRef[mult.first]->setTrollTarget(TrollParams::kTroll_RightAtenuation, valueRight);
        mAtomicTrollRef[mult.first]->setTrollGUITarget(TrollParams::kTroll_RightAtenuation, valueRight);
    }
    
}

void AtomicTimeProcessor::AtomicActivate()
{
    mAtomicListeners.call([this](AtomicListener& l) { l.atomicMultUpdated(this);});
}

void AtomicTimeProcessor::hordSizeUpdate(int _newSize)
{
    mAtomicListeners.call([this, _newSize](AtomicListener& l){ l.newHordSize(_newSize);});
}
/**sets the parameters of the section based on the provided XMLElement*/
XmlElement* AtomicTimeProcessor::getTimeProcessorXMLElement(){
    
    StringRef atomicTimeStringRef = "AtomicTimeProcessor";
    XmlElement* atomicElement = new XmlElement(atomicTimeStringRef);
     //DBG("saving mMults: "<<mMults.size());
    for (auto mult : mMults) {
        Identifier multID = String("multID" + String(mult.first));
        atomicElement->setAttribute(multID, static_cast<double>(mult.second));
    }
   // DBG("saving quants: "<<mQuantSlots.size());
    for (int i = 0; i<mQuantSlots.size(); i++) {
        Identifier quantID = String("quantID" + String(i));
        atomicElement->setAttribute(quantID, static_cast<double>(mQuantSlots[i].second));
    }
    
    
  //  DBG("constraint size saving: "<<mConstraintSections.size());
    for (int i = 0; i<mConstraintSections.size(); i++) {
        atomicElement->addChildElement(mConstraintSections[i]->getXMLElement());
    }
    
    for(int i = 0; i<mAtomicTrollRef.size(); i++){
        atomicElement->addChildElement(mAtomicTrollRef[i]->getTrollXMLElement(i));
    }
    
    return atomicElement;
}

/**sets the parameters of the section based on the provided XMLElement*/
void AtomicTimeProcessor::setTimeProcessorXMLElement(XmlElement* _xmlElement){
    /**THIS SHOULD ONLY BE CALLED AFTER A SPINLOCK PROTECTOR**/
   // DBG("=======================================================================");
   // DBG(_xmlElement->getTagName());
    
    mMults.clear();
    mQuantSlots.clear();
    for (int i = 0; i<_xmlElement->getNumAttributes(); i++) {
        //DBG(_xmlElement->getAttributeName(i));
        String tester = _xmlElement->getAttributeName(i).upToLastOccurrenceOf(StringRef("D"), true, false);
        if(tester == "multID")
        {
            mMults.add(std::pair<int, float>(mMults.size(), static_cast<float>(_xmlElement->getDoubleAttribute(_xmlElement->getAttributeName(i)))));
        }
        else if (tester == "quantID"){
            mQuantSlots.add(std::pair<int, float>(-1, static_cast<float>(_xmlElement->getDoubleAttribute(_xmlElement->getAttributeName(i)))));
        }
        
    }
    mAtomicHordRef.setHordSizeTarget(mMults.size());
    for(auto& quant: mQuantSlots){
        for (auto mult : mMults) {
            if(approximatelyEqual(mult.second, quant.second))
            {
                quant.first = mult.first;
            }
        }
    }
    
    
    //DBG("Number of Children: "<<_xmlElement->getNumChildElements());
    mConstraintSections.clear();
    forEachXmlChildElement(*_xmlElement, e)
    {
        //DBG(e->getTagName());
        String tester = e->getTagName().upToLastOccurrenceOf(StringRef("D"), true, false);
        String number = e->getTagName().fromLastOccurrenceOf(StringRef("D"), false, false);
        if(tester == "trollID"){
            mAtomicTrollRef[number.getIntValue()]->setTrollXMLElement(e);
        }
        else{
            mConstraintSections.add(new DTConstraintSection());
            mConstraintSections.getLast()->setXMLElement(e);
        }
    }
    
    //the IIR Filters need resetting but everythen else should be hanndled by setTrollXMLElement
    float lowvalue, highvalue, centerfreq, Q;
    for(auto mult: mMults){
        lowvalue = mAtomicTrollRef[mult.first]->mGUIAttributes[TrollParams::kTroll_LowPassFreq];
        highvalue = mAtomicTrollRef[mult.first]->mGUIAttributes[TrollParams::kTroll_HighPassFreq];
        centerfreq = std::sqrtf(lowvalue*highvalue);
        Q = centerfreq/(lowvalue - highvalue);
        Q= Q <= 0.f? 0.0001f: Q;
        //DBG("Mult: "<<mult.first<<" Loading EQ: "<< highvalue<<" : "<<lowvalue);
        *mIIRFiltersRef[mult.first]->state = *dsp::IIR::Coefficients<float>::makeBandPass(mLastSampleRate, centerfreq, Q);
    }

    AtomicActivate();
    hordSizeUpdate(mMults.size());
    updateTimes();
    
   // DBG("=======================================================================");
}
