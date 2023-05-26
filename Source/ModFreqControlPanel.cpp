/*
  ==============================================================================

    ModFreqControlPanel.cpp
    Created: 20 Sep 2020 10:36:04am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ModFreqControlPanel.h"
#include "DTHelperFunctions.h"
#include "DTModulationDefines.h"
#include "DTUsedParameters.h"

//==============================================================================
ModFreqControlPanel::ModFreqControlPanel(DelayTrollAudioProcessor& p, ModManagment& _inManager):
PanelBase(p),
mModManagerRef(_inManager),
mBPMManagerRef(_inManager.getBPMManager()),
mGearImage(ImageCache::getFromMemory(BinaryData::GearE_png, BinaryData::GearE_pngSize)),
mFreqLableImage(ImageCache::getFromMemory(BinaryData::FreqLable_png, BinaryData::FreqLable_pngSize)),
mHertzSlider(DTDefualtHzRange, 1.0, "Hertz", mGearImage),
mHertzMode(ImageCache::getFromMemory(BinaryData::SwitchBOff_png, BinaryData::SwitchBOff_pngSize)),
mSyncMode(ImageCache::getFromMemory(BinaryData::SwitchBOn_png, BinaryData::SwitchBOn_pngSize)),
mBPMSyncMode("Sync", mSyncMode, mHertzMode),
mQuantDropDownImage(ImageCache::getFromMemory(BinaryData::DropDownC_png, BinaryData::DropDownC_pngSize)),
mQuantizationDropDown("QuantDrop", mQuantDropDownImage),
mQuantGlImage(ImageCache::getFromMemory(BinaryData::DropDownCGl_png, BinaryData::DropDownCGl_pngSize)),
mBarFractionsRef(mBPMManagerRef.getNoteFractions()),
mHzLabel(" Hz", mHertzSlider.getValueObject()),
mBPMSyncLabel(mBPMManagerRef.mBPMLabelInfo),
mSmallDisplayImage(ImageCache::getFromMemory(BinaryData::SmallDisplay4_png, BinaryData::SmallDisplay4_pngSize)),
mSmallDisplayGlImage(ImageCache::getFromMemory(BinaryData::SmallDisplay4Gl_png, BinaryData::SmallDisplay4Gl_pngSize))
{

    setSize(MODULATION_FREQ_CONTROL_WIDTH, MODULATION_FREQ_CONTROL_HEIGHT);
    
    Array<Rectangle<int>> Quarters = DTsetSubBox(getLocalBounds(), 2, 2);

    mBPMSyncMode.setToggleState(mModManagerRef.getBPMSyncStatus(), NotificationType::dontSendNotification);
    mBPMSyncMode.setClickingTogglesState(true);
    //mBPMSyncMode.setBounds(Quarters[0]);
    mBPMSyncMode.setCentrePosition(Quarters[0].getCentre().translated(5,4));
    
    
    mBPMSyncMode.onClick = [this](){
        bool isSyncing = mBPMSyncMode.getToggleState();
        
        mModManagerRef.setBPMSyncMode(isSyncing);
        mQuantizationDropDown.setEnabled(isSyncing);
        mBPMSyncLabel.repaint();
        mBPMSyncLabel.setVisible(isSyncing);
        mHzLabel.setVisible(!isSyncing);
        
        //set a new normalisable range for the Hertz slider
        NormalisableRange<double> newHzRange = isSyncing ?  NormalisableRange<double>(1.0, static_cast<double>(mBPMManagerRef.getMaxNoteFraction()), 1.0) :
                                                                                            DTDefualtHzRange;
        mHertzSlider.setNormalisableRange(newHzRange);
        
        //get the correct value to store in the hertz slider
        if(isSyncing){
            mHertzSlider.setValue(static_cast<double>(mBPMManagerRef.getNoteFractions().getValue()));
            mModManagerRef.updateStepSizeByNoteFraction();
        }
        else{
            mHertzSlider.setValue(DT_ms_to_HZ(static_cast<float>(mBPMManagerRef.getQuantTime().getValue())), dontSendNotification);
            mModManagerRef.updateStepSize(DT_Hz_to_ms(mHertzSlider.getValue()));
        }
    };//bpmsync on click
    addAndMakeVisible(&mBPMSyncMode);
    //addAndMakeVisible(&mSyncSwitch);
    
    //mQuantizationDropDown.setBounds(Quarters[1]);
    mQuantizationDropDown.setCentrePosition(Quarters[1].getCentre());
    mQuantizationDropDown.addItemList(DTBPMQuantizationOptions, 1);
    mQuantizationDropDown.setSelectedId(mBPMManagerRef.getBPMInfo().mQuantizationSelectionID);
    mQuantizationDropDown.setEnabled(mModManagerRef.getBPMSyncStatus());
    
    mQuantizationDropDown.onChange = [this](){
        mBPMManagerRef.updateQuantizationMult(mQuantizationDropDown.getSelectedId());
        
        //protection for very low BPMs
        if(mBPMManagerRef.getNoteFractions().getValue() < var(1.f) )
        {
            mBPMManagerRef.setNoteFractions(1.f);
        }
        
        //check the range of our note fractions as Hz and make sure they fall between 0.1 and 5
        while(mBPMManagerRef.getNoteFractionsAsHz() >= 5.f && mBPMManagerRef.getNoteFractions().getValue() < mBPMManagerRef.getMaxNoteFraction())
        {
            mBPMManagerRef.setNoteFractions(static_cast<float>(mBPMManagerRef.getNoteFractions().getValue()) + 1.f);
        }
        
        while(mBPMManagerRef.getNoteFractionsAsHz() <= 0.1f && mBPMManagerRef.getNoteFractions().getValue() > var(1.f))
        {
            mBPMManagerRef.setNoteFractions(static_cast<float>(mBPMManagerRef.getNoteFractions().getValue()) - 1.f);
        }
        //set a new normalizable range for the Hz slider and set it to the new note fraction value
        if(mModManagerRef.getBPMSyncStatus()){
            mHertzSlider.setNormalisableRange(NormalisableRange<double>(1.0, static_cast<double>(mBPMManagerRef.getMaxNoteFraction()), 1.0));
            mModManagerRef.updateStepSizeByNoteFraction();
        }
        
        if(mModManagerRef.getBPMSyncStatus())
            mHertzSlider.setValue(static_cast<double>(mBPMManagerRef.getNoteFractions().getValue()), dontSendNotification);

        mBPMSyncLabel.repaint();
    };// Quant on Change
    
    addAndMakeVisible(&mQuantizationDropDown);
    
    mQuantGl.setImage(mQuantGlImage);
    mQuantGl.setSize(mQuantGlImage.getWidth(), mQuantGlImage.getHeight());
    mQuantGl.setCentrePosition(mQuantizationDropDown.getBounds().getCentre());
    mQuantGl.setBoundingBox(mQuantGl.getBounds().toFloat());
    addAndMakeVisible(&mQuantGl);
    
    NormalisableRange<double> newHzRange = mModManagerRef.getBPMSyncStatus() ? NormalisableRange<double>(1.0, static_cast<double>(mBPMManagerRef.getMaxNoteFraction()), 1.0) :
    DTDefualtHzRange;
    
    mFreqLable.setImage(mFreqLableImage);
    mFreqLable.setSize(mFreqLableImage.getWidth()+2, mFreqLableImage.getHeight()+2);
    mFreqLable.setCentrePosition(Quarters[2].getCentre().translated(0,-3));
    mFreqLable.setBoundingBox(mFreqLable.getBounds().toFloat());
    addAndMakeVisible(&mFreqLable);
    
    mHertzSlider.setNormalisableRange(newHzRange);
    
    mHertzSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mHertzSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    //mHertzSlider.setBounds(Quarters[2]);
    mHertzSlider.setCentrePosition(Quarters[2].getCentre());
    
    
    mSmallDisplay.setSize(mSmallDisplayImage.getWidth(), mSmallDisplayImage.getHeight());
    mSmallDisplay.setImage(mSmallDisplayImage);
    mSmallDisplay.setCentrePosition(Quarters[3].getCentre());
    mSmallDisplay.setBoundingBox(mSmallDisplay.getBounds().toFloat());
    addAndMakeVisible(&mSmallDisplay);
    
    mHzLabel.setVisible(!mModManagerRef.getBPMSyncStatus());
    mHzLabel.setSignificanDigits(2);
    //mHzLabel.setBounds(Quarters[3]);
    mHzLabel.setBounds(mSmallDisplay.getBoundsInParent());
    addChildComponent(&mHzLabel);

    mBPMSyncLabel.setVisible(mModManagerRef.getBPMSyncStatus());
    mBPMSyncLabel.setBounds(mHzLabel.getBounds());

    addChildComponent(&mBPMSyncLabel);
    
    mSmallDisplayGl.setSize(mSmallDisplayGlImage.getWidth(), mSmallDisplayGlImage.getHeight());
    mSmallDisplayGl.setImage(mSmallDisplayGlImage);
    mSmallDisplayGl.setCentrePosition(Quarters[3].getCentre());
    mSmallDisplayGl.setBoundingBox(mSmallDisplayGl.getBounds().toFloat());
    addAndMakeVisible(&mSmallDisplayGl);
    
    addAndMakeVisible(&mHertzSlider);
    
    if(mModManagerRef.getBPMSyncStatus())
        mHertzSlider.setValue(static_cast<double>(mBPMManagerRef.getNoteFractions().getValue()), dontSendNotification);
    else
        mHertzSlider.setValue(DT_ms_to_HZ(mModManagerRef.getStepSizeAsMsPerCycle()), dontSendNotification);
        
    
    mHertzSlider.onValueChange = [this](){
        
        if(mBPMSyncMode.getToggleState()){
            
            mBPMManagerRef.setNoteFractions(static_cast<double>(mHertzSlider.getValue()));
            //check the range of our note fractions as Hz and make sure they fall between 0.1 and 5
            while(mBPMManagerRef.getNoteFractionsAsHz() >= 5.f && mBPMManagerRef.getNoteFractions().getValue() < mBPMManagerRef.getMaxNoteFraction())
            {
                mBPMManagerRef.setNoteFractions(static_cast<float>(mBPMManagerRef.getNoteFractions().getValue()) + 1.f);
            }

            while(mBPMManagerRef.getNoteFractionsAsHz() <= 0.1f && mBPMManagerRef.getNoteFractions().getValue() > var(1.f))
            {
                mBPMManagerRef.setNoteFractions(static_cast<float>(mBPMManagerRef.getNoteFractions().getValue()) - 1.f);
            }
            
            mHertzSlider.setValue(static_cast<double>(mBPMManagerRef.getNoteFractions().getValue()), dontSendNotification);
            mModManagerRef.updateStepSizeByNoteFraction();
            mBPMSyncLabel.repaint();
        }
        else{
            mModManagerRef.updateStepSize(DT_Hz_to_ms(mHertzSlider.getValue()));
            mHzLabel.repaint();
        }
        
    };//Hz value change
    
    
}

ModFreqControlPanel::~ModFreqControlPanel()
{
}

void ModFreqControlPanel::paint (juce::Graphics& g)
{

    //g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

}

void ModFreqControlPanel::resized()
{

}
