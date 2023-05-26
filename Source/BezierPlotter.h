/*
  ==============================================================================

    BezierPlotter.h
    Created: 9 Mar 2019 8:25:54am
    Author:  Sam Ferguson

  ==============================================================================
 should probably make copy constructor for this =
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "ModManagment.h"

//**Plotter class that "plots" an array by measuring a Bezier Curve*/
class BezierPlotter : public ModManagment
{
public:
    
    /**creates a new Plotter for the provided array with the provided size*/
    BezierPlotter(double* const& Array, float _arraySize, float _polar, int _managmentID, Value& _BPM, Value& _timeSigNumerator);
   
    ~BezierPlotter();

    /**copys the array, not sure if this is needed*/
    void fillArray(double array[1000]) const{ memcpy(array, m_plotArray, sizeof(double) * 1000);};
    
    /**resets all nodes*/
    void setNodes(std::vector<Point<float>> newNodes);
    
    /**returns the nodes*/
    inline std::vector<Point<float>> getNodes() {return nodes;};
    
    /**gives the node at nodeNumb a new value*/
    void setNode(Point<float> newNode, int nodeNumb){ nodes[nodeNumb] = newNode;};
    
    /**adds new node at nodeNumb*/
    void newNode(Point<float> newNode, int nodeNumb);
    
    /**removes a node*/
    void removeNode(int nodeNumb);
    
    /**clears the plot to default*/
    void clearPlot();
    
    /*  *Draws a muli point Bezier Curve
        *note: this is not a nth order Bezier curve but a bunch of quadratic Bezier curves stiched together*/
    void multipointBezierCurve(int draggingNodeNum, bool update);
    
    /**measures just the part of the curve arround the draggingNodeNumber*/
    void measureCurve(int draggingNodeNum);
    
    /**returns refference to the array*/
    inline double *getArray() const{return m_plotArray;};
    
    /**returns the number of nodes used by the plot*/
    inline int getNumNode() const{ return static_cast<int>(nodes.size());};
    
    /**get the paths that will draw the curves*/
    inline OwnedArray<Path>& getQuadPaths(){return m_quadPaths;};
    
    /**gets an XMLElement representing the state of the plot*/
    XmlElement* getXMLElement(int _plotNum);
    
    /**sets the parameters of the plotter based on the provided XMLElement*/
    void setXMLElement(XmlElement*);
    
    
private:
    
    Array<Value> modValues;
    
    //our x and y position as nodes, could be abstracted to std::pair<int, int>
    std::vector<Point<float>> nodes;
    

    //array of paths that will be drawn, these start at 1 not 0...for some reason, not sure this is still true
    //an ownedarray is like an array of smart pointers, and a path is an object that manages an array of markers
    //these markers can denote different relationships, a line maker point sugests a streight line to the next marker
    //a quad path maker sugests a quadradic curve to the next marker and will contain the nessisary mathematical specifications for the curve
    OwnedArray<Path> m_quadPaths;
    
    //measures the whole curve
    void measureCurve();
    
    float hight;
    float width;
    
};
