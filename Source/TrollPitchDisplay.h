/*
  ==============================================================================

    TrollPitchDisplay.h
    Created: 31 Jan 2020 6:34:05pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "TrollDisplayPanelBase.h"
#include "BezierCurveDisplay.h"
#include "DTButton.h"
//#include "TrollPitchControlPanel.h"

//==============================================================================
/**repesents and manages interactive controls for the Trolls Temporal Location
 */
class TrollPitchDisplay    : public TrollDisplayPanelBase, public ChangeListener, public DelayTrollAudioProcessor::Listener
{
public:
    TrollPitchDisplay(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel);
    ~TrollPitchDisplay();
    
    void paint (Graphics&) override;
    
    void loadActivated(DelayTrollAudioProcessor* _p) override;
    
    void setComponentVisability(int _controlMode) override;
    
    /**handles changes to the BezierCurveDisplay component*/
    void changeListenerCallback (ChangeBroadcaster* source) override {        
        if(source == &mCurve && isVisible()){
            timerCallback();
        }
        
        repaint();};
    
    void timerCallback() override{
        if(isVisible()){
            if(mMode.getSelectedId() == 2)
                mProcessor.getAtomicTimeProcessor().processPitch();
            
            mDisplayComponent.setLines();
            repaint();
        }
    };
private:
    
    BezierCurveDisplay mCurve;
    
    Slider mTest;
    
    DTButton mBypass;
    const Image mBypassImage;
    DrawableImage mBypassDisplay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrollPitchDisplay)
};
