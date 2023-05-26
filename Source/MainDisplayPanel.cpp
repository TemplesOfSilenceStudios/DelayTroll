/*
  ==============================================================================

    MainDisplayPanel.cpp
    Created: 23 Apr 2019 9:13:25am
    Author:  Sam Ferguson

  ==============================================================================
*/


#include "MainDisplayPanel.h"
#include "DTUsedParameters.h"

//==============================================================================
MainDisplayPanel::MainDisplayPanel(DelayTrollAudioProcessor& p):
PanelBase(p),
mDisplayBGImage(ImageCache::getFromMemory(BinaryData::LargeDisplay1_png, BinaryData::LargeDisplay1_pngSize)),
mDisplayGlImage(ImageCache::getFromMemory(BinaryData::LargeDisplay1Gl_png, BinaryData::LargeDisplay1Gl_pngSize)),
mMenu(p),
mDisplayValues(p.getDisplayValues()),
mControlPanel(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    mProcessor.addListener(this);
    setSize(MAIN_DISPLAY_PANEL_WIDTH, MAIN_DISPLAY_PANEL_HEIGHT);
    
    mDisplayBG.setImage(mDisplayBGImage);
    mDisplayBG.setSize(mDisplayBGImage.getWidth(), mDisplayBGImage.getHeight());
    mDisplayBG.setCentrePosition(getWidth()/2, (getHeight()/2)-30);
    mDisplayBG.setBoundingBox(mDisplayBG.getBounds().toFloat());
    addAndMakeVisible(&mDisplayBG);
    
    mMenu.setTopLeftPosition(0, 0);
    mMenu.addChangelistener(this);
    addAndMakeVisible(&mMenu);
    
    //must be kept in sync with DTInterfaceDefines.h's DTDisplayPanels
    mDisplayPanels.add(new TrollTimeDisplay(p, mControlPanel));     //kDisPanel_Time
    mDisplayPanels.add(new TrollLevelDisplay(p, mControlPanel));    //kDisPanel_Level
    mDisplayPanels.add(new TrollEQDisplay(p, mControlPanel));       //kDisPanel_EQ
    mDisplayPanels.add(new TrollPitchDisplay(p, mControlPanel));    //kDisPannel_Pitch
    mDisplayPanels.add(new TrollPanDisplay(p, mControlPanel));      //kDisPannel_Pan
    
    for(auto *p : mDisplayPanels)
    {
        p->setTopLeftPosition(0, mMenu.getBottom());
        addChildComponent(p);
    }

    mDisplayPanels[mDisplayValues[DTDisplayValueIDs[kDIsVal_DisplayPanelSelection]]]->setVisible(true);
    
    mControlPanel.setTopLeftPosition(0, mMenu.getBottom() + mDisplayPanels[0]->getHeight());
    addAndMakeVisible(&mControlPanel);
    
    mDisplayGl.setImage(mDisplayGlImage);
    mDisplayGl.setSize(mDisplayGlImage.getWidth(), mDisplayGlImage.getHeight());
    mDisplayGl.setCentrePosition((getWidth()/2)+2, (getHeight()/2)-29);
//    mDisplayGl.setTopLeftPosition(mDisplayBG.getX()+2,mDisplayBG.getY()+1);
    mDisplayGl.setBoundingBox(mDisplayGl.getBounds().toFloat());
    mDisplayGl.setInterceptsMouseClicks(false, false);
    mDisplayGl.setAlwaysOnTop(true);
    addAndMakeVisible(&mDisplayGl);

    //mProcessor.addChangeListener(this);
}

MainDisplayPanel::~MainDisplayPanel()
{
    mMenu.removeChangelistener(this);
    mProcessor.removeListener(this);
    //mProcessor.removeChangeListener(this);
    
}

void MainDisplayPanel::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if(auto *p = dynamic_cast<DTTabbedButtonBar*>(source))
    {
        int timeControlMode = mProcessor.getNonModParamAsInt(kNMod_TimeControlMode);
        for(auto *dp : mDisplayPanels)
        {
            dp->setVisible(false);
            dp->setAlwaysOnTop(false);
            dp->setQuantEnabled(mProcessor.getSyncStatus());
            //dumb way to solve problem is it even nessisary any more?
            if(auto *timedisplay = dynamic_cast<TrollTimeDisplay*>(dp)){
                timedisplay->setConstraintVisable(timeControlMode == kTimeOp_Divisitions);
            }
        }
        
        mDisplayPanels[p->getCurrentTabIndex()]->setVisible(true);
        mDisplayValues.set(DTDisplayValueIDs[kDIsVal_DisplayPanelSelection], p->getCurrentTabIndex());
        mDisplayPanels[p->getCurrentTabIndex()]->getDisplayComponent().setTabSelection(p->getCurrentTabIndex());

    }//Tabbed button bar check
//    else if (auto *mPro = dynamic_cast<DelayTrollAudioProcessor*>(source)){
//        DBG("PROCESSOR CHANGE DETECTED IN MAIN DISPLAY PANEL");
//    }//processor check
}

void MainDisplayPanel::loadActivated(DelayTrollAudioProcessor* _p)
{
    if(_p == &mProcessor){
        //time tab
        int controlMode = mProcessor.getNonModParamAsInt(kNMod_TimeControlMode);
        mDisplayPanels[kDisPanel_Time]->setComponentVisability(controlMode);
        //level tab
        controlMode = mProcessor.getNonModParamAsInt(kNMod_LevelManuelMode);
        mDisplayPanels[kDisPanel_Level]->setComponentVisability(controlMode);
        //eq tab
        controlMode = mProcessor.getNonModParamAsInt(kNMod_EQManuelMode);
        mDisplayPanels[kDisPanel_EQ]->setComponentVisability(controlMode);
        //pitch tab
        controlMode = mProcessor.getNonModParamAsInt(kNMod_PitchManuelMode);
        mDisplayPanels[kDisPannel_Pitch]->setComponentVisability(controlMode);
        //pan tab
        controlMode = mProcessor.getNonModParamAsInt(kNMod_PanManuelMode);
        mDisplayPanels[kDisPannel_Pan]->setComponentVisability(controlMode);
        
        
    }
}

void MainDisplayPanel::setDisplayTimer(bool _shouldTimerRun,  int _timerTime)
{
    if(_shouldTimerRun){
        mDisplayPanels[mDisplayValues[DTDisplayValueIDs[kDIsVal_DisplayPanelSelection]]]->startTimer(_timerTime);
    }
    else{
        mDisplayPanels[mDisplayValues[DTDisplayValueIDs[kDIsVal_DisplayPanelSelection]]]->stopTimer();
        mDisplayPanels[mDisplayValues[DTDisplayValueIDs[kDIsVal_DisplayPanelSelection]]]->repaint();
    }
}
