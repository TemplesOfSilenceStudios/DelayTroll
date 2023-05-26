/*
  ==============================================================================

    DisplayMenuPanel.h
    Created: 23 Apr 2019 9:14:58am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "DTTabbedButtonBar.h"
#include "DTLookAndFeel.h"

//==============================================================================
/**Hosts the menue for the display panels
*/
class DisplayMenuPanel    : public PanelBase
{
public:
    DisplayMenuPanel(DelayTrollAudioProcessor& p);
    ~DisplayMenuPanel();
    
    void addChangelistener(ChangeListener* inListener);
    void removeChangelistener(ChangeListener* inListener);
    

private:
    
    const Image mTabbedBarImage;
    const Image mTabbedBarGlImage;
    DTTabbedButtonBar mTabbedBar;
    DrawableImage mTabbedBarGl;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisplayMenuPanel)
};
