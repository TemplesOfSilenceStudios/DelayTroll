/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "DTInterfaceDefines.h"

//==============================================================================
DelayTrollAudioProcessorEditor::DelayTrollAudioProcessorEditor (DelayTrollAudioProcessor& p):
AudioProcessorEditor (&p),
processor (p),
mTopPanel(p),
mMainPanel(p),
mMainModPanel(p),
mUnlockForm(mUnlockStatus)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //DBG("Editor Construction");
    
    setSize (EDITOR_PANEL_WIDTH, EDITOR_PANEL_HEIGHT);
    
    mBackgroundImage = ImageCache::getFromMemory(BinaryData::TopHalf_png, BinaryData::TopHalf_pngSize);
    
    mLookAndFeel = std::make_unique<DTLookAndFeel>();
    
    setLookAndFeel(mLookAndFeel.get());
    
    LookAndFeel::setDefaultLookAndFeel(mLookAndFeel.get());
    
    mTopPanel.setTopLeftPosition(0, 0);
    addAndMakeVisible(&mTopPanel);
    
    mBackgroundDrawable.setImage(mBackgroundImage);
    mBackgroundDrawable.setSize(mBackgroundImage.getWidth(), mBackgroundImage.getHeight());
    mBackgroundDrawable.setTopLeftPosition(0, mTopPanel.getBottom());
    mBackgroundDrawable.setBoundingBox(mBackgroundDrawable.getBoundsInParent().toFloat());
    mBackgroundDrawable.setAlwaysOnTop(false);
    addAndMakeVisible(mBackgroundDrawable);
    mBackgroundDrawable.setBufferedToImage(true);
    mBackgroundDrawable.setInterceptsMouseClicks(false, false);
    mBackgroundDrawable.setPaintingIsUnclipped(true);
    mBackgroundDrawable.setRepaintsOnMouseActivity(false);
    
    
    mMainPanel.setTopLeftPosition(0, mTopPanel.getBottom());
    addAndMakeVisible(&mMainPanel);
    
    mMainModPanel.setTopLeftPosition(0, mMainPanel.getBottom());
    addAndMakeVisible(&mMainModPanel);
    
    mMainModPanel.getMenuPanel().addButtonListener(this, kButton_Expand);
    mMainModPanel.getMenuPanel().addButtonListener(this, kButton_ModulationTabs);
    
    mTopBounds = mBackgroundDrawable.getBoundsInParent().toFloat();
    mPainted = false;
   // setBufferedToImage(true);

    mUnlockForm.setSize(EDITOR_PANEL_WIDTH/2, EDITOR_PANEL_HEIGHT/2);

    mUnlockForm.setTopLeftPosition(EDITOR_PANEL_WIDTH/4, EDITOR_PANEL_HEIGHT/4);
    
    addChildComponent(mUnlockForm);
    
}

DelayTrollAudioProcessorEditor::~DelayTrollAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    mMainModPanel.getMenuPanel().removeButtonListener(this, kButton_Expand);
    mMainModPanel.getMenuPanel().removeButtonListener(this, kButton_ModulationTabs);
}

//==============================================================================
void DelayTrollAudioProcessorEditor::paint (Graphics& g)
{
 
}

void DelayTrollAudioProcessorEditor::resized()
{

}
void DelayTrollAudioProcessorEditor::buttonClicked(Button* b)
{
    
    if(auto* p = dynamic_cast<DTButton*>(b)){
        if(!(p->getToggleState()))
        {
            setSize(EDITOR_PANEL_WIDTH, EDITOR_PANEL_EXP_HEIGHT);
            mMainModPanel.setSize(MAIN_MOD_PANEL_WIDTH, MAIN_MOD_PANEL_HEIGHT);
            p->setToggleState(true, dontSendNotification);
            p->setButtonText("v");
        }
        else{
            setSize(EDITOR_PANEL_WIDTH, EDITOR_PANEL_HEIGHT);
            mMainModPanel.setSize(MAIN_MOD_PANEL_WIDTH, MAIN_MOD_MENU_PANEL_HEIGHT);
            p->setToggleState(false, dontSendNotification);
            p->setButtonText("<-");
        }
    }
    
}
