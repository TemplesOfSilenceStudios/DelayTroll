/*
  ==============================================================================

    LFOCurveDisplay.cpp
    Created: 26 Apr 2019 2:11:53pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LFOCurveDisplay.h"
#include "DTModulationDefines.h"
#include "DTInterfaceDefines.h"

//==============================================================================
LFOCurveDisplay::LFOCurveDisplay(DelayTrollAudioProcessor& p, int _plotEditingIndex, float _width, float _height)
:   mLFOPlot(*(dynamic_cast<LFOPlotter*>(p.getModulationArray()[_plotEditingIndex]))),
    pathStroke(3.f, PathStrokeType::curved, PathStrokeType::rounded)
{
    setSize(_width, _height);
    //DBG("Curve display Construction");
    //mLFOPlot.addWave(1.0f, 1.0f, 1.0f, kLFO_Sine);
}

LFOCurveDisplay::~LFOCurveDisplay()
{
}

void LFOCurveDisplay::paint (Graphics& g)
{
    
    float dashArray[4]{4,4,4,4};
    g.setColour (Colours::black);
    float height = getLocalBounds().toFloat().getHeight() * (1.f-mLFOPlot.getPolarValue());
    Line<float> pol(0.f, height, getLocalBounds().toFloat().getWidth(), height);
    
    g.drawDashedLine(pol, dashArray, 4, 1.f,0);
    
    Path tempPath;
    tempPath.preallocateSpace(200);
    
    float arraysize = mLFOPlot.getarraySize();
    float fWidth = static_cast<float>(getWidth());
    float fHight = static_cast<float>(getHeight());
    
    for(int i = 0 ; i < DTmodArraySize - 1; i++){
        //get y ajust to the polar 0 and scale to the height
        float y = 1.f - mLFOPlot[i];
        float yPlus = 1.f - mLFOPlot[i + 1];
        y *= fHight;
        yPlus *= fHight;
        
        //get x and scale to the height
        float x = static_cast<float>(i)/arraysize;
        float xPlus = static_cast<float>(i + 1)/arraysize;
        x *= fWidth;
        xPlus *= fWidth;
        
        //check to make sure we arnt trying to draw a line outside the display
        if((y && yPlus < getHeight()) && (y && yPlus >= 0.0f )){
            // make a line and add it to the path
            Line<float> tempLine(x, y, xPlus, yPlus);
            tempPath.addLineSegment(tempLine, 3.0f);
        }
    }//mod array loop
    
    //draw the path
    g.setColour (DTColourIds::LrgDisplayGlow);
    pathStroke.setStrokeThickness(7.f);
    g.setOpacity(0.4f);
    g.strokePath(tempPath, pathStroke);
    pathStroke.setStrokeThickness(3.f);
    g.setOpacity(1.f);
    g.strokePath(tempPath, pathStroke);
}

void LFOCurveDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
