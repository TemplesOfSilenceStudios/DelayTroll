/*
  ==============================================================================

    BezierDisplayPanel.h
    Created: 24 Apr 2019 11:24:21am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ModulationDisplayPanelBase.h"
#include "BezierCurveDisplay.h"
#include "DTButton.h"
//==============================================================================
/**displays the background for a BezierCurveDisplay Component 
*/
class BezierDisplayPanel    : public ModulationDisplayPanelBase
{
public:
    BezierDisplayPanel(DelayTrollAudioProcessor& p, int _index, int _width, int _height);
    ~BezierDisplayPanel();

    void mouseDown(const MouseEvent& _e) override;
    void mouseDrag(const MouseEvent& _e) override;

private:
    
    BezierCurveDisplay mCurve;
    std::vector<Point<int>> mDefaultNotes;
    const Image mResetImageOn, mResetImageOff;
    std::unique_ptr<DTButton> mReset;
    const Image mResetLableImage;
    DrawableImage mResetLable;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BezierDisplayPanel)
};
