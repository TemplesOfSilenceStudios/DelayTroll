/*
  ==============================================================================

    BezierDisplayPanel.cpp
    Created: 24 Apr 2019 11:24:21am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "BezierDisplayPanel.h"
//#include "DTModulationDefines.h"

//==============================================================================
BezierDisplayPanel::BezierDisplayPanel(DelayTrollAudioProcessor& p, int _index, int _width, int _height)
:   ModulationDisplayPanelBase(p, _index),
    mCurve(p, _index, MOD_INNER_DISPLAY_PANEL_WIDTH-24, MOD_INNER_DISPLAY_PANEL_HEIGHT-10),
    mResetImageOn(ImageCache::getFromMemory(BinaryData::DeleteDownSmall_png, BinaryData::DeleteDownSmall_pngSize)),
    mResetImageOff(ImageCache::getFromMemory(BinaryData::DeleteUpSmall_png, BinaryData::DeleteUpSmall_pngSize)),
    mResetLableImage(ImageCache::getFromMemory(BinaryData::ResetBezLabel_png, BinaryData::ResetBezLabel_pngSize))
{

    setSize(_width, _height);
    
    mCurve.setTopLeftPosition(DISPLAY_PANEL_HORIZONTAL_BUMPER+18, DISPLAY_PANEL_VERTICAL_BUMPER+6);
    mCurve.setHasPolarValue(true);
    addAndMakeVisible(&mCurve);
    addAndMakeVisible(&mPolar);
    mPolar.setInterceptsMouseClicks(false, false);
    addChildComponent(&mBubble);
    
    mDefaultNotes = mCurve.getNodes();
    mReset = std::make_unique<DTButton>("Reset", mResetImageOn, mResetImageOff);
    //mReset->setSize(25, 25);
    mReset->setTopLeftPosition((getWidth()/8), 7);
    mReset->onClick = [this](){
        std::vector<Point<float>> nodes;
        
        nodes.push_back(Point<float>(0, 1.0f));
        nodes.push_back(Point<float>(0.2f, 0.8f));
        nodes.push_back(Point<float>(0.5f, 0.2f));
        nodes.push_back(Point<float>(0.8f, 0.8f));
        nodes.push_back(Point<float>(1.0f, 0));
        
        mCurve.getPlotter()->setNodes(nodes);
        mCurve.resetNodes();
    };
    addAndMakeVisible(*mReset);
    
    
    
    mResetLable.setImage(mResetLableImage);
    mResetLable.setSize(mResetLableImage.getWidth(), mResetLableImage.getHeight());
    //mResetLable.setTopLeftPosition(mReset->getRight(), 0);
    mResetLable.setCentrePosition(mReset->getBounds().getCentreX() + mReset->getWidth()/2 + mResetLable.getWidth()/2 + 3, mReset->getBounds().getCentreY()-1);
    mResetLable.setBoundingBox(mResetLable.getBounds().toFloat());
    addAndMakeVisible(&mResetLable);
    
    //mBubble.setAlwaysOnTop(true);
}

BezierDisplayPanel::~BezierDisplayPanel()
{
}
void BezierDisplayPanel::mouseDown(const MouseEvent &_e){
    Point<float> translated = _e.getPosition().toFloat().transformedBy(AffineTransform::translation(Point<float>(0,7) - mCurve.getPosition().toFloat()));
    MouseEvent eTranslated = _e.withNewPosition(translated);
  if(mPolar.getBounds().removeFromLeft(mCurve.getX()).contains(_e.getPosition()))
      mPolar.mouseDown(eTranslated);
}
void BezierDisplayPanel::mouseDrag(const MouseEvent &_e){
    Point<float> translated = _e.getPosition().toFloat().transformedBy(AffineTransform::translation(Point<float>(0,7) - mCurve.getPosition().toFloat()));
    MouseEvent eTranslated = _e.withNewPosition(translated);
  if(mPolar.getBounds().contains(_e.getPosition()))
      mPolar.mouseDrag(eTranslated);
}
