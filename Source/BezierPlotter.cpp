/*
  ==============================================================================

    BezierPlotter.cpp
    Created: 9 Mar 2019 8:25:54am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "BezierPlotter.h"

//==============================================================================
BezierPlotter::BezierPlotter(double* const& Array, float _arraySize, float _polar, int _managmentID, Value& _BPM, Value& _timeSigNumerator) : ModManagment(Array, _arraySize, _managmentID, _BPM, _timeSigNumerator), hight(1.0f), width(1.0f)
{
    //initialize the array that the processor will refer too default to a 100 by 100 size graph
    m_quadPaths.ensureStorageAllocated(100);
    clearPlot();
    mPolar.setValue(_polar);
}
BezierPlotter::~BezierPlotter()
{
    
}
void BezierPlotter::clearPlot()
{
    //clears the plot and resets to default
    nodes.clear();
    m_quadPaths.clear(true);

    nodes.push_back(Point<float>(0, 1.0f));
    nodes.push_back(Point<float>(0.2f, 0.8f));
    nodes.push_back(Point<float>(0.5f, 0.2f));
    nodes.push_back(Point<float>(0.8f, 0.8f));
    nodes.push_back(Point<float>(1.0f, 0));
    
    for(int i = 0; i < nodes.size() - 1; i++)
    {
        m_quadPaths.add(new Path());
    }
    multipointBezierCurve(-1, true);
    measureCurve(2);
    
}

void BezierPlotter::newNode(Point<float> newNode, int nodeNumb)
{
    //inserts a new node

    //create an iterator and set it to the begining of the nodes vector
    std::vector<Point<float>>::iterator inserterIt;
    inserterIt = nodes.begin();
    
    //insert a new vector and path at position n
    nodes.insert(inserterIt+nodeNumb, newNode);
    
    m_quadPaths.insert(nodeNumb, new Path());
    
    multipointBezierCurve(nodeNumb, false);
    measureCurve(nodeNumb);
}
void BezierPlotter::removeNode(int nodeNumb)
{
    //erase a node
    
    //erase the node, path, slider and listener
    nodes.erase(nodes.begin() + nodeNumb);
    m_quadPaths.remove(nodeNumb);
    
    multipointBezierCurve(nodeNumb, false);
    measureCurve(nodeNumb);
}
void BezierPlotter::setNodes(std::vector<Point<float>> newNodes)
{
    //reset all the nodes with new ones
    jassert(newNodes.size() >=4);
    nodes = newNodes;
    m_quadPaths.clear(true);
    for(int i = 0; i < nodes.size() - 1; i++)
    {
        m_quadPaths.add(new Path());
    }
    multipointBezierCurve(-1, true);
    measureCurve();
    
    
}
void BezierPlotter::measureCurve()
{
    //this makes each stroked path into a shape that goes to the top of the line
    //so the bottom of the shape is our quatpath then it draws a line to the top of the display on both sides
    //then closes the shap at the top, 0, and adds it to our path kachunk kachunk
    //not sure how it interperts when the bounds are not a full int but a x.5 but testing doesn't seem to reveal any anomolies
    Path path;
    for(int i = 1; i < m_quadPaths.size(); i++)
    {
        Path pathCopy(*m_quadPaths[i]);
            
        pathCopy.lineTo(m_quadPaths[i]->getBounds().getRight(), 0);
        pathCopy.lineTo(m_quadPaths[i]->getBounds().getX(), 0);
        pathCopy.closeSubPath();
        
        path.addPath(pathCopy);
    }
    
    int start = 0;
    int end = static_cast<int>(m_arraySize);
    Line<float> line;
    double multi;
    //scale up for acuracy
    AffineTransform mScaleUP = AffineTransform::scale(1000.f, 1000.f);
    AffineTransform mScaleDown = AffineTransform::scale(1.f/1000.f, 1.f/1000.f);
    
    for(int i = start; i < end; i++)
    {
        //get arraySixe xvalues
        //float x = width*(float(i)/m_arraySize);
        float x = width*(float(i)/mArratIndexEnd);
        
        //x cannot equel the first or last position
        if (x <= 0){x = 0.001f;}
        
        if (x >= width){ x = width-0.001f;}
        
        //draw a line to the top
        line.setStart(x, hight);
        line.setEnd(x, 0.0001f);
        line.applyTransform(mScaleUP);
        path.applyTransform(mScaleUP);
        //clip the line when it intersects with the closed path
        line = path.getClippedLine(line, true);
        
        line.applyTransform(mScaleDown);
        path.applyTransform(mScaleDown);
        //the end of the clipped line is the y positon
        multi = line.getEndY();
        
        
        multi = double(hight - multi)/double(hight);
        
        if(hight == line.getEndY())
        {
            multi = 0;
        }
        jassert(multi <= 1.0);
        jassert(multi >= 0.0);
        //set the values calculated to the array
        m_plotArray[i] = multi;
    }
    //DBG("measure all :"<<m_plotArray[50]);
 
    
}
void BezierPlotter::measureCurve(int draggingNodeNum)
{
    //measuring the Bezier curve
    //==============================================================================
    
    
    //it's not allways nessisary measure the whole curve, just the part that is being manipulated by the user so different start and end's can be set
    int start = 0;
    int end = static_cast<int>(mArratIndexEnd);
    
    //generaly the path infront and behind the control node being edited will need to be measured, which means 7 control points in total
    
    double yZero{0}, yOne{0}, yTwo{0}, yThree{0}, yFour{0}, yFive{0}, ySix{0};
    double xZero{0}, xOne{0}, xTwo{0}, xThree{0}, xFour{0}, xFive{0}, xSix{0};
    
    
    //several different senarios will effect how each control node is defined
    if(draggingNodeNum > 2 && draggingNodeNum < nodes.size() - 3 && nodes.size() >= 6)
    {
        //compleatly in the middle the first and last path need not be measured
        start = int(((m_quadPaths[draggingNodeNum - 1]->getBounds().getX()) / width)* mArratIndexEnd);
        
        end = int(((m_quadPaths[draggingNodeNum + 1]->getBounds().getRight()) / width)* mArratIndexEnd);
        
        yZero = double(nodes[draggingNodeNum - 2].y+nodes[draggingNodeNum-1].y)/2.f;
        yOne = double(nodes[draggingNodeNum-1].y);
        yTwo = double(nodes[draggingNodeNum - 1].y + nodes[draggingNodeNum].y)/2.f;
        yThree = double(nodes[draggingNodeNum].y);
        yFour = double(nodes[draggingNodeNum + 1].y + nodes[draggingNodeNum].y)/2.f;
        yFive = double(nodes[draggingNodeNum + 1].y);
        ySix = double(nodes[draggingNodeNum + 1].y + nodes[draggingNodeNum + 2].y)/2.f;
        
        xZero = ((nodes[draggingNodeNum - 2].x/width) * mArratIndexEnd + (nodes[draggingNodeNum - 1].x/width) * mArratIndexEnd)/2.f;
        xOne = (nodes[draggingNodeNum - 1].x/width) * mArratIndexEnd;
        xTwo = ((nodes[draggingNodeNum - 1].x/width) * mArratIndexEnd + (nodes[draggingNodeNum].x/width) * mArratIndexEnd)/2.f;
        xThree = (nodes[draggingNodeNum].x/width) * mArratIndexEnd;
        xFour = ((nodes[draggingNodeNum].x/width) * mArratIndexEnd + (nodes[draggingNodeNum + 1].x/width) * mArratIndexEnd)/2.f;
        xFive = (nodes[draggingNodeNum+1].x/width) * mArratIndexEnd;
        xSix = ((nodes[draggingNodeNum+1].x/width) * mArratIndexEnd + (nodes[draggingNodeNum + 2].x/width) * mArratIndexEnd)/2.f;
        
    }
    else if(draggingNodeNum == 2 && nodes.size() == 5)
    {
        //the first and last path both need to be calculated
        start = 0;
        end = static_cast<int>(mArratIndexEnd);
        
        yZero = double(nodes[0].y);
        yOne = double(nodes[1].y);
        yTwo = double(nodes[1].y + nodes[2].y)/2.f;
        yThree = double(nodes[2].y);
        yFour = double(nodes[2].y + nodes[3].y)/2.f;
        yFive = double(nodes[3].y);
        ySix = double(nodes[4].y);
        
        xZero = 0.f;
        xOne = (nodes[1].x/width) * mArratIndexEnd;
        xTwo = ((nodes[1].x/width) * mArratIndexEnd + (nodes[2].x/width) * mArratIndexEnd)/2.f;
        xThree = (nodes[2].x/width) * mArratIndexEnd;
        xFour = ((nodes[2].x/width) * mArratIndexEnd + (nodes[3].x/width) * mArratIndexEnd)/2.f;
        xFive = (nodes[3].x/width) * mArratIndexEnd;
        xSix = mArratIndexEnd;
        
    }
    else if(nodes.size() == 4)
    {
        //only 2 paths are being manipulated so only 5 node positions need to be set
        start = 0;
        end = static_cast<int>(mArratIndexEnd);
        
        yZero = double(nodes[0].y);
        yOne = double(nodes[1].y);
        yTwo = double(nodes[1].y + nodes[2].y)/2.f;
        yThree = double(nodes[2].y);
        yFour = double(nodes[3].y);
        
        xZero = 0.f;
        xOne = (nodes[1].x/width) * mArratIndexEnd;
        xTwo = ((nodes[1].x/width) * mArratIndexEnd + (nodes[2].x/width) * mArratIndexEnd)/2.f;
        xThree = (nodes[2].x/width) * mArratIndexEnd;
        xFour = mArratIndexEnd;
        
    }
    else if(draggingNodeNum <= 2 && draggingNodeNum < nodes.size() - 3)
    {
        //the first path needs to be calculated
        start = 0;
        end = int(((m_quadPaths[draggingNodeNum + 1]->getBounds().getRight()) / width)* mArratIndexEnd);
        
        yZero = double(nodes[0].y);
        yOne = double(nodes[1].y);
        yTwo = double(nodes[1].y+nodes[2].y)/2.f;
        yThree = double(nodes[2].y);
        yFour = double(nodes[2].y+nodes[3].y)/2.f;
        yFive = double(nodes[3].y);
        ySix = double(nodes[3].y+nodes[4].y)/2.f;
        
        xZero = 0.f;
        xOne = (nodes[1].x/width) * mArratIndexEnd;
        xTwo = ((nodes[1].x/width) * mArratIndexEnd + (nodes[2].x/width) * mArratIndexEnd)/2.f;
        xThree = (nodes[2].x/width) * mArratIndexEnd;
        xFour = ((nodes[2].x/width) * mArratIndexEnd + (nodes[3].x/width) * mArratIndexEnd)/2.f;
        xFive = (nodes[3].x/width) * mArratIndexEnd;
        xSix = ((nodes[3].x/width) * mArratIndexEnd + (nodes[4].x/width) * mArratIndexEnd)/2.f;
        
    }
    else if(draggingNodeNum > 2 && draggingNodeNum >= nodes.size() - 3)
    {
        //the last path needs to be calculated
        start = int(((m_quadPaths[draggingNodeNum - 1]->getBounds().getX()) / width)* mArratIndexEnd);
        end = static_cast<int>(mArratIndexEnd);
        
        yZero = double(nodes[nodes.size() - 5].y+nodes[nodes.size() - 4].y)/2.f;
        yOne = double(nodes[nodes.size() - 4].y);
        yTwo = double(nodes[nodes.size() - 4].y+nodes[nodes.size() - 3].y)/2.f;
        yThree = double(nodes[nodes.size() - 3].y);
        yFour = double(nodes[nodes.size() - 3].y+nodes[nodes.size() - 2].y)/2.f;
        yFive = double(nodes[nodes.size() - 2].y);
        ySix = double(nodes[nodes.size() - 1].y);
        
        
        xZero = ((nodes[nodes.size() - 5].x/width) * mArratIndexEnd + (nodes[nodes.size() - 4].x/width) * mArratIndexEnd)/2.f;
        xOne = (nodes[nodes.size() - 4].x/width) * mArratIndexEnd;
        xTwo = ((nodes[nodes.size() - 4].x/width) * mArratIndexEnd + (nodes[nodes.size() - 3].x/width) * mArratIndexEnd)/2.f;
        xThree = (nodes[nodes.size() - 3].x/width) * mArratIndexEnd;
        xFour = ((nodes[nodes.size() - 3].x/width) * mArratIndexEnd + (nodes[nodes.size() - 2].x/width) * mArratIndexEnd)/2.f;
        xFive = (nodes[nodes.size() - 2].x/width) * mArratIndexEnd;
        xSix = mArratIndexEnd;
    }
    else{
        DBG("why");
    }
    
    //in the equation we use the middle control point cannot be exactly half way between the surrounding control points, if this is the case the control point is nugged
    if(xOne == (xZero + xTwo)/2.f)
    {
        xOne = xOne + 0.001;
    }
    
    if(xThree == (xTwo + xFour)/2.f)
    {
        xThree = xThree + 0.001;
    }
    
    if(xFive == (xFour + xSix)/2.f)
    {
        xFive = xFive + 0.001;
    }
    
    //some variables we will need
    double multi = 0.0;
    double k = 0.0;
    double n = 0.0;

    double sqrChecker = 0.0;
    //loop from the begining to the end of the paths being manipulated
    for(int i = start; i <= end; i++)
    {
        
        //if the user is dragging just measure the parts of the line that are being manipulated
            /*
             in order to avoid creating an array to measure both x and y
             and then using liniar interpolation to determin the position of y at any particular x
             k(t in tradition notation of Bezier curves) must be determined at position x
             
             n is the current positon of i reletive to the start and end, this is the x positon
             
             n is used to determin k
             
             k is then put into the traditional Bezier curve for y positon only
             */
            if(i < xTwo)
            {
                
                n=(xTwo-xZero)*(float(float(i)-xZero)/(xTwo-xZero))+xZero;
                sqrChecker = xZero*n+xTwo*n-2.f*xOne*n+pow(xOne, 2.f)-xZero*xTwo;
                
                if(sqrChecker < 0.0){
                    sqrChecker = 0.0;
                }
                
                k = (xZero+sqrt(sqrChecker)-xOne)/(xZero-2*xOne+xTwo);
                
                //DBG(k);
                if(k < 0.0){
                    k = 0.0;
                }
                
                multi = (pow((1.f-k), 2.f)*yZero) + 2.f * (1.f-k)*k*yOne + (pow(k, 2.f)*yTwo);
                
                //jassert(multi <= 1.0);
                //jassert(multi >= 0.0);
                
            }
            else if (i >= xTwo && i < xFour)
            {
                n=(xFour-xTwo)*(float(i-xTwo)/(xFour-xTwo))+xTwo;
                
                k = (xTwo+sqrt(xTwo*n+xFour*n-2.f*xThree*n+pow(xThree, 2.f)-xTwo*xFour)-xThree)/(xTwo-2*xThree+xFour);
                multi = (pow((1.f-k), 2.f)*yTwo) + 2.f * (1.f-k)*k*yThree + (pow(k, 2.f)*yFour);
                //jassert(multi <= 1.0);
                //jassert(multi >= 0.0);
            }
            else
            {
                
                n=(xSix-xFour)*(float(i-xFour)/(xSix-xFour))+xFour;
                
                k = (xFour+sqrt(xFour*n+xSix*n-2.f*xFive*n+pow(xFive, 2.f)-xFour*xSix)-xFive)/(xFour-2*xFive+xSix);
                
                if(k < 0.0){
                    k = 0.0;
                }
                
                multi = (pow((1.f-k), 2.f)*yFour) + 2.f * (1.f-k)*k*yFive + (pow(k, 2.f)*ySix);

                
                //jassert(multi <= 1.0);
                //jassert(multi >= 0.0);
            }
        
        //extract the y value
        //note this will be upsidown since the pixles count up from the top of the graph down
        //replace this with j limit because optomizer will not allow such shinanigans*******************************
        multi = static_cast<double>(static_cast<float>(multi));
        multi = double(hight - multi)/double(hight);
        
        
        //set the values calculated to the array
        jassert(multi <= 1.0);
        jassert(multi >= 0.0);
        m_plotArray[i] = multi;
    }
    m_plotArray[99] = double(hight - nodes[nodes.size() - 1].getY())/double(hight);
    //DBG("measure some :"<<m_plotArray[50]);
}
void BezierPlotter::multipointBezierCurve(int draggingNodeNum, bool update)
{
    //This method updates the Bezeir curve for efficiency purposes only the curves that are changing are calculated
    
    //the start and end of most curves is the midpoint between 2 controle nodes
    float midx, midy;
    
    //this represents the path being calculated
    int i=0;
    
    //the index at which the paths are no longer needed to be calculated
    int end=0;
    
    //true if the final path must be calculated
    bool runLastNode;
    
    //there are several situations in which the calculation requires different starting conditions
    if((nodes.size() == 6 && (draggingNodeNum == 3 || draggingNodeNum == 2 ))|| (nodes.size() == 7 && draggingNodeNum == 3) || (nodes.size() == 5 || nodes.size() == 4) || update)
    {
        //this first one is for when to entire curve needs to be calcuated
        i = 1;
        midy = nodes[0].y;
        midx = nodes[0].x;
        end = int(m_quadPaths.size()) - 2;
        runLastNode = true;
        
    }
    else if(draggingNodeNum > 2 && draggingNodeNum < int(nodes.size()) - 3)
    {
        //if neither the first or last three paths need to be calculated
        i = draggingNodeNum - 1;
        midx = (nodes[i-1].x + nodes[i].x)/2.f;
        midy = (nodes[i-1].y + nodes[i].y)/2.f;
        end = draggingNodeNum + 1;
        runLastNode = false;
        
    }
    else if(draggingNodeNum <= 2)
    {
        //if any of the first three nodes need to be calculated
        i = 1;
        midx = nodes[0].x;
        midy = nodes[0].y;
        end = draggingNodeNum + 1;
        runLastNode = false;
    }
    else if(draggingNodeNum >= int(nodes.size()) - 3)
    {
        //if any of the last three nodes need to be calculated
        i = draggingNodeNum - 1;
        midx = (nodes[i-1].x + nodes[i].x)/2.f;
        midy = (nodes[i-1].y + nodes[i].y)/2.f;
        end = int(m_quadPaths.size()) - 2;
        runLastNode = true;
    }
    else
    {
        //this is just for safty, should never run
        i = 1;
        midy = nodes[0].y;
        midx = nodes[0].x;
        end = int(m_quadPaths.size()) - 2;
        runLastNode = true;
    }
    
    
    //itterate through the paths and perform the quadic calculation
    for(; i <= end; i++)
    {
        m_quadPaths[i]->clear();
        m_quadPaths[i]->startNewSubPath(midx, midy);
        
        midx = float(nodes[i].x + nodes[i+1].x)/2.f;
        midy = float(nodes[i].y + nodes[i+1].y)/2.f;
        
        m_quadPaths[i]->quadraticTo(float(nodes[i].x), float(nodes[i].y), midx, midy);
    }
    //special calculation if the last path needs to be calculated
    if(runLastNode)
    {
        m_quadPaths[i]->clear();
        m_quadPaths[i]->startNewSubPath(midx,midy);
        m_quadPaths[i]->quadraticTo(float(nodes[nodes.size() - 2].x), float(nodes[nodes.size() - 2].y), float(nodes[nodes.size()-1].x), float(nodes[nodes.size()-1].y));
    }
}
XmlElement* BezierPlotter::getXMLElement(int _plotNum)
{
    //set a name for our element and creat it
    String plotNumberString = "bezierPlot";
    plotNumberString += _plotNum;
    StringRef plotNumberStringRef = plotNumberString;
    XmlElement* plotNumber = new XmlElement(plotNumberStringRef);

    plotNumber->setAttribute("numOfNodes", getNumNode());
    
   
    //add an x and y attribute to it based on its size
    for(int i = 0; i < nodes.size(); i++)
    {
        String bezierXString = "bezierXID";
        String bezierYString = "bezierYID";
        bezierXString += i;
        bezierYString += i;
        Identifier bezierXID = bezierXString;
        Identifier bezierYID = bezierYString;
        plotNumber->setAttribute(bezierXID, nodes[i].x);
        plotNumber->setAttribute(bezierYID, nodes[i].y);
    }
    
    //return the element, someone will have to clean this up
    return plotNumber;
    
}
void BezierPlotter::setXMLElement(juce::XmlElement *_xmlElement)
{
    
    //clear the nodes set a new node for ever 2 attribues
    nodes.clear();
    for(int i = 0; i < _xmlElement->getIntAttribute("numOfNodes"); i++)
    {
        String bezierXString = "bezierXID";
        String bezierYString = "bezierYID";
        bezierXString += i;
        bezierYString += i;
        Identifier bezierXID = bezierXString;
        Identifier bezierYID = bezierYString;
        
        nodes.push_back(Point<float>(_xmlElement->getDoubleAttribute(bezierXString), _xmlElement->getDoubleAttribute(bezierYString)));
    }
    setNodes(nodes);
}

