/*
  ==============================================================================

    DTButton.h
    Created: 22 Apr 2019 4:50:53pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class DTButton    : public ImageButton
{
public:
    DTButton(const String& buttonLabel, const Image& _onImage, const Image& _offImage);
    DTButton(AudioProcessorValueTreeState& stateToControl,
            const String& parameterID,
            const String& parameterLabel,
             const Image& _onImage,
             const Image& _offImage);
    //DTButton(){};
    ~DTButton();


private:
    
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> mAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTButton)
};
