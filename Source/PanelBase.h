/*
  ==============================================================================

    PanelBase.h
    Created: 22 Apr 2019 4:42:48pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "DTInterfaceDefines.h"
#include "DTLookAndFeel.h"

//==============================================================================
/**BaseClass for all Delay Troll Panels
*/
class PanelBase    : public Component
{
public:
    PanelBase(DelayTrollAudioProcessor& p);
    ~PanelBase();

    virtual void paint (Graphics&) override;
    virtual void resized() override;

protected:
    
    DelayTrollAudioProcessor& mProcessor;
    std::unique_ptr<DTLookAndFeel> mLookAndFeel;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanelBase)
};
