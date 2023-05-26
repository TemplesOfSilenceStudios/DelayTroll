/*
  ==============================================================================

    DTParamCheckBox.h
    Created: 5 Feb 2020 8:28:40pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once



#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
 */
class DTParamCheckBox    : public ToggleButton
{
public:
    DTParamCheckBox(AudioProcessorValueTreeState& stateToControl,
                      const String& parameterID,
                      const String& parameterLabel);
    
    DTParamCheckBox(Value* _valueToRefference, const String& _parameterLabel);
    
    DTParamCheckBox(String _label);
    ~DTParamCheckBox();
    
    
private:
    
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> mAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DTParamCheckBox)
};
