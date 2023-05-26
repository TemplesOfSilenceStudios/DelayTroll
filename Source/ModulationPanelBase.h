/*
  ==============================================================================

    ModulationPanelBase.h
    Created: 23 Apr 2019 11:29:46am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "ModFreqControlPanel.h"
#include "LFODisplayPanel.h"
#include "BezierDisplayPanel.h"
//==============================================================================
/**base class wrapper for our modulation Panels
*/
class ModulationPanelBase    : public PanelBase
{
public:
    ModulationPanelBase(DelayTrollAudioProcessor& p);
    ~ModulationPanelBase();

    void paint (Graphics&) override;
    

protected:
    
    int mPlotNumber;
    
    std::unique_ptr<ModFreqControlPanel> mFreqControlPanel;
    std::unique_ptr<ModulationDisplayPanelBase> mDisplayPanel;
    
private:
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulationPanelBase)
};
