/*
  ==============================================================================

    DTParamCheckBox.cpp
    Created: 5 Feb 2020 8:28:40pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "DTParamCheckBox.h"


//==============================================================================
DTParamCheckBox::DTParamCheckBox(AudioProcessorValueTreeState& stateToControl,
                                 const String& parameterID,
                                 const String& parameterLabel): ToggleButton(parameterLabel)
{
    setButtonText(parameterLabel);
    mAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(stateToControl, parameterID, *this);
    
}

DTParamCheckBox::DTParamCheckBox(Value* _valueToReferTo, const String& _label) : ToggleButton(_label)
{
    getToggleStateValue().referTo(*_valueToReferTo);
}

DTParamCheckBox::DTParamCheckBox(String _label): ToggleButton(_label)
{
}

DTParamCheckBox::~DTParamCheckBox()
{
}
