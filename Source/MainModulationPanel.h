/*
  ==============================================================================

    MainModulationPanel.h
    Created: 22 Apr 2019 4:48:07pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "PanelBase.h"
#include "MainModMenuPanel.h"
#include "AssignPanel.h"
#include "LFOModulationPanel.h"
#include "BezierModulationPanel.h"

//==============================================================================
/**container panel for the GUI's modulation section
*/
class MainModulationPanel    : public PanelBase, public Button::Listener, public DelayTrollAudioProcessor::Listener
{
public:
    MainModulationPanel(DelayTrollAudioProcessor& p);
    ~MainModulationPanel();

    void paint (Graphics&) override;
    void resized() override;
    
    /**gets a refference to the modulation menue panel*/
    MainModMenuPanel& getMenuPanel(){return mModMenuPanel;};
    
    /**handels menu button events and sets the visablity of the mModPanels acordingly*/
    void buttonClicked (Button* b) override;
    
    /**called by the processor to signal a new preset has loaded*/
    void loadActivated(DelayTrollAudioProcessor* p) override;

private:
    
    MainModMenuPanel mModMenuPanel;
    
    OwnedArray<ModulationPanelBase> mModPanelPtrs;
    
    int selectedModulation;

    int Adder;
    
    Identifier disValID;
    
    Viewport mViewPort;
    
    Identifier mSelectedPlot;
    
    Image mBackgroundImage;
    
    DrawableImage mBackgroundDrawable;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainModulationPanel)
};
