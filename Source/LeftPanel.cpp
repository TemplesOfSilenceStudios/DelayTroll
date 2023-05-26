/*
  ==============================================================================

    LeftPanel.cpp
    Created: 23 Apr 2019 9:11:46am
    Author:  Sam Ferguson

  ==============================================================================
*/


#include "LeftPanel.h"
#include "DTUsedParameters.h"
#include "DTHelperFunctions.h"
#include "DTModulationDefines.h"
//==============================================================================
LeftPanel::LeftPanel(DelayTrollAudioProcessor& p):
PanelBase(p),
mNumOfDelsImage(ImageCache::getFromMemory(BinaryData::GearA1_png, BinaryData::GearA1_pngSize)),
mFeedbackImage(ImageCache::getFromMemory(BinaryData::GearA2_png, BinaryData::GearA2_pngSize)),
mNumOfDels(*p.getNonModParams()[kNMod_NumbOfDels], NormalisableRange<double>(2.0, 64.0, 1.0),  DTNonModParamNames[kNMod_NumbOfDels], mNumOfDelsImage),
mFeedback(mProcessor.tree, DTParameterID[kParam_Feedback], DTParameterName[kParam_Feedback], mFeedbackImage),
mFeedbackLabel("%", mFeedback.getValueObject(), 100.0),
mNumOfDelsLabel("", mNumOfDels.getValueObject())
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize(LEFT_PANEL_WIDTH, LEFT_PANEL_HEIGHT);
    
    Array<Rectangle<int>> subBoxes = DTsetSubBox(getLocalBounds(), 1, 2);
    
    Array<Rectangle<int>> numOfDelsBoxs = DTsetSubBox(subBoxes[0], 1, 5);
    mNumOfDelsLabel.setBounds(numOfDelsBoxs[4].reduced(20, 5).translated(0, 5));
    mNumOfDelsLabel.setJustificationType(Justification::centred);
    mNumOfDelsLabel.setEditable(false);
    
    Rectangle<int> numOfDelsBox;
    numOfDelsBox.setBounds(numOfDelsBoxs[0].getX(), numOfDelsBoxs[0].getY(), numOfDelsBoxs[0].getWidth(), numOfDelsBoxs[0].getHeight() * 4);
    //mNumOfDels.setBounds(DTRemoveLabelBumper(subBoxes[0]).removeFromLeft(getWidth()-23));
    mNumOfDels.setCentrePosition(subBoxes[0].getCentre().translated(0, 10));
    mNumOfDels.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mNumOfDels.setDoubleClickReturnValue(false, 32.0);
    
    mNumOfDels.onDragStart = [this](){mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);};
    mNumOfDels.onDragEnd = [this](){mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);};
    
    mNumOfDels.onValueChange = [this](){
        mAttString.clear();
        //mPrefix = "test";
        //mAttString.append(mPrefix, mFont,  DTColourIds::PopUpText);
        mAttString.append(String(roundToInt(mNumOfDels.getValue())), mFont,  DTColourIds::PopUpText);
        mBubble.showAt(&mNumOfDels, mAttString, 150);
        
        //user is not dragging
        if(!mNumOfDels.isMouseButtonDown())
            return;
        
        //user is loading preset
        if(mProcessor.getLoadingStatus().load(std::memory_order_acquire))
            return;
        
        //DBG("Slider detecting Change: "<< mNumOfDels.getValue()<<" =================================================");
        int numberofdels = static_cast<int>(mNumOfDels.getValue());
        DTDSP::AtomicTimeProcessor& atomicProcessor = mProcessor.getAtomicTimeProcessor();
        
        DTWaitForUpdate(mProcessor.getAtomicPauseStatus(), mProcessor.getAtomicUpdateStatus());
        int selection = mProcessor.getNonModParamAsInt(kNMod_TimeControlMode);
        
        //check what time control mode is selected and some special cases
        if(selection == kTimeOp_QuantizeDelays &&
           numberofdels > mProcessor.getAtomicHord().getHordNoteFractionRang().getLength() + 1){
            //capps the number of delays to the available number of quantized slots
            mNumOfDels.setValue(static_cast<double>(mProcessor.getAtomicHord().getHordNoteFractionRang().getLength() + 1), dontSendNotification);
            numberofdels = mProcessor.getAtomicHord().getHordNoteFractionRang().getLength() + 1;
        }
        else if (selection == kTimeOp_Divisitions &&
                 atomicProcessor.getConstraintSections().getLast()->getIndexStart() + 1 >= numberofdels){
            //makes sure that the number of delays doesnt remove a delay that is currently tied to a constriant line
            numberofdels = atomicProcessor.getConstraintSections().getLast()->getIndexStart() + 2;
            mNumOfDels.setValue(static_cast<double>(numberofdels), dontSendNotification);
        }
        else{
            mNumOfDels.setValue(static_cast<double>(numberofdels), dontSendNotification);
        }
        
        if(numberofdels != atomicProcessor.getMults().size()){
            atomicProcessor.resizeMult(numberofdels);
        }
        else{
            mProcessor.getAtomicPauseStatus().store(false, std::memory_order_release);
        }
        
        //DBG("SLIDER END=====================================================================");
        mNumOfDelsLabel.repaint();
    };
    
    Array<Rectangle<int>> feedbackBoxs = DTsetSubBox(subBoxes[1], 1, 5);
    mFeedbackLabel.setBounds(feedbackBoxs[4].reduced(20, 5).translated(0, 5));
    mFeedbackLabel.setJustificationType(Justification::centred);
    mFeedbackLabel.setEditable(false);
    
    Rectangle<int> feedbackSliderBox;
    feedbackSliderBox.setBounds(feedbackBoxs[0].getX(), feedbackBoxs[0].getY(), feedbackBoxs[0].getWidth(), feedbackBoxs[0].getHeight() * 4);
    //mFeedback.setBounds(DTRemoveLabelBumper(subBoxes[1]));
    mFeedback.setCentrePosition(subBoxes[1].getCentre().translated(-2, 8));
    mFeedback.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mFeedback.setDoubleClickReturnValue(false, 0.0);
    mFeedback.onDragStart = [this](){mProcessor.getGUIMovingStatus().store(true, std::memory_order_release);
        mFeedback.startedDragging();
    };
    mFeedback.onDragEnd = [this](){mProcessor.getGUIMovingStatus().store(false, std::memory_order_release);
        mFeedback.startedDragging();
    };
    
    
    mFeedback.onValueChange = [this](){
        mAttString.clear();
        mAttString.append(String(roundToInt(mFeedback.getValue()*100.0)), mFont,  DTColourIds::PopUpText);
        mSuffix = "%";
        mAttString.append(mSuffix, mFont,  DTColourIds::PopUpText);
        mBubble.showAt(&mFeedback, mAttString, 150);
        
        if(!mFeedback.isMouseOverOrDragging())
            return;
        
        if(!mProcessor.getAssignableParameters()[kAss_Feedback]->isAttached)
            mProcessor.getAssignableParameters()[kAss_Feedback]->setValue(mFeedback.getValue());
        else
            mProcessor.updateModulations();
    };

    mFeedback.setModulatableParameter(true, p.getAssignableParameters()[kAss_Feedback], &p);
    

 
    addAndMakeVisible(&mNumOfDels);
    addAndMakeVisible(&mFeedback);
    
    mProcessor.getAtomicTimeProcessor().addListener(this);
    
    mBubble.setColour(BubbleMessageComponent::ColourIds::backgroundColourId, DTColourIds::PopUpBG);
    mBubble.setColour(BubbleMessageComponent::ColourIds::outlineColourId, DTColourIds::PopUpText);
    mFont = Font("Vilane", 40.00f, Font::bold);
    addChildComponent(&mBubble);
    
}

LeftPanel::~LeftPanel()
{
    mProcessor.getAtomicTimeProcessor().removeListener(this);
 
}

void LeftPanel::paint (Graphics& g)
{
    //g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    //g.setColour (Colours::grey);
   // g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

