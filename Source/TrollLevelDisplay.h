/*
  ==============================================================================

    TrollLevelDisplay.h
    Created: 23 Apr 2019 9:44:19am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "TrollDisplayPanelBase.h"
#include "BezierCurveDisplay.h"
//==============================================================================
/**repesents and manages interactive controls for the Trolls amplitude Parameter
*/
class TrollLevelDisplay    : public TrollDisplayPanelBase, public ChangeListener, public DelayTrollAudioProcessor::Listener
{
public:
    TrollLevelDisplay(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel);
    ~TrollLevelDisplay();

    void paint (Graphics&) override;
    
    void loadActivated(DelayTrollAudioProcessor* _p) override;
    
    void setComponentVisability(int _controlMode) override;
    
    /**handles changes to the BezierCurveDisplay component*/
    void changeListenerCallback (ChangeBroadcaster* source) override {
        if(dynamic_cast<BezierCurveDisplay*>(source) == mCurve.get() && isVisible()){
            timerCallback();
        }
        repaint();};
    void timerCallback() override{
        if(isVisible()){
            if(mMode.getSelectedId() != 1)
                mProcessor.getAtomicTimeProcessor().processLevel();
            
            mDisplayComponent.setLines();
            repaint();
        }
    };


private:
    
    std::unique_ptr<BezierCurveDisplay> mCurve;
    

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrollLevelDisplay)
};
