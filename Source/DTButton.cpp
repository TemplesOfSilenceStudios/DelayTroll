/*
  ==============================================================================

    DTButton.cpp
    Created: 22 Apr 2019 4:50:53pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DTButton.h"

//==============================================================================
DTButton::DTButton(const String& buttonLabel,  const Image& _offImage, const Image& _onImage)
:   juce::ImageButton()
{
    setImages(false, false, true, _onImage, 1.f, Colours::transparentWhite, _onImage, 1.f, Colours::transparentWhite, _offImage, 1.f, Colours::transparentWhite);
    setSize(_onImage.getWidth(), _onImage.getHeight());
    setName(buttonLabel);
}
DTButton::DTButton(AudioProcessorValueTreeState& stateToControl,
                    const String& parameterID,
                    const String& parameterLabel,
                   const Image& _offImage,
                   const Image& _onImage)
{
    setName(parameterLabel);
    mAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(stateToControl, parameterID, *this);
    setImages(false, false, true, _onImage, 1.f, Colours::transparentWhite, _onImage, 1.f, Colours::transparentWhite, _offImage, 1.f, Colours::transparentWhite);
    setSize(_onImage.getWidth(), _onImage.getHeight());
}

DTButton::~DTButton()
{
}


