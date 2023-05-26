/*
  ==============================================================================

    LFOModulationPanel.h
    Created: 23 Apr 2019 11:32:14am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "ModulationPanelBase.h"
#include "LFODisplayPanel.h"
#include "LFOWaveManagementPanel.h"
//==============================================================================
/**hosts the modulation display panels and managment panels
*/
class LFOModulationPanel    : public ModulationPanelBase, public ChangeListener
{
public:
    LFOModulationPanel(DelayTrollAudioProcessor& p);
    ~LFOModulationPanel();
    
    void paint (Graphics&) override;
    
    void changeListenerCallback(ChangeBroadcaster* source) override;
    

private:
    
    //OwnedArray<LFODisplayPanel> mDisplayPanels;
    OwnedArray<LFOWaveManagementPanel> mManagmentPanels;
    Viewport mViewPort;
    
    

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOModulationPanel)
};
