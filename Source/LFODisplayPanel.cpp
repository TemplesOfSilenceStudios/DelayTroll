/*
  ==============================================================================

    LFODisplayPanel.cpp
    Created: 24 Apr 2019 11:24:36am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "LFODisplayPanel.h"

//==============================================================================
LFODisplayPanel::LFODisplayPanel(DelayTrollAudioProcessor& p, int _index, int _width, int _height)
:   ModulationDisplayPanelBase(p, _index),
    mCurve(p, _index, MOD_INNER_DISPLAY_PANEL_WIDTH-24, MOD_INNER_DISPLAY_PANEL_HEIGHT-10)
{
    setSize(_width, _height);
    mCurve.setTopLeftPosition(DISPLAY_PANEL_HORIZONTAL_BUMPER+19, DISPLAY_PANEL_VERTICAL_BUMPER+7);
    addAndMakeVisible(&mCurve);
    //mCurve.setAlwaysOnTop(false);

    addAndMakeVisible(&mPolar);
    addChildComponent(&mBubble);
    mBubble.setAlwaysOnTop(true);
    
}

LFODisplayPanel::~LFODisplayPanel()
{
}


