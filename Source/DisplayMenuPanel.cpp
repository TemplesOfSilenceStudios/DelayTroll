/*
  ==============================================================================

    DisplayMenuPanel.cpp
    Created: 23 Apr 2019 9:14:58am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DisplayMenuPanel.h"

//==============================================================================
DisplayMenuPanel::DisplayMenuPanel(DelayTrollAudioProcessor& p): PanelBase(p),
mTabbedBarImage(ImageCache::getFromMemory(BinaryData::TabBarA_png, BinaryData::TabBarA_pngSize)),
mTabbedBarGlImage(ImageCache::getFromMemory(BinaryData::TabBarAGl_png, BinaryData::TabBarAGl_pngSize)),
mTabbedBar("DisplayTabbedBar", mTabbedBarImage)
{
    
    
    
    setSize(DISPLAY_MENU_PANEL_WIDTH, DISPLAY_MENU_PANEL_HEIGHT);
    
    //mTabbedBar.setTopLeftPosition(50, 0);
    for(int i = 0 ; i < kDisPanel_TotalNumDisplayPanels; i++){
        mTabbedBar.addTab(DTDisplayPanelIDs[i], Colours::transparentWhite, i);
        //mTabbedBar.getTabButton(i)->setSize(mTabbedBarImage.getWidth()/kDisPanel_TotalNumDisplayPanels, mTabbedBarImage.getHeight());
    }
    
    mTabbedBar.setSize(mTabbedBarImage.getWidth(), mTabbedBarImage.getHeight());
    mTabbedBar.resized();
    mTabbedBar.setCentrePosition(getLocalBounds().getCentre());
    addAndMakeVisible(&mTabbedBar);
    mTabbedBar.setCurrentTabIndex(mProcessor.getDisplayValues()[DTDisplayValueIDs[kDIsVal_DisplayPanelSelection]]);
    
    mTabbedBarGl.setImage(mTabbedBarGlImage);
    mTabbedBarGl.setSize(mTabbedBarGlImage.getWidth(), mTabbedBarGlImage.getHeight());
    mTabbedBarGl.setCentrePosition(getLocalBounds().getCentre());
    mTabbedBarGl.setBoundingBox(mTabbedBarGl.getBounds().toFloat());
    addAndMakeVisible(&mTabbedBarGl);

}

DisplayMenuPanel::~DisplayMenuPanel()
{
}


void DisplayMenuPanel::addChangelistener(juce::ChangeListener *inListener)
{
    mTabbedBar.addChangeListener(inListener);
}
void DisplayMenuPanel::removeChangelistener(juce::ChangeListener *inListener)
{
    mTabbedBar.removeChangeListener(inListener);
}
