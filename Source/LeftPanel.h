/*
  ==============================================================================

    LeftPanel.h
    Created: 23 Apr 2019 9:11:46am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "DTParamSlider.h"
#include "DTParamValueLabel.h"
//==============================================================================
/**hosts "Macro" Parameters like the Number of delays and tape head speed
*/
class LeftPanel    : public PanelBase, public ChangeBroadcaster, public DTDSP::AtomicTimeProcessor::AtomicListener
{
public:
    LeftPanel(DelayTrollAudioProcessor& p);
    ~LeftPanel();

    void paint (Graphics&) override;
    

    
    void newHordSize(int _newSize) override{
        mNumOfDels.setValue(_newSize, dontSendNotification);
        mNumOfDels.repaint();
        mNumOfDelsLabel.repaint();
    };
    
    void atomicMultUpdated(DTDSP::AtomicTimeProcessor*)override{};

private:
    
    Font mFont;
    AttributedString mAttString;
    String  mSuffix;
    BubbleMessageComponent mBubble;
    
    const Image mNumOfDelsImage;
    const Image mFeedbackImage;
    DTParameterSlider mNumOfDels;
    DTParameterSlider mFeedback;

    DTParamValueLabel mFeedbackLabel, mNumOfDelsLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LeftPanel)
};
