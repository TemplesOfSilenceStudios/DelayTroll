/*
  ==============================================================================

    EQTrollDisplay.h
    Created: 19 Apr 2019 5:29:23pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "TrollDisplayPanelBase.h"
#include "BezierCurveDisplay.h"
#include "DTCheckBox.h"
#include "DTParamCheckBox.h"
#include "DTButton.h"

//==============================================================================
/**repesents and manages interactive controls for the Trolls EQ parameters
*/
class TrollEQDisplay : public TrollDisplayPanelBase, public ChangeListener, public DelayTrollAudioProcessor::Listener
{
public:
    TrollEQDisplay(DelayTrollAudioProcessor& p, TrollTimeControlPanel& _ControlPanel);
    ~TrollEQDisplay();

    void paint (Graphics&) override;
    
    void loadActivated(DelayTrollAudioProcessor* _p) override;
    
    void setComponentVisability(int _controlMode) override;
    
    /**determine whitch plot the user wants to edit based on the mouses current location*/
    void mouseMove(const MouseEvent& e) override;
    
    /**adds a new node*/
    void mouseDown(const MouseEvent& e) override;
    
    /**changes the position of one of the plots nodes*/
    void mouseDrag(const MouseEvent& e) override;
    
    /**removes a node*/
    void mouseDoubleClick(const MouseEvent& e) override;
    
    /**makes sure the plot is set to the last position of the mouse*/
    void mouseUp(const MouseEvent& e) override;
    
    /**handles changes in the Bezier curve display components*/
    void changeListenerCallback (ChangeBroadcaster* source) override{ repaint();};
    
    void timerCallback() override;
    
private:
    
    NamedValueSet& displayValues;
    
    //required to draw a path without closing it, must be initialized
    PathStrokeType pathStroke;
    
    //keep track of what plot we are editing
    int mPlotEditingNumber;
    
    BezierCurveDisplay mLowPassCurve;
    BezierCurveDisplay mHighPassCurve;
    BezierCurveDisplay mBandPassCurve;
    
    AffineTransform mTranslate;
    
    DTButton mBypass;
    const Image mBypassImage;
    DrawableImage mBypassDisplay;
    
    
    enum EQTrolldisBezEditIndex{
        kBezIndex_BPPlot = 0,
        kBezIndex_LowPassPlot,
        kBezIndex_HighPassPlot,
    };
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrollEQDisplay)
};
