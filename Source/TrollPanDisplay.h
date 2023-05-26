/*
  ==============================================================================

    TrollPanDisplay.h
    Created: 4 Aug 2020 2:54:18pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TrollDisplayPanelBase.h"
#include "BezierCurveDisplay.h"

//==============================================================================
/*
*/
class TrollPanDisplay  : public TrollDisplayPanelBase, public ChangeListener, public DelayTrollAudioProcessor::Listener
{
public:
    TrollPanDisplay(DelayTrollAudioProcessor &p, TrollTimeControlPanel& _ControlPanel);
    ~TrollPanDisplay() override;
    
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
    
    void timerCallback() override;
    
    void changeListenerCallback (ChangeBroadcaster* source) override {
//        if(dynamic_cast<DTManualControlDisplay*>(source) == &mManualControlDisplay && isVisible()){
//            timerCallback();
//        }
    
    repaint();};
    
    /**draws the underlying grid for this display*/
    void paint (juce::Graphics&) override;
    
    void loadActivated(DelayTrollAudioProcessor* _p) override;
    
    void setComponentVisability(int _controlMode) override;

private:
    
    BezierCurveDisplay mLeftCurve;
    BezierCurveDisplay mRightCurve;
    BezierCurveDisplay mBalCurve;
    
    enum PanTrolldisBezEditIndex{
        kBezIndex_LeftPlot = 0,
        kBezIndex_RightPlot,
        kBezIndex_BalPlot,
    };
    
    AffineTransform mTranslateLeft;
    AffineTransform mTranslateRight;
    
    int mPlotEditingNumber;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrollPanDisplay)
};
