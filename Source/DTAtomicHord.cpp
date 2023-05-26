/*
  ==============================================================================

    DTAtomicHord.cpp
    Created: 19 Dec 2022 2:26:18pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTAtomicHord.h"

DTAtomicHord::DTAtomicHord(float _min, float _max, int _size){
    initializeHord(_min, _max, _size);
}

void DTAtomicHord::initializeHord(float _min, float _max, int _size){
    mAtomicHordSize.store(_size, std::memory_order_relaxed);
    mCurrentSize = _size;
    
    jassert(_min < _max);
    mTimes.first.store(_min, std::memory_order_relaxed);
    mTimes.second.store(_max, std::memory_order_relaxed);
    mHordRange = NormalisableRange<float>(_min, _max);
}

void DTAtomicHord::updateHordSizeToTarget(){
    mNewSize = mAtomicHordSize.load(std::memory_order_acquire);
    
    if( mNewSize != mCurrentSize)
    {
        mCurrentSize = mNewSize;
    }
}

void DTAtomicHord::setTimeTarget(float _min, float _max){
    
    if(_min >= _max){
        DBG("SHOULD NEVER RUN");
        jassertfalse;
        if(_min <= 0.f){
            _max = _min + 1.f;
        }
        else if (_max >= 10000.f)
        {
            _min = _max - 1.f;
        }
        else{
            _max = _min + 1.f;
        }
    }
    
    mTimes.first.store(_min, std::memory_order_release);
    mTimes.second.store(_max, std::memory_order_release);
    
}
