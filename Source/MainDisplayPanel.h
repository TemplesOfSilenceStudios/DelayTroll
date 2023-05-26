/*
  ==============================================================================

    MainDisplayPanel.h
    Created: 23 Apr 2019 9:13:25am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "DisplayMenuPanel.h"
#include "TrollTimeDisplay.h"
#include "TrollLevelDisplay.h"
#include "TrollEQDisplay.h"
#include "TrollPitchDisplay.h"
#include "TrollTimeControlPanel.h"
#include "TrollPanDisplay.h"

//==============================================================================
/**Hosts panels that display the trolls and alow for interactive controle
*/
class MainDisplayPanel    : public PanelBase, public ChangeListener, public DelayTrollAudioProcessor::Listener
{
public:
    MainDisplayPanel(DelayTrollAudioProcessor& p);
    ~MainDisplayPanel();
    
    /**handles changes in the display, control and macro Panels*/
    void changeListenerCallback (ChangeBroadcaster* source) override;
    
    void loadActivated(DelayTrollAudioProcessor* _p) override;
    
    /**stores an adress to diferentiate between multiple incomeing change broadcasters*/
    void setAdress(ChangeBroadcaster* source){mAddress = source;};
    
    void setDisplayTimer(bool _shouldTimerRun, int _timerTime);
    


private:
    
    const Image mDisplayBGImage;
    const Image mDisplayGlImage;
    
    DrawableImage mDisplayBG;
    DrawableImage mDisplayGl;
    //DrawableImage mDisplayGl2;
    
    DisplayMenuPanel mMenu;
    ChangeBroadcaster* mAddress;
    NamedValueSet& mDisplayValues;
    
    OwnedArray<TrollDisplayPanelBase> mDisplayPanels;
    
    TrollTimeControlPanel mControlPanel;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainDisplayPanel)
};
