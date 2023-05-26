/*
  ==============================================================================

    BezierModulationPanel.cpp
    Created: 23 Apr 2019 11:30:34am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "BezierModulationPanel.h"
#include "DTModulationDefines.h"
#include "DTHelperFunctions.h"
#include "DTUsedParameters.h"

//==============================================================================
BezierModulationPanel::BezierModulationPanel(DelayTrollAudioProcessor& p)
: ModulationPanelBase(p),
mBezierPlot(static_cast<BezierPlotter*>(p.getModulationArray()[p.getDisplayValues()[DTDisplayValueIDs[kDisVal_bezierEditingIndex]]]))
{
    

}

BezierModulationPanel::~BezierModulationPanel()
{
}

