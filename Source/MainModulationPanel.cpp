/*
  ==============================================================================

    MainModulationPanel.cpp
    Created: 22 Apr 2019 4:48:07pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#include "MainModulationPanel.h"
#include "DTModulationDefines.h"
//==============================================================================
MainModulationPanel::MainModulationPanel(DelayTrollAudioProcessor& p)
:   PanelBase(p),
    mModMenuPanel(p)
{

    setSize(MAIN_MOD_PANEL_WIDTH, MAIN_MOD_MENU_PANEL_HEIGHT);
    
    mSelectedPlot = DTDisplayValueIDs[kDisVal_PlotEditingIndex];
    
    Adder = 0;
    
    mModMenuPanel.setTopLeftPosition(0, 0);
    
    for(int i = 0 ; i < kButton_TotalNumButtons; i++){
        mModMenuPanel.addButtonListener(this, i);
    }
    
    addAndMakeVisible(&mModMenuPanel);
    
    mBackgroundImage = ImageCache::getFromMemory(BinaryData::bottomHalf_png, BinaryData::bottomHalf_pngSize);
    
    mBackgroundDrawable.setImage(mBackgroundImage);
    //mBackgroundDrawable.setBounds(getLocalBounds());
    //mBackgroundDrawable.setBoundingBox(mTopBounds);
    mBackgroundDrawable.setBounds(0, mModMenuPanel.getHeight(), getWidth(), MODULATION_PANEL_HEIGHT);
 
    mBackgroundDrawable.setAlwaysOnTop(false);
    //addAndMakeVisible(mBackgroundDrawable);
    addChildComponent(&mBackgroundDrawable);
    mBackgroundDrawable.setBufferedToImage(true);
    mBackgroundDrawable.setInterceptsMouseClicks(false, false);
    mBackgroundDrawable.setPaintingIsUnclipped(true);
    mBackgroundDrawable.setRepaintsOnMouseActivity(false);
    
    
    //mAssignPanel.setTopLeftPosition(0, mModMenuPanel.getBottom());
    //addAndMakeVisible(&mAssignPanel);
    disValID = DTDisplayValueIDs[kDisVal_bezierEditingIndex];
    
    mViewPort.setBounds(0, mModMenuPanel.getBottom(), MODULATION_ASSIGN_PANEL_WIDTH, MODULATION_ASSIGN_PANEL_HEIGHT);
    
    mViewPort.setViewedComponent(new AssignPanel(p,
                                                     *p.getModulationArray()[p.getDisplayValues()[mSelectedPlot]],
                                                     mViewPort.getMaximumVisibleWidth()-mViewPort.getScrollBarThickness(),
                                                     MODULATION_ASSIGN_PANEL_HEIGHT));
    
    mViewPort.setScrollBarPosition(true, false);
    
    addAndMakeVisible(&mViewPort);
    
    
    mModPanelPtrs.add(std::make_unique<BezierModulationPanel>(mProcessor));
    mModPanelPtrs.add(std::make_unique<LFOModulationPanel>(mProcessor));
    
    
    for(auto* p : mModPanelPtrs)
    {
        p->setTopLeftPosition(mViewPort.getRight(), mModMenuPanel.getBottom());
        addChildComponent(p);
    }
    
    selectedModulation = kModPanel_Bezier;

    mModPanelPtrs[selectedModulation]->setVisible(true);
    
    mProcessor.addListener(this);
}

MainModulationPanel::~MainModulationPanel()
{
    for(int i = 0 ; i < kButton_TotalNumButtons; i++){
        mModMenuPanel.removeButtonListener(this, i);
    }
    mProcessor.removeListener(this);
}

void MainModulationPanel::paint (Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void MainModulationPanel::resized()
{

}

void MainModulationPanel::buttonClicked (Button* b)
{
    //****************COMBINE ME WITH IF STATEMENT BELOW***********************
    NamedValueSet& displayVal = mProcessor.getDisplayValues();

    if(b->getName() ==DTButtonNames[kButton_Expand])
    {
        selectedModulation = mModMenuPanel.getSelectedModulation();
        switch (selectedModulation) {
            case kModPanel_Bezier:
                disValID = DTDisplayValueIDs[kDisVal_bezierEditingIndex];
                Adder = 0;
                break;

            case kModPanel_LFO:
                disValID = DTDisplayValueIDs[kDisVal_LFOEditingIndex];
                Adder = DTnumOfMods;
                break;

            default:
                break;
                
        }
        int plotNumber = displayVal[disValID];
        mModMenuPanel.updatePlotNumber(plotNumber + 1);
        
        //========
        for(auto * mp: mModPanelPtrs)
        {
            mp->setVisible(false);
            mp->setAlwaysOnTop(false);
        }
        displayVal.set(mSelectedPlot, static_cast<int>(displayVal[disValID]) + Adder);
        removeChildComponent(mModPanelPtrs[selectedModulation]);
        if (selectedModulation == kModPanel_Bezier) {
            mModPanelPtrs.set(selectedModulation, std::make_unique<BezierModulationPanel>(mProcessor), true);
        } else {
            mModPanelPtrs.set(selectedModulation, std::make_unique<LFOModulationPanel>(mProcessor), true);
        }
        mModPanelPtrs[selectedModulation]->setTopLeftPosition(mViewPort.getRight(), mModMenuPanel.getBottom());
        addAndMakeVisible(mModPanelPtrs[selectedModulation]);
        
        mModPanelPtrs[selectedModulation]->setAlwaysOnTop(true);
        mViewPort.setViewedComponent(new AssignPanel(mProcessor,
                                                         *mProcessor.getModulationArray()[plotNumber + Adder],
                                                         mViewPort.getMaximumVisibleWidth()-mViewPort.getScrollBarThickness(),
                                                         mViewPort.getMaximumVisibleHeight()));
        
        mBackgroundDrawable.setBounds(0, mModMenuPanel.getBottom(), getWidth(), MODULATION_PANEL_HEIGHT);
        mBackgroundDrawable.setVisible(true);
    }
    
    //handle a change in the type of modulation panel the user wants to edit
    if(auto* p = dynamic_cast<TabBarButton*>(b))
    {
        selectedModulation = p->getTabbedButtonBar().getCurrentTabIndex();
        switch (selectedModulation) {
            case kModPanel_Bezier:
                disValID = DTDisplayValueIDs[kDisVal_bezierEditingIndex];
                Adder = 0;
                break;

            case kModPanel_LFO:
                disValID = DTDisplayValueIDs[kDisVal_LFOEditingIndex];
                Adder = DTnumOfMods;
                break;

            default:
                break;
        }
        mBackgroundDrawable.setBounds(0, mModMenuPanel.getBottom(), getWidth(), MODULATION_PANEL_HEIGHT);
        mBackgroundDrawable.setVisible(true);
    }
    
    //handles a change in the plot number that the user wants to edit
    if(auto* p = dynamic_cast<TabBarButton*>(b))
    {
        int plotNumber = displayVal[disValID];
        mModMenuPanel.updatePlotNumber(plotNumber + 1);
        
        //========
        for(auto * mp: mModPanelPtrs)
        {
            mp->setVisible(false);
            mp->setAlwaysOnTop(false);
        }
        displayVal.set(mSelectedPlot, static_cast<int>(displayVal[disValID]) + Adder);
        removeChildComponent(mModPanelPtrs[selectedModulation]);
        if (selectedModulation == kModPanel_Bezier) {
            mModPanelPtrs.set(selectedModulation, std::make_unique<BezierModulationPanel>(mProcessor), true);
        } else {
            mModPanelPtrs.set(selectedModulation, std::make_unique<LFOModulationPanel>(mProcessor), true);
        }
        mModPanelPtrs[selectedModulation]->setTopLeftPosition(mViewPort.getRight(), mModMenuPanel.getBottom());
        addAndMakeVisible(mModPanelPtrs[selectedModulation]);
        
        mModPanelPtrs[selectedModulation]->setAlwaysOnTop(true);
        mViewPort.setViewedComponent(new AssignPanel(mProcessor,
                                                         *mProcessor.getModulationArray()[plotNumber + Adder],
                                                         mViewPort.getMaximumVisibleWidth()-mViewPort.getScrollBarThickness(),
                                                         mViewPort.getMaximumVisibleHeight()));
        //========
        
    }
    else if(b->getName() == DTButtonNames[kButton_Left])
    {
        int plotNumber = displayVal[disValID];
        if(plotNumber >= 1){
            plotNumber -= 1;
            mModMenuPanel.updatePlotNumber(plotNumber + 1);
            displayVal.set(disValID, plotNumber);
            //DBG("Dsiplay Value: "<< displayVal[disValID].toString());
            
            mViewPort.setViewedComponent(new AssignPanel(mProcessor,
                                                             *mProcessor.getModulationArray()[static_cast<int>(displayVal[disValID]) + Adder],
                                                             mViewPort.getMaximumVisibleWidth()-mViewPort.getScrollBarThickness(),
                                                             mViewPort.getMaximumVisibleHeight()));
            
            //===================
            displayVal.set(mSelectedPlot, static_cast<int>(displayVal[disValID]) + Adder);
            removeChildComponent(mModPanelPtrs[selectedModulation]);
            if (selectedModulation == kModPanel_Bezier) {
                mModPanelPtrs.set(selectedModulation, std::make_unique<BezierModulationPanel>(mProcessor), true);
            } else {
                mModPanelPtrs.set(selectedModulation, std::make_unique<LFOModulationPanel>(mProcessor), true);
            }
            mModPanelPtrs[selectedModulation]->setTopLeftPosition(mViewPort.getRight(), mModMenuPanel.getBottom());
            addAndMakeVisible(mModPanelPtrs[selectedModulation]);
            //===================
            
        }
    }
    else if(b->getName() == DTButtonNames[kButton_Right])
    {
        int plotNumber = displayVal[disValID];
        if(plotNumber < 9){
            plotNumber += 1;
            mModMenuPanel.updatePlotNumber(plotNumber + 1);
            displayVal.set(disValID, plotNumber);
            //DBG("Dsiplay Value: "<< displayVal[disValID].toString());
             mViewPort.setViewedComponent(new AssignPanel(mProcessor,
                                                              *mProcessor.getModulationArray()[plotNumber + Adder],
                                                              mViewPort.getMaximumVisibleWidth()-mViewPort.getScrollBarThickness(),
                                                              mViewPort.getMaximumVisibleHeight()));
            
            //===================
            displayVal.set(mSelectedPlot, plotNumber + Adder);
            removeChildComponent(mModPanelPtrs[selectedModulation]);
            if (selectedModulation == kModPanel_Bezier) {
                mModPanelPtrs.set(selectedModulation, std::make_unique<BezierModulationPanel>(mProcessor), true);
            } else {
                mModPanelPtrs.set(selectedModulation, std::make_unique<LFOModulationPanel>(mProcessor), true);
            }
            mModPanelPtrs[selectedModulation]->setTopLeftPosition(mViewPort.getRight(), mModMenuPanel.getBottom());
            addAndMakeVisible(mModPanelPtrs[selectedModulation]);
            //===================
        }
    }
}
void MainModulationPanel::loadActivated(DelayTrollAudioProcessor* p)
{
    if(&mProcessor == p)
    {
        
        NamedValueSet& displayVal = mProcessor.getDisplayValues();
        int plotNumber = displayVal[disValID];
        mViewPort.setViewedComponent(new AssignPanel(mProcessor,
                                                         *mProcessor.getModulationArray()[plotNumber + Adder],
                                                         mViewPort.getMaximumVisibleWidth()-mViewPort.getScrollBarThickness(),
                                                         mViewPort.getMaximumVisibleHeight()));
        //===================
        for(auto * mp: mModPanelPtrs)
        {
            mp->setVisible(false);
            mp->setAlwaysOnTop(false);
        }
        displayVal.set(mSelectedPlot, plotNumber + Adder);
        removeChildComponent(mModPanelPtrs[selectedModulation]);
        if (selectedModulation == kModPanel_Bezier) {
            mModPanelPtrs.set(selectedModulation, std::make_unique<BezierModulationPanel>(mProcessor), true);
        } else {
            mModPanelPtrs.set(selectedModulation, std::make_unique<LFOModulationPanel>(mProcessor), true);
        }
        mModPanelPtrs[selectedModulation]->setTopLeftPosition(mViewPort.getRight(), mModMenuPanel.getBottom());
        mModPanelPtrs[selectedModulation]->setAlwaysOnTop(true);
        addAndMakeVisible(mModPanelPtrs[selectedModulation]);
        //===================
        
    }
}
