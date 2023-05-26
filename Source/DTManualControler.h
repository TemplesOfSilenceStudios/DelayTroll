/*
  ==============================================================================

    DTManualControler.h
    Created: 1 Sep 2020 9:28:05am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
namespace DTManualControl{
enum DTManualModes{
    kManMode_TopOnly = 0,
    kManMode_TopAndBottom,
    kManMode_Split,
    kManMode_Xeditable,
    kManMode_QuantizedX,
    kManMode_QuantizedY,
    kManMode_InvertedSplit,
    kManMode_TotalNumManModes
};

class DTManualControler  : public juce::Component
{
public:
    DTManualControler(DelayTrollAudioProcessor* _p, int _topControlParamIndex, DTManualModes _mode, int _bottomControlParamIndex = -1);
    ~DTManualControler() override;

    void paint (juce::Graphics&) override;
    /**selects point*/
    void mouseDown(const MouseEvent& e) override;
    /**changes a points hight or x */
    void mouseDrag(const MouseEvent& e) override;
    /**just resets the slection index to -1*/
    void mouseUp(const MouseEvent& e) override;
    
    inline void setMode(DTManualModes _newMode){mMode = _newMode;};
    


private:
    DelayTrollAudioProcessor* mProcessorPtr;
    DTDSP::AtomicTimeProcessor& mTimeProcessor;
    Array<std::pair<int, float>>& mMults;
    Array<std::pair<int, float>>& mQuantSlots;
    std::atomic_bool& mPausedRef;
    std::atomic_bool& mUpdateRef;
    
    DTManualModes mMode;
    int mTopControlParamIndex, mBottomControlParamIndex;
    
    OwnedArray<AtomicTroll>& mAtomicTrollRef;
    DTAtomicHord& mAtomicHordRef;
    
    AffineTransform mScaleDown;
    AffineTransform mScaleUp;
    int mLineSelection;
    bool mTopSelected;
    
    Font mFont;
    AttributedString mAttString;
    String  mSuffix;
    BubbleMessageComponent mBubble;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTManualControler)
};

}
