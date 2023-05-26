/*
  ==============================================================================

    DTVirticalConstraintDisplay.h
    Created: 31 Aug 2020 3:03:26pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/

class DTVirticalConstraintDisplay  : public juce::Component, public Timer, public DTDSP::AtomicTimeProcessor::AtomicListener
{
public:
    DTVirticalConstraintDisplay(DelayTrollAudioProcessor* _p);
    ~DTVirticalConstraintDisplay() override;

    void paint (juce::Graphics&) override;
    
    /**seleccts line*/
    void mouseDown(const MouseEvent& e) override;
    /**changes a lines x values */
    void mouseDrag(const MouseEvent& e) override;
    /**removes a line*/
    void mouseDoubleClick(const MouseEvent& e) override;
    /**not sure yet*/
    void mouseUp(const MouseEvent& e) override;
    /**ment to replace mouse drag, not implemented yet*/
    void timerCallback() override;
    
    void atomicMultUpdated(DTDSP::AtomicTimeProcessor* _tp) override {
        mLines.clear();
        for(int i = 1; i < mConstraintSectionsRef.size(); i++)
              {
                  Line<float> newLine(mConstraintSectionsRef[i]->getRangeStart(),
                                       static_cast<float>(getLocalBounds().getBottom()),
                                       mConstraintSectionsRef[i]->getRangeStart(),
                                       static_cast<float>(getLocalBounds().getY()));
                  
                  newLine.applyTransform(mScaleUp);
                  
                  mLines.set(mConstraintSectionsRef[i]->getIndexStart(),
                                       Line<float>(newLine));
              }
        repaint();
    };
    
private:
    
    
    int mLineSelection;
    AffineTransform mScaleDown;
    AffineTransform mScaleUp;
    
    Point<float> mCurrentMousePoint;
    HashMap<int, Line<float>> mLines;
    
    DelayTrollAudioProcessor& mProcessor;
    
    DTDSP::AtomicTimeProcessor& mTimeProcessor;
    OwnedArray<DTConstraintSection>& mConstraintSectionsRef;
    Array<std::pair<int, float>>& mMultsRef;
    std::atomic_bool& mPause;
    std::atomic_bool& mUpdate;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTVirticalConstraintDisplay)
};
