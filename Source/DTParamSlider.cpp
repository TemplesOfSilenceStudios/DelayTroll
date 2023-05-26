/*
  ==============================================================================

    DTParamSlider.cpp
    Created: 19 Apr 2019 1:02:12pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTParamSlider.h"
#include "DTModulationDefines.h"
#include "DTInterfaceDefines.h"
DTParameterSlider::DTParameterSlider(AudioProcessorValueTreeState& stateToControl,
                                     const String& parameterID,
                                     const String& parameterLabel,
                                     const Image& _image):
juce::Slider(parameterLabel),
mIsModulatable(false),
mIsReversed(false),
mIsGhostReversed(false),
mGearImage(_image)
{
    setSize(mGearImage.getWidth(), mGearImage.getWidth());
    setSliderStyle(SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(NoTextBox, false, 0, 0);
    NormalisableRange<double> range(stateToControl.getParameter(parameterID)->getNormalisableRange().getRange().getStart(),
                              stateToControl.getParameter(parameterID)->getNormalisableRange().getRange().getEnd(),
                              stateToControl.getParameter(parameterID)->getNormalisableRange().interval,
                              stateToControl.getParameter(parameterID)->getNormalisableRange().skew);
    setNormalisableRange(range);

    mAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(stateToControl, parameterID, *this);

    addAndMakeVisible(mLabel);
    mLabel.setText(parameterLabel, dontSendNotification);
    //mLabel.attachToComponent(this, false);
    mLabel.setColour(Label::textColourId, Colours::orange);
    mLabel.setJustificationType(Justification::centred);
    mLabel.setBorderSize(BorderSize<int> (0,0,0,0));
    
    setBufferedToImage(true);
    
}
DTParameterSlider::DTParameterSlider(NormalisableRange<double> _range,
                                     float _value,
                                     const String& parameterLabel,
                                     const Image& _image) :
mIsModulatable(false),
mIsReversed(false),
mIsGhostReversed(false),
mGearImage(_image)
{
    setSize(mGearImage.getWidth(), mGearImage.getWidth());
    setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    setNormalisableRange(_range);
    setValue(_value, dontSendNotification);
    setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    setScrollWheelEnabled(false);
    
    addAndMakeVisible(mLabel);
    mLabel.setText(parameterLabel, dontSendNotification);
    //mLabel.attachToComponent(this, false);
    mLabel.setColour(Label::textColourId, Colours::orange);
    mLabel.setJustificationType(Justification::centred);
    mLabel.setBorderSize(BorderSize<int> (0,0,0,0));
    setBufferedToImage(true);
    
}
DTParameterSlider::DTParameterSlider(Value& _valueToReferTo,  NormalisableRange<double> _range, const String& parameterLabel, const Image& _image):
juce::Slider(parameterLabel),
mIsModulatable(false),
mIsReversed(false),
mIsGhostReversed(false),
mGearImage(_image)
{
    setSize(mGearImage.getWidth(), mGearImage.getWidth());
    setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    setNormalisableRange(_range);
    getValueObject().referTo(_valueToReferTo);
    setTextBoxStyle(NoTextBox, false, 0, 0);
    
    addAndMakeVisible(mLabel);
    mLabel.setText(parameterLabel, dontSendNotification);
    //mLabel.attachToComponent(this, false);
    mLabel.setColour(Label::textColourId, Colours::orange);
    mLabel.setJustificationType(Justification::centred);
    mLabel.setBorderSize(BorderSize<int> (0,0,0,0));
    setBufferedToImage(true);
}
DTParameterSlider::~DTParameterSlider()
{
    stopTimer();
}

void DTParameterSlider::paint(juce::Graphics &_g)
{

    if(isRotary())
    {
        
        //auto outline = findColour (Slider::rotarySliderOutlineColourId);
        auto fill    = findColour (Slider::rotarySliderFillColourId).withAlpha(0.5f);
       
        auto bounds = getLookAndFeel().getSliderLayout(*this).sliderBounds.toFloat().reduced(5);

        auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        
        auto lineW = jmin (8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;
        
        RotaryParameters rotaryParams = getRotaryParameters();
        
        Path backgroundArc;
        backgroundArc.addCentredArc (bounds.getCentreX(),
                                     bounds.getCentreY(),
                                     arcRadius,
                                     arcRadius,
                                     0.0f,
                                     rotaryParams.startAngleRadians,
                                     rotaryParams.endAngleRadians,
                                     true);
        
        //_g.setColour (outline);
        //_g.strokePath (backgroundArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
        
        const int numFrames = mGearImage.getHeight() / mGearImage.getWidth();
        
        //int gearmult =mGearImage.getWidth() == 120? 175:mGearImage.getWidth();
        
        const int frameIndex = static_cast<int>(std::ceil(valueToProportionOfLength(getValue()) * (numFrames - 1)));
        
        //const float radius = jmin(width * 0.5f, height * 0.5f);
        radius = getLocalBounds().toFloat().getWidth()/2.f;
        //auto width =bounds.getWidth();
        //auto height = bounds.getHeight();
        //auto x = bounds.getX();
        //auto y = bounds.getY();
        
        //const float centreX = bounds.getX() + bounds.getWidth() * 0.5f;
        //const float centreY = bounds.getY() + bounds.getHeight() * 0.5f;
        //const float rx = centreX - radius;
        //const float ry = centreY - radius;
        
        //Draws the image file at the current frameIndex
        if(mIsReversed){
            const int frameReversed = (numFrames - 1) - frameIndex;
            _g.drawImage(mGearImage,
                         0,
                         0,
                         mGearImage.getWidth(),
                         mGearImage.getWidth(),
                         0,
                         frameReversed * mGearImage.getWidth(),
                         mGearImage.getWidth(),
                         mGearImage.getWidth());
        }
        else{
            _g.drawImage(mGearImage,
                         0,
                         0,
                         mGearImage.getWidth(),
                         mGearImage.getWidth(),
                         0,
                         frameIndex * mGearImage.getWidth(),
                         mGearImage.getWidth(),
                         mGearImage.getWidth());
        }
     
        if (mIsModulatable && mModParamPtr->isAttached)
        {
            //DBG("starting modulation display paint===========================================================");
            double centreValue = getValue();

            double maxOfset = 0.0;
            double minOfset = 0.0;
            
            //cycle through the parameters modulation managers and inciment the ofsets from their polar and depth values
            for(auto mod : mModParamPtr->modulationManagers)
            {
                int index = mModParamPtr->typeIndex;
                
                //only works if the same manager isn't applied twice
                double depth = mod->getDepthValue(index);
                double polar = mIsGhostReversed ? 1.0 - mod->getPolarValue() : mod->getPolarValue();
                
                maxOfset += (1.0 - polar) * depth;
                minOfset += (0.0 - polar) * depth;
            }
                
            //centreValue = mModParamPtr->range.convertTo0to1(centreValue);
            //centreValue = ((float)centreValue - mModParamPtr->range.getRange().getStart()) / mModParamPtr->range.getRange().getLength();
            centreValue = (centreValue - getRange().getStart()) / getRange().getLength();
                
            double maxValue = centreValue + maxOfset;
            double minValue = centreValue + minOfset;
                
            maxValue = jmax(centreValue, maxValue);
            maxValue = jmin(1.0, maxValue);
                
            minValue = jmin(centreValue, minValue);
            minValue = jmax(0.0, minValue);
                
            //maxValue = mModParamPtr->range.convertFrom0to1(maxValue);
            //minValue = mModParamPtr->range.convertFrom0to1(minValue);
                
//            maxValue = mModParamPtr->range.getRange().getStart() + mModParamPtr->range.getRange().getLength() * maxValue;
//            minValue = mModParamPtr->range.getRange().getStart() + mModParamPtr->range.getRange().getLength() * minValue;
            
            maxValue = getRange().getStart() + getRange().getLength() * maxValue;
            minValue = getRange().getStart() + getRange().getLength() * minValue;

            if(mModParamPtr->typeIndex == kAss_MinTime)
            {
                maxValue = maxValue < static_cast<double>(mProcessorPtr->getAssignableParameters()[kAss_MaxTime]->getNextValue()) ? maxValue : static_cast<double>(mProcessorPtr->getAssignableParameters()[kAss_MaxTime]->getNextValue());
            }
            else if (mModParamPtr->typeIndex == kAss_MaxTime)
            {
                minValue = minValue > static_cast<double>(mProcessorPtr->getAssignableParameters()[kAss_MinTime]->getNextValue()) ? minValue : static_cast<double>(mProcessorPtr->getAssignableParameters()[kAss_MinTime]->getNextValue());
            }

            centreValue = valueToProportionOfLength(static_cast<double>(mModParamPtr->getNextValue()));
            
            maxValue = valueToProportionOfLength(maxValue);
            
            //maxValue = ((float)maxValue - mModParamPtr->range.getRange().getStart()) / mModParamPtr->range.getRange().getLength();
            minValue = valueToProportionOfLength(minValue);
            //minValue = ((float)minValue - mModParamPtr->range.getRange().getStart()) / mModParamPtr->range.getRange().getLength();
            
            if(mIsReversed){
                maxValue = 1.0 - maxValue;
                minValue = 1.0 - minValue;
                centreValue = 1.0 - centreValue;
            }
            
            
            auto fromAngle = rotaryParams.startAngleRadians + minValue * (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
            auto toAngle = rotaryParams.startAngleRadians + maxValue * (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
            
            //wrapping the angles if the range of the modulation takes the toAngle past 3oclock...I think it's 3oclock at least assuming the rotation is normal
            if(toAngle < 0.0)
            {
                toAngle = MathConstants<double>::twoPi + toAngle;
                fromAngle += MathConstants<double>::twoPi;
            }
            arcRadius *= mArcRadiusMult;
            //DBG(mArcRadius);
            
            Path valueArc;
            valueArc.addCentredArc (bounds.getCentreX(),
                                    bounds.getCentreY(),
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    fromAngle,
                                    toAngle,
                                    true);
         
            valueArc.applyTransform(mTranslate);

            _g.setColour (fill);
            _g.strokePath (valueArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
   
            
            auto modAngle = rotaryParams.startAngleRadians + centreValue * (rotaryParams.endAngleRadians - rotaryParams.startAngleRadians);
            auto thumbWidth = lineW * 2.0f;
            Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (modAngle - MathConstants<float>::halfPi),
                                     bounds.getCentreY() + arcRadius * std::sin (modAngle - MathConstants<float>::halfPi));
            thumbPoint.applyTransform(mTranslate);
            
            _g.setColour (juce::Colours::bisque);
            _g.setOpacity(0.5f);
            _g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
            //DBG("Ending Modulation Code=============================================================");
        }//end modulatable
        
        
        Slider::paint(_g);
    }//end rotary
    else if(isHorizontal())
    {
        
        auto bounds = getLookAndFeel().getSliderLayout(*this).sliderBounds.toFloat().reduced(5);
        
        auto trackWidth = jmin (6.0f, getHeight() * 0.25f);
        
        Point<float> startPoint (bounds.getX(), bounds.getCentreY());
        
        Point<float> endPoint ( bounds.getRight(), startPoint.y);
        
        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        _g.setColour (findColour (Slider::backgroundColourId));
        _g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });
    
        if (mIsModulatable && mModParamPtr->isAttached)
           {
               double centreValue = getValue();
               
               double maxValue = centreValue;
               double minValue = centreValue;

               double maxOfset = 0.0;
               double minOfset = 0.0;
               
               //cycle through the parameters modulation managers and inciment the ofsets from their polar and depth values
               for(auto mod : mModParamPtr->modulationManagers)
               {
                   
                   int index = mModParamPtr->typeIndex;
                   
                   //only works if the same manager isn't applied twice
                   double depth = mod->getDepthValue(index);
                   double polar = mod->getPolarValue();
                   
                   maxOfset += (1.0 - polar) * depth;
                   minOfset += (0.0 - polar) * depth;
               }

               centreValue = ((float)centreValue - mModParamPtr->range.getRange().getStart()) / mModParamPtr->range.getRange().getLength();
                   
               maxValue = centreValue + maxOfset;
               minValue = centreValue + minOfset;
                   
               maxValue = jmax(centreValue, maxValue);
               maxValue = jmin(1.0, maxValue);
                   
               minValue = jmin(centreValue, minValue);
               minValue = jmax(0.0, minValue);
                   
               //maxValue = mModParamPtr->range.convertFrom0to1(maxValue);
               //minValue = mModParamPtr->range.convertFrom0to1(minValue);
                   
               maxValue = mModParamPtr->range.getRange().getStart() + mModParamPtr->range.getRange().getLength() * maxValue;
               minValue = mModParamPtr->range.getRange().getStart() + mModParamPtr->range.getRange().getLength() * minValue;

               Path valueTrack;
               Point<float> minPoint, maxPoint, thumbPoint;
               
               minPoint = {bounds.getX() + mModParamPtr->range.convertTo0to1(minValue) * bounds.getWidth(), bounds.getCentreY()};
               maxPoint = {bounds.getX() + mModParamPtr->range.convertTo0to1(maxValue) * bounds.getWidth(), bounds.getCentreY()};
               

               valueTrack.startNewSubPath (minPoint);
               valueTrack.lineTo (maxPoint);
               _g.setColour (findColour (Slider::trackColourId));
               _g.strokePath (valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });
               
               auto thumbWidth = bounds.getHeight()/2.f;
 
               centreValue = valueToProportionOfLength(static_cast<double>(mModParamPtr->getNextValue()));
               
               thumbPoint = {bounds.getX() + (float)centreValue * bounds.getWidth(), bounds.getCentreY()};
               
               _g.setColour (Colours::bisque);
               _g.setOpacity(0.5f);
               _g.fillEllipse (Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (thumbPoint));
        
               
            }//end modulatable

        Slider::paint(_g);
    }//end horizontal
    else
    {
        
        Slider::paint(_g);
    }
    
}
void DTParameterSlider::setModulatableParameter(bool _isModulated, DTModulation::DTAssignableParam* _param,  DelayTrollAudioProcessor* _p)
{
    mIsModulatable = _isModulated;
    
    if(mIsModulatable){
        mModParamPtr = _param;
        mProcessorPtr = _p;
        startTimer(200);
    }
    else{
        stopTimer();
    }
    
}

void DTParameterSlider::timerCallback()
{
    if(mIsModulatable && mModParamPtr->isAttached){
        repaint(getLocalBounds());
    }
}
