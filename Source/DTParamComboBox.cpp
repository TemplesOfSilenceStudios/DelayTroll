/*
  ==============================================================================

    DTParamComboBox.cpp
    Created: 19 Apr 2019 1:02:45pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTParamComboBox.h"

DTParamComboBox::DTParamComboBox(AudioProcessorValueTreeState& stateToControl,
                                 const String& parameterID,
                                 const String& parameterLabel,
                                 const Image& _image):
ComboBox(parameterLabel),
mBackgroundImage(_image)
{
    mAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(stateToControl, parameterID, *this);
    setSize(_image.getWidth(), _image.getHeight());
    setColour(ComboBox::ColourIds::textColourId, Colours::black);
    
    
    setJustificationType(Justification::centred);
}
DTParamComboBox::DTParamComboBox(Value* _valueToReferTo, const String& parameterLabel, const Image& _image):
ComboBox(parameterLabel),
mBackgroundImage(_image)
{
    getSelectedIdAsValue().referTo(*_valueToReferTo);
    setSize(_image.getWidth(), _image.getHeight());
    setColour(ComboBox::ColourIds::textColourId, Colours::black);
    setColour(ComboBox::ColourIds::backgroundColourId, Colours::blue);

    setJustificationType(Justification::centred);
}
DTParamComboBox::DTParamComboBox(const String& parameterLabel, const Image& _image):
ComboBox(parameterLabel),
mBackgroundImage(_image)
{
    setSize(_image.getWidth(), _image.getHeight());
    setColour(ComboBox::ColourIds::textColourId, Colours::black);
    setColour(ComboBox::ColourIds::backgroundColourId, Colours::blue);
    setJustificationType(Justification::centred);
}

DTParamComboBox::~DTParamComboBox(){}

void DTParamComboBox::mouseEnter(const MouseEvent& _e)
{
    mIsMouseOver = true;
    repaint();
}

void DTParamComboBox::mouseExit(const MouseEvent& _e)
{
    mIsMouseOver = false;
    repaint();
}

void DTParamComboBox::paint(Graphics& _g)
{
    _g.drawImage(mBackgroundImage, getLocalBounds().toFloat());
    
    if(mIsMouseOver && isEnabled()){
        Colour centreColour = Colours::white.withAlpha(0.65f);
        Colour edgeColour = Colours::white.withAlpha(0.f);
        Point<float> centrePoint = getLocalBounds().toFloat().getCentre();
        Point<float> edgePoint = centrePoint.withX(getLocalBounds().toFloat().getX() + (static_cast<float>(getWidth()) *0.1f));
        ColourGradient gradient(centreColour, centrePoint, edgeColour, edgePoint, true);
        _g.setGradientFill(gradient);
        _g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(1.f), 3.f);
    }
    //ComboBox::paint(_g);
}


