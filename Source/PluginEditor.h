/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "TopPanel.h"
#include "MainPanel.h"
#include "MainModulationPanel.h"
#include "DTLookAndFeel.h"
#include "DTUnlockStatus.h"



//==============================================================================
/**JUCE's editor panel
*/
class DelayTrollAudioProcessorEditor  : public AudioProcessorEditor, public TextButton::Listener
{
public:
    DelayTrollAudioProcessorEditor (DelayTrollAudioProcessor&);
    ~DelayTrollAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void buttonClicked(Button* b) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayTrollAudioProcessor& processor;
    
    std::unique_ptr<DTLookAndFeel> mLookAndFeel;
    
    TopPanel mTopPanel;
    MainPanel mMainPanel;
    MainModulationPanel mMainModPanel;
    
    Image mBackgroundImage;
    
    DrawableImage mBackgroundDrawable;
    
    Rectangle<float> mTopBounds;
    
    bool mPainted;
    
    DTUnlockStatus mUnlockStatus;
    DTUnlockForm mUnlockForm;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayTrollAudioProcessorEditor)
};
