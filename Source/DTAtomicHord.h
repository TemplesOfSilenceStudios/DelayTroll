/*
  ==============================================================================

    DTAtomicHord.h
    Created: 19 Dec 2022 2:26:18pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DTAtomicHord {
public:
    /**class representing and managing some global Troll variables*/
    DTAtomicHord(float _min, float _max, int _size);
    
    /**initializes some static variables assosiated with the hord*/
    void initializeHord(float _min, float _max, int _size);
    
    /**sets the target size of processing delay lines*/
    inline void setHordSizeTarget(int _NewSize){mAtomicHordSize.store(_NewSize, std::memory_order_release);}
    
    /**updates the total size of processing delay lines */
    void updateHordSizeToTarget();
    
    /**sets the min and max time for a future troll range update call*/
    void setTimeTarget(float _min, float _max); 
    
    /**updates the hord range via atomicly stored min and max times*/
    inline void updateTrollRange(){
        //DBG("Updating The HordRange with Values of: ");
        //DBG("mtimes.second: "<<mTimes.second.load(std::memory_order_acquire));
        mHordRange = NormalisableRange<float>(mTimes.first.load(std::memory_order_acquire), mTimes.second.load(std::memory_order_acquire));
        jassert(!mHordRange.getRange().isEmpty());
    };
    
    /**sets the range of note fractions that the processing delay lines will fall between*/
    inline void setHordNoteFractionRange(int _newMin, int _newMax){
        jassert(_newMin < _newMax);
        mHordNoteFractionRange = Range<int>(_newMin, _newMax);
        jassert(!mHordNoteFractionRange.isEmpty());
    }
    /**directly updates the hord range*/
    inline void updateHordRange(float _min, float _max)
    {
        jassert(_min < _max);
        mHordRange = NormalisableRange<float>(_min,_max);
    };
    
    /**gets the current time of a delay line based on its % position in the hord range*/
    inline float getTrollTimeFromMult(float _mult){ return mHordRange.convertFrom0to1(_mult);};
    
    /**gets the current number of delay taps*/
    inline int getCurrentHordSize(){return mCurrentSize;};
    
    inline std::atomic_int& getAtomicHordSize(){return mAtomicHordSize;};
    
    inline Range<int>& getHordNoteFractionRang(){return mHordNoteFractionRange;};
    
    
private:
    NormalisableRange<float> mHordRange;
    Range<int> mHordNoteFractionRange;
    
    int mCurrentSize;
    int mNewSize;
    
    std::atomic_int mAtomicHordSize;
    
    std::pair<std::atomic<float>, std::atomic<float>> mTimes;
    std::pair<std::atomic_int, std::atomic_int> mNoteFractions;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTAtomicHord)
};
