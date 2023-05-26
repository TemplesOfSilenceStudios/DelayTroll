/*
  ==============================================================================

    DTParamComboBox.h
    Created: 19 Apr 2019 1:02:45pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once


#include "JuceHeader.h"

class DTParamComboBox
:   public ComboBox
{
public:
    DTParamComboBox(AudioProcessorValueTreeState& stateToControl,
                      const String& parameterID,
                      const String& parameterLabel,
                    const Image& _image);
    DTParamComboBox(Value* _valueToReferTo, const String& parameterLabel, const Image& _image);
    DTParamComboBox(const String& parameterLabel, const Image& _image);
    ~DTParamComboBox();
    
    void mouseEnter(const MouseEvent& _e) override;
    void mouseExit(const MouseEvent& _e) override;

    
    void paint(Graphics& _g)override;
    
private:
    
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> mAttachment;
    
    const Image& mBackgroundImage;
    bool mIsMouseOver = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DTParamComboBox);
    
};
