/*
 ==============================================================================
 
 DTAtomicTimeProcessor.h
 Created: 27 Aug 2020 2:23:14pm
 Author:  Sam Ferguson
 
 ==============================================================================
 */

#pragma once
#include "AtomicTroll.h"
#include "DTAtomicHord.h"
#include "DTAssignableParam.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "DTConstraintSection.h"
#include "BezierPlotter.h"

namespace DTDSP {
/**The time processor is inteded to handle changes to the trolls delaytime and any attribute changes that said change would intail*/
class AtomicTimeProcessor {
public:
    /**The time processor is inteded to handle changes to the trolls delaytime and any attribute changes that said change would intail*/
    AtomicTimeProcessor(OwnedArray<AtomicTroll>& _AtomicTrolls,
                        DTAtomicHord& _AtomicHord,
                        std::atomic_bool& _updatePaused,
                        OwnedArray<DTModulation::DTAssignableParam>& _assParam,
                        OwnedArray<Value>& _nonModParams,
                        OwnedArray<ModManagment>& _modManagers,
                        OwnedArray<dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>>& _IIRFiltersRef,
                        double& _lastSampleRate);
    
    ~AtomicTimeProcessor();
    
    /**Sets a new size for the mult representing the delay times*/
    void resizeMult(int _newSize);
    
    /**updates the range of the delay trolls, if the notefraction range has changed that must be set before calling this */
    void updateRange(double _newMin, double _newMax);
    
    /**handles changes to the range in the case that the min and max times are modulating*/
    void modulateRange();

    /**resizes the array representing the quantized positions and reduces the mult size if nessisary*/
    void resizeQuant(int _newSize);
    
    /**updates the array representing the delay times as a % of the hord range*/
    void updateMult();
    
    /**updates the times of of the actual trolls based off the mult array*/
    void updateTimes();
    
    //updates some atributes of the delays
    void processLevel();
    void processEQ(int _TrollID = -1);
    void processPitch();
    void processPan();
    void processCombo(){
        processLevel();
        processEQ();
        processPitch();
        processPan();
    };
    
    /**listener class to update the GUI when a Cange has occured*/
    class AtomicListener
    {
    public:
        virtual ~AtomicListener() = default;
        virtual void atomicMultUpdated(AtomicTimeProcessor*) = 0;
        virtual void newHordSize(int _newSize){};
    };
    
    /**add listener*/
    void addListener(AtomicListener* listenerToAdd){mAtomicListeners.add(listenerToAdd);};
    /**remove listener*/
    void removeListener(AtomicListener* listenerToRemove){jassert(mAtomicListeners.contains(listenerToRemove)); mAtomicListeners.remove(listenerToRemove);};
    
    /**calling this function will bang the gui for an update when preset is loaded*/
    void AtomicActivate();
    
    //getters
    Array<std::pair<int, float>>& getMults(){return mMults;};
    
    Array<std::pair<int, float>>& getQuants(){return mQuantSlots;};
    
    OwnedArray<DTConstraintSection>& getConstraintSections(){return mConstraintSections;};
    
    /**provides an XMLElement representing a saved state of this processors data*/
    XmlElement* getTimeProcessorXMLElement();
    
    /**sets the parameters of based on the provided XMLElement*/
    void setTimeProcessorXMLElement(XmlElement* _xmlElement);
    
    void hordSizeUpdate(int _newSize);
    
private:
    
    /**updates the mult after a resize operation*/
    void updateMult(int _newSize, int _oldSize);
    
    /**an array representing the delay time as a percentage of the way between the min and max times*/
    Array<std::pair<int, float>> mMults;
    OwnedArray<AtomicTroll>& mAtomicTrollRef;
    DTAtomicHord& mAtomicHordRef;
    std::atomic_bool& mUpdatePausedRef;
    OwnedArray<DTModulation::DTAssignableParam>& mAssignableParamsRef;
    OwnedArray<Value>& mNonModParamRef;
    //an array representing the divided sections when the time control methoed selected by the user is constraints
    OwnedArray<DTConstraintSection> mConstraintSections;
    /*an array representing the quantized position of each note fraction as a percentage of the way between the min and max times,
     also contains an index of an assigned mult, an int value of -1 is considered and empty slot
     */
    Array<std::pair<int, float>> mQuantSlots;
    OwnedArray<ModManagment>& mModManagers;
    OwnedArray<dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>>>& mIIRFiltersRef;
    double& mLastSampleRate;
    //array to hold listeners
    ListenerList<AtomicListener> mAtomicListeners;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AtomicTimeProcessor)
    
};

}
