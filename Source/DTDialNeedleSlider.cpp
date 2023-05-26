/*
  ==============================================================================

    DTDialNeedleSlider.cpp
    Created: 20 Oct 2020 10:57:26am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DTDialNeedleSlider.h"
#include "DTModulationDefines.h"

//==============================================================================
DTDialNeedleSlider::DTDialNeedleSlider(AudioProcessorValueTreeState& stateToControl,
                                       const String& parameterID,
                                       const String& parameterLabel,
                                       DTModulation::DTAssignableParam& _param):
mNeedleImage(ImageCache::getFromMemory(BinaryData::Dial_Needle_png, BinaryData::Dial_Needle_pngSize)),
mModParamRef(_param)
{
    setSize(41, 70);
    mCentreRect.setSize(5.f, 5.f);
    
    mNeedle = std::make_unique<DrawableImage>();
    mNeedle->setImage(mNeedleImage);
    
    if(mModParamRef.typeIndex == kAss_OutputLevel){
        mImageBounds = Rectangle<int>(0,0, mNeedleImage.getWidth(),mNeedleImage.getHeight());
        setRotaryParameters(degreesToRadians(310.f), degreesToRadians(180.f), true);
    }
    else if (mModParamRef.typeIndex == KAss_InputLevel){
        mImageBounds = Rectangle<int>(getRight() - mNeedleImage.getWidth(),0, mNeedleImage.getWidth(),mNeedleImage.getHeight());
        setRotaryParameters(degreesToRadians(50.f), degreesToRadians(180.f), true);
    }
    
    mImageBounds = mImageBounds.withY(getLocalBounds().getCentreY());
    setSliderStyle(SliderStyle::RotaryVerticalDrag);
    
    mNeedle->setBounds(mImageBounds);
    mNeedle->setBoundingBox(mImageBounds.toFloat());

    mNeedle->setInterceptsMouseClicks(false, false);
    
    //ADD AND MAKE VISABE MUST HAPPEN AFTER GET BOUNDS
   
    mRotate = mNeedle->getTransform();
    mDefalt = mNeedle->getTransform();
    
    NormalisableRange<double> range(stateToControl.getParameter(parameterID)->getNormalisableRange().getRange().getStart(),
                              stateToControl.getParameter(parameterID)->getNormalisableRange().getRange().getEnd(),
                              stateToControl.getParameter(parameterID)->getNormalisableRange().interval,
                              stateToControl.getParameter(parameterID)->getNormalisableRange().skew);
    setNormalisableRange(range);

    mAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(stateToControl, parameterID, *this);
    
    addChildComponent(*mNeedle);
    
    
   startTimer(100);
}

DTDialNeedleSlider::~DTDialNeedleSlider()
{
    stopTimer();
}

void DTDialNeedleSlider::paint (juce::Graphics& g)
{
  
    
    auto lineW = 4.f;
    
    
    //auto fill    = findColour (Slider::rotarySliderFillColourId).withAlpha(0.8f);
    g.setColour (Colours::yellow);
    g.setOpacity(0.5f);
    g.strokePath (mModRangePath, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
    
    if(mModParamRef.isAttached){
        //g.drawEllipse(mCentrePoint.getX(), mCentrePoint.getY(), 5.f, 5.f, 2.f);
        g.drawEllipse(mCentreRect, 2.f);
    }
    mNeedle->draw(g, 1.f);
}

void DTDialNeedleSlider::resized()
{
}
 void DTDialNeedleSlider::valueChanged()
{
    mNeedle = std::make_unique<DrawableImage>();

    mNeedle->setImage(mNeedleImage);

    mNeedle->setBounds(mImageBounds);
    mNeedle->setBoundingBox(mImageBounds.toFloat());
    
    
    mRotate = mDefalt;
    //mRotate = mDefalt;
    
    float value = 0.f;
    if(mModParamRef.typeIndex == kAss_OutputLevel)
        value = jmap(static_cast<float>(getValue()), 310.f, 195.f);
    else if (mModParamRef.typeIndex == KAss_InputLevel)
        value = jmap(static_cast<float>(getValue()), 50.f, 165.f);
    
    float rads = degreesToRadians(value);
    
    mRotate = mRotate.rotated(rads, mNeedle->getBounds().toFloat().getCentreX(),mNeedle->getBounds().toFloat().getY());
    
    mNeedle->setTransform(mRotate);
    
    repaint();
}
void DTDialNeedleSlider::timerCallback()
{
    mModRangePath.clear();
    mCentrePoint.setXY(0.f,0.f);
    
    double centreValue = getValue();
    double maxOfset = 0.0;
    double minOfset = 0.0;
    
    //cycle through the parameters modulation managers and inciment the ofsets from their polar and depth values
    for(auto mod : mModParamRef.modulationManagers)
    {
        int index = mModParamRef.typeIndex;
        
        //only works if the same manager isn't applied twice
        double depth = mod->getDepthValue(index);
        double polar = mod->getPolarValue();
        
        maxOfset += (1.0 - polar) * depth;
        minOfset += (0.0 - polar) * depth;
    }
    centreValue = (centreValue - getRange().getStart()) / getRange().getLength();
        
    double maxValue = centreValue + maxOfset;
    double minValue = centreValue + minOfset;
        
    maxValue = jmax(centreValue, maxValue);
    maxValue = jmin(1.0, maxValue);
    minValue = jmin(centreValue, minValue);
    minValue = jmax(0.0, minValue);
    
    maxValue = getRange().getStart() + getRange().getLength() * maxValue;
    minValue = getRange().getStart() + getRange().getLength() * minValue;
    
    maxValue = valueToProportionOfLength(maxValue);
    minValue = valueToProportionOfLength(minValue);
    
    centreValue = valueToProportionOfLength(static_cast<double>(mModParamRef.getNextValue()));
    
    auto radius = 33.f;
    
    if(mModParamRef.typeIndex == kAss_OutputLevel){
        centreValue = jmap(1.0-centreValue, 285.0, 360.0+45.0);
        centreValue = centreValue > 360.0 ? centreValue - 360.0:centreValue;
        mCentrePoint.setX(radius - 1.f);
        
        minValue = jmap(1.0-minValue, 15.0, 135.0);
        maxValue = jmap(1.0-maxValue,  15.0, 135.0);
    }
    else{
        centreValue = jmap(centreValue, 135.0, 255.0);
        mCentrePoint.setX(radius);
        
        minValue = jmap(minValue, 225.0, 345.0);
        maxValue = jmap(maxValue,  225.0, 345.0);
    }
    
    mCentrePoint = mCentrePoint.rotatedAboutOrigin(degreesToRadians(centreValue));
    
    minValue = degreesToRadians(minValue);
    maxValue = degreesToRadians(maxValue);
    
    auto lineW = 4.f;

    auto arcRadius = radius - lineW * 0.5f;
    
    if(mModParamRef.typeIndex == kAss_OutputLevel){
        mModRangePath.addCentredArc(mImageBounds.getWidth()/2, mImageBounds.toFloat().getY(), arcRadius, arcRadius-1.5f, 0.0f, minValue, maxValue, true);
        //float xtranslate = jmap(mCentrePoint.getX()/(radius-1.f), 0.f, 3.f);
        //float ytranslate = jmap(1.f - mCentrePoint.getX()/radius, 0.f, 4.f);
        
        //mCentrePoint = mCentrePoint.translated(3.f - xtranslate, mImageBounds.toFloat().getY() + ytranslate - 5.f);
        mCentrePoint = mCentrePoint.translated(0.f, mImageBounds.toFloat().getY());
    }
    else{
        mModRangePath.addCentredArc(getWidth()-mImageBounds.getWidth()/2, mImageBounds.toFloat().getY(), arcRadius, arcRadius-1.5f, 0.0f, minValue, maxValue, true);
        //float ytranslate = jmap(abs(mCentrePoint.getX())/radius, 0.f, 4.f);
        //float xtranslate =   - jmap(1.f - abs(mCentrePoint.getX())/(radius-1.f), 3.f, 7.f);
        
        //mCentrePoint = mCentrePoint.translated(xtranslate, mImageBounds.toFloat().getY() - ytranslate);
        
        mCentrePoint = mCentrePoint.translated(static_cast<float>(getWidth()), mImageBounds.toFloat().getY());
        
    }
    mModRangePath.getNearestPoint(mCentrePoint, mCentrePoint);

    mCentreRect.setCentre(mCentrePoint);
    
    repaint();
}
