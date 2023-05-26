/*
  ==============================================================================

    ModulationPanelBase.cpp
    Created: 23 Apr 2019 11:29:46am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "ModulationPanelBase.h"
#include "DTModulationDefines.h"

//==============================================================================
ModulationPanelBase::ModulationPanelBase(DelayTrollAudioProcessor& p): PanelBase(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize(MODULATION_PANEL_WIDTH, MODULATION_PANEL_HEIGHT);
   
    mPlotNumber = mProcessor.getDisplayValues()[DTDisplayValueIDs[kDisVal_PlotEditingIndex]];
    
    mFreqControlPanel = std::make_unique<ModFreqControlPanel>(mProcessor,  *mProcessor.getModulationArray()[mPlotNumber]);
    mFreqControlPanel->setTopLeftPosition(0, 0);
    addAndMakeVisible(*mFreqControlPanel);
    mFreqControlPanel->setAlwaysOnTop(true);
 
    if(mPlotNumber < DTnumOfMods)
    {
        mDisplayPanel = std::make_unique<BezierDisplayPanel>(p, mPlotNumber, MOD_DISPLAY_PANEL_WIDTH, MOD_DISPLAY_PANEL_HEIGHT);
    }
    else{
        mDisplayPanel = std::make_unique<LFODisplayPanel>(p, mPlotNumber, MOD_DISPLAY_PANEL_WIDTH, MOD_DISPLAY_PANEL_HEIGHT);
    }
    
    mDisplayPanel->setTopLeftPosition(mFreqControlPanel->getRight(), 0);
    addAndMakeVisible(*mDisplayPanel);
    
    
}

ModulationPanelBase::~ModulationPanelBase()
{
}

void ModulationPanelBase::paint (Graphics& g)
{

}

