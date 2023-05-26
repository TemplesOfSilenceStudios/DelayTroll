/*
  ==============================================================================

    BezierCurveDisplay.h
    Created: 22 Apr 2019 10:45:01am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "BezierPlotter.h"


//==============================================================================
/**displays and controls a BezierPlotter
*/
class BezierCurveDisplay    : public Component, public Timer, public ChangeBroadcaster, public DelayTrollAudioProcessor::Listener
{
public:
    BezierCurveDisplay(DelayTrollAudioProcessor& p, int _plotEditingIndex, float _width, float _height);
    ~BezierCurveDisplay();

    void paint (Graphics&) override;
    void resized() override;
    
    /**throttle repaint when dragging*/
    void timerCallback() override;
    
    /**creates new node*/
    void mouseDown(const MouseEvent& e) override;
    /**changes a nodes location*/
    void mouseDrag(const MouseEvent& e) override;
    /**removes a node*/
    void mouseDoubleClick(const MouseEvent& e) override;
    /**ensures the curve is set to the mouses last position*/
    void mouseUp(const MouseEvent& e) override;
    
    
    
    //updates the display for a new plot to be shown
    //void updatePlot(int _numEditingIndex){plotEditingNumber = --_numEditingIndex; nodes = m_bezierPlot->getNodes(); repaint();};
    
    //simple functions to get set and clear nodes
    //void clearPlot(){m_bezierPlot->clearPlot(); nodes = m_bezierPlot->getNodes(); repaint();};
    //void setNodes(std::vector<Point<int>> _nodes){nodes = _nodes; m_bezierPlot->setNodes(_nodes); repaint();};
    
    /**sets the nodes for the display*/
    void setNodes(std::vector<Point<int>> _nodes, int _width, int _hight);
    
    /**returns the nodes of the display*/
    inline std::vector<Point<int>> getNodes(){return nodes;};
    /**sets the colour of the curve*/
    void setPathColour(Colour _inColour){mPathColour = _inColour;};
    /**returns a Path representing the Bezier Plot*/
    Path& getBezPath(){return mBezCurve;};
    /**manualy sets update paint to stop drawing when it is not nessisary*/
    void setUpdatePaint(bool _inBool){ updatePaint = _inBool;};
    /**retruns the currently dragging node number*/
    int getDraggingNodeNumber(){return draggingNodeNum;};
    
    BezierPlotter* getPlotter(){return m_bezierPlot;}
    
    void loadActivated(DelayTrollAudioProcessor* p) override;
    
    void resetNodes();
    
    void setHasPolarValue(bool _hasPolarValue){mHasPolarValue = _hasPolarValue;};
    
private:
    
    Path mBezCurve;
    
    Colour mPathColour;
    
    //access to the processpr
    DelayTrollAudioProcessor& processor;
    
    //required to draw a path without closing it, must be initialized
    PathStrokeType pathStroke;
    
    //access to the plots themselves, pointers get cleaned up by the manager
    BezierPlotter* m_bezierPlot;
    
    //used in conjunction with the timerCallback method to throttle repaint while dragging
    bool updatePaint = false;
    
    bool mHasPolarValue = false;
    
    //controle points stored in a vector
    std::vector<Point<int>> nodes;
    
    //the current node that is being dragged, set to -1 if no nodes are being draged
    int draggingNodeNum = -1;
    
    AffineTransform mScaleDown;
    AffineTransform mScaleUp;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BezierCurveDisplay)
};
