/*
  ==============================================================================

    MainPanel.h
    Created: 22 Apr 2019 4:46:54pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "PanelBase.h"
#include "RightPanel.h"
#include "LeftPanel.h"
#include "MainDisplayPanel.h"


/**Hosts the non modulation Section of the GUI*/
class MainPanel    : public PanelBase
{
public:
    MainPanel(DelayTrollAudioProcessor& p);
    ~MainPanel();
    
    void paint (Graphics&) override;
    void resized() override;
    
private:
    
    LeftPanel          mLeftPanel;
    MainDisplayPanel    mMainDisplayPanel;
    RightPanel            mRightPanel;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
