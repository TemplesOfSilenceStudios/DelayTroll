/*
  ==============================================================================

    MainPanel.cpp
    Created: 22 Apr 2019 4:46:54pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "MainPanel.h"

MainPanel::MainPanel(DelayTrollAudioProcessor& p):
PanelBase(p),
mLeftPanel(p),
mMainDisplayPanel(p),
mRightPanel(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    setSize(MAIN_PANEL_WIDTH, MAIN_PANEL_HEIGHT);
    
    mLeftPanel.setTopLeftPosition(0, 0);
    addAndMakeVisible(&mLeftPanel);
    
    mMainDisplayPanel.setTopLeftPosition(mLeftPanel.getRight(), 0);
    addAndMakeVisible(&mMainDisplayPanel);
    mLeftPanel.addChangeListener(&mMainDisplayPanel);
    mMainDisplayPanel.setAdress(&mLeftPanel);
    
    
    mRightPanel.setTopLeftPosition(mMainDisplayPanel.getRight(), 0);
    addAndMakeVisible(&mRightPanel);
    
    
    
}

MainPanel::~MainPanel()
{
}

void MainPanel::paint (Graphics& g)
{
}

void MainPanel::resized()
{
}
