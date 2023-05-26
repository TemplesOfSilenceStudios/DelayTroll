/*
  ==============================================================================

    BezierCurveDisplay.cpp
    Created: 22 Apr 2019 10:45:01am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BezierCurveDisplay.h"
#include "DTModulationDefines.h"
#include "DTInterfaceDefines.h"
//==============================================================================
BezierCurveDisplay::BezierCurveDisplay(DelayTrollAudioProcessor& p, int _plotEditingIndex, float _width, float _height)
:   processor(p),
    pathStroke(6.f, PathStrokeType::curved, PathStrokeType::butt)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize(static_cast<int>(_width), static_cast<int>(_height));
    setPathColour(DTColourIds::LrgDisplayGlow);
    startTimer(40);
    m_bezierPlot = dynamic_cast<BezierPlotter*>(processor.getModulationArray()[_plotEditingIndex]);
    
    mScaleDown = AffineTransform::scale(1.0f/_width, 1.0f/_height);
    mScaleUp = AffineTransform::scale(_width, _height);
    
    std::vector<Point<float>> tempnodes = m_bezierPlot->getNodes();
    
    for(std::vector<Point<float>>::iterator it = tempnodes.begin(); it != tempnodes.end(); it++ ){
        Point<float> tempPoint(it->getX(), it->getY());
        tempPoint.applyTransform(mScaleUp);
        nodes.push_back(tempPoint.toInt());
    }
    processor.addListener(this);
}

BezierCurveDisplay::~BezierCurveDisplay()
{
    stopTimer();
    processor.removeListener(this);
}

void BezierCurveDisplay::setNodes(std::vector<Point<int>> _nodes, int _width, int _hight)
{
    //sets the nodes when the incoming size may be different than the current size of the display
    //make sure the plot is big enough
    //m_bezierPlot->setSize(_width, _hight);
    //put in the nodes
    std::vector<Point<float>> tempNodes;
    AffineTransform transformer;
    transformer.scale(1.0f/static_cast<float>(getWidth()), 1.0f/static_cast<float>(getHeight()));
    
    for(int i = 0 ; i < _nodes.size(); i++){
        tempNodes.push_back(_nodes[i].toFloat());
        tempNodes[i].applyTransform(transformer);
    }
    
    m_bezierPlot->setNodes(tempNodes);
    //scale the new plot to the current size
    repaint();
}

void BezierCurveDisplay::mouseDown(const MouseEvent& e)
{
    //inserts a new node
    
    //int to keep track of the node number being looked at
    int n = 0;
    
    //iterate through the nodes
    for(std::vector<Point<int>>::iterator it = nodes.begin(); it != nodes.end(); it++ )
    {
        //check to see if the node is in dragging distance
        if(e.getPosition().getDistanceFrom(*it) < 10 || e.getPosition().x == getWidth() || e.getPosition().x == 0)
        {
            //set the draggingNodeNum and exit the method
            draggingNodeNum = n;
            return;
        }
        //check to see if the position of the mouse is greater than the x positon of the node being looked at
        if(e.getPosition().x > it->x)
        {
            //add to the value of n to determin where the new node is to be inserted and assign it to the draggingNodeNum
            draggingNodeNum = ++n;
        }

    }
    
    Point<int> newPoint(e.getPosition());
    
    //create an iterator and set it to the begining of the nodes vector
    std::vector<Point<int>>::iterator inserterIt;
    inserterIt = nodes.begin();
    
    //insert a new vector and path at position n
    nodes.insert(inserterIt+n, e.getPosition());
    
    Point<float> tempPoint = newPoint.toFloat();
    tempPoint.applyTransform(mScaleDown);
    
    m_bezierPlot->newNode(tempPoint, n);
    
    //repaint the graph
    updatePaint = true;
    repaint();
}
void BezierCurveDisplay::mouseDrag(const MouseEvent& e)
{
    
    //make a point
    Point<int> p1(e.getPosition());
    
    //check to make sure the point does not go to the left or right of it's neighbouring nodes or go outside the graph
    if(draggingNodeNum == 0)
    {
        p1.x = 0;
    }
    else if(draggingNodeNum == nodes.size() - 1)
    {
        p1.x = getWidth();
    }
    else if(p1.x > nodes[draggingNodeNum+1].x-5)
    {
        p1.x = nodes[draggingNodeNum+1].x - 5;
    }
    else if(p1.x < nodes[draggingNodeNum-1].x+5)
    {
        p1.x = nodes[draggingNodeNum-1].x + 5;
    }
    
    
    if(p1.y >= getHeight())
    {
        p1.y = getHeight();
    }
    else if(p1.y <= 0)
    {
        p1.y = 0;
    }
    
    //assing the node being dragged to the mouse position
    nodes[draggingNodeNum] = p1;
    
    Point<float> tempPoint = p1.toFloat();
    tempPoint.applyTransform(mScaleDown);
    
    m_bezierPlot->setNode(tempPoint, draggingNodeNum);
    
    //repaint the graph
    updatePaint = true;
}
void BezierCurveDisplay::mouseDoubleClick(const MouseEvent& e)
{
    //erase a node
    //check to see if the the node is not the first or last node and the total number of nodes at least 4
    if(nodes.size() <= 4 || draggingNodeNum == 0 || draggingNodeNum == (nodes.size() - 1))
    {
        return;
    }
    else if(nodes.size() > 4 && draggingNodeNum != 0 && draggingNodeNum != nodes.size() - 1)
    {
        //erase the node, path, slider and listener
        nodes.erase(nodes.begin() + draggingNodeNum);
        
        m_bezierPlot->removeNode(draggingNodeNum);
        
        //repaint the graph
        updatePaint = true;
        repaint();
        
    }
}
void BezierCurveDisplay::mouseUp(const MouseEvent& e)
{
    //once the user has finished editing the array can be saved and the graph no longer needs to be repainted
    if(updatePaint){
        m_bezierPlot->multipointBezierCurve(draggingNodeNum, false);
        m_bezierPlot->measureCurve(draggingNodeNum);
        sendChangeMessage();
    }
    updatePaint = false;
    repaint();
    //for testing will remove
    //processor.setMultiValue();
}
void BezierCurveDisplay::timerCallback()
{
    //throttling the repaint function
    if(updatePaint){
        m_bezierPlot->multipointBezierCurve(draggingNodeNum, false);
        m_bezierPlot->measureCurve(draggingNodeNum);
        repaint();
        sendChangeMessage();
    }
    
}
void BezierCurveDisplay::paint (Graphics& g)
{
    //drawing dashed lines coneting the nodes
    //==============================================================================
    
    //an array that defines how many pixles each line segment and break will be
    float dashArray[4]{4,4,4,4};
    g.setColour (Colours::black);
    float height = getLocalBounds().toFloat().getHeight() * (1.f-m_bezierPlot->getPolarValue());
    Line<float> pol(0.f, height, getLocalBounds().toFloat().getWidth(), height);
    
    if(mHasPolarValue)
        g.drawDashedLine(pol, dashArray, 4, 1.f,0);
    
    g.setColour (Colours::grey);
    
    //iterate though the nodes
    for(std::vector<Point<int>>::iterator it = nodes.begin(); it != nodes.end()-1; it++ )
    {
        std::vector<Point<int>>::iterator nextIt = it+1;
        //make a line between the current and next node
        Line<float> dashedLine(float(it->x), float(it->y), float(nextIt->x), float(nextIt->y));
        //draw the line
        g.drawDashedLine(dashedLine, dashArray, 4, 1.f, 0);
    }
    
    //draw the stroked path
    //==============================================================================
    mBezCurve.clear();
    for(int i = 1; i < m_bezierPlot->getQuadPaths().size(); i++)
    {
        //get the quadradic path and scale it
        Path tempPath = *m_bezierPlot->getQuadPaths()[i];
        tempPath.applyTransform(mScaleUp);
        mBezCurve.addPath(tempPath);
    }
    g.setColour(mPathColour);
    pathStroke.setStrokeThickness(8.f);
    g.setOpacity(0.4f);
    g.strokePath(mBezCurve, pathStroke);
    pathStroke.setStrokeThickness(5.f);
    g.setOpacity(1.f);
    
    g.strokePath(mBezCurve, pathStroke);
    
    //draw some Ellipses to represent the nodes
    //==============================================================================
    for(std::vector<Point<int>>::iterator it = nodes.begin(); it != nodes.end(); it++ )
    {
        //g.setColour (Colours::blue.interpolatedWith(Colours::red.interpolatedWith(Colours::green, float(it->y)/float(getHeight())), float(it->x)/float(getWidth())));
        g.setColour(DTColourIds::NodePoints);
        g.fillEllipse(it->getX() - 2, it->getY() - 2, 4, 4);
        g.drawEllipse(it->getX()-5.f, it->getY() - 5.f, 10.f, 10.f, 1.f);
    }
}

void BezierCurveDisplay::resized()
{
    //allways seems to get called before paint...which makes sense i think


}

void BezierCurveDisplay::loadActivated(DelayTrollAudioProcessor *p)
{
    if(p == &processor){
        resetNodes();
    }
}

void BezierCurveDisplay::resetNodes()
{
    nodes.clear();
    std::vector<Point<float>> tempnodes = m_bezierPlot->getNodes();
    
    for(std::vector<Point<float>>::iterator it = tempnodes.begin(); it != tempnodes.end(); it++ ){
        Point<float> tempPoint(it->getX(), it->getY());
        tempPoint.applyTransform(mScaleUp);
        nodes.push_back(tempPoint.toInt());
    }
    repaint();
    sendChangeMessage();
}
