/*
  ==============================================================================

    LFOModulationPanel.cpp
    Created: 23 Apr 2019 11:32:14am
    Author:  Sam Ferguson

  ==============================================================================
*/


#include "LFOModulationPanel.h"
#include "DTModulationDefines.h"
#include "DTHelperFunctions.h"
//==============================================================================
LFOModulationPanel::LFOModulationPanel(DelayTrollAudioProcessor& p)
:   ModulationPanelBase(p)
{
    
    Rectangle<int> viewportBounds(mFreqControlPanel->getX(), mFreqControlPanel->getBottom(), mFreqControlPanel->getWidth(), getHeight() - mFreqControlPanel->getHeight());
    
    mViewPort.setBounds(viewportBounds);
    mViewPort.setViewedComponent(new LFOWaveManagementPanel(p,
                                                            mPlotNumber,
                                                            mViewPort.getMaximumVisibleWidth()-mViewPort.getScrollBarThickness(),
                                                            getHeight(),
                                                            this));
    
    mViewPort.setScrollBarsShown(true, false);
    
    addAndMakeVisible(&mViewPort);
    
}

LFOModulationPanel::~LFOModulationPanel()
{
}

void LFOModulationPanel::paint (Graphics& g)
{
//g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
}



void LFOModulationPanel::changeListenerCallback(juce::ChangeBroadcaster *source)
{

    mDisplayPanel->repaint();
}
