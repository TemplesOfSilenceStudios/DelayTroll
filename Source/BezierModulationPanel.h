/*
  ==============================================================================

    BezierModulationPanel.h
    Created: 23 Apr 2019 11:30:34am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "ModulationPanelBase.h"
#include "BezierDisplayPanel.h"
#include "DTParamSlider.h"
#include "ModManagment.h"
#include "DTCheckBox.h"
#include "DTParamValueLabel.h"
#include "BPMSyncLabel.h"

//==============================================================================
/*Hosts a Bezier Display Panel and controlse the corispoinding Baizier Curve
*/
class BezierModulationPanel    : public ModulationPanelBase
{
public:
    BezierModulationPanel(DelayTrollAudioProcessor& p);
    ~BezierModulationPanel();
    
private:
    
    
    BezierPlotter* mBezierPlot;

    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BezierModulationPanel)
};
