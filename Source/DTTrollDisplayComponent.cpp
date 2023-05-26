/*
  ==============================================================================

    DTTrollDisplayComponent.cpp
    Created: 11 Aug 2020 11:44:56am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DTTrollDisplayComponent.h"
#include "DTInterfaceDefines.h"
#include "DTUsedParameters.h"
#include "TrollDisplayPanelBase.h"
#include "DTHelperFunctions.h"

//==============================================================================
DTTrollDisplayComponent::DTTrollDisplayComponent( OwnedArray<AtomicTroll>& _trollRef, DTDSP::AtomicTimeProcessor& _timeProcessor, DelayTrollAudioProcessor* _p):
PanelBase(*_p),
mAtomicTrollRefs(_trollRef),
mTimeMultRef(_timeProcessor.getMults()),
mTimeProcessor(_timeProcessor)
{
    setSize(TROLL_DISPLAY_COMPONENT_WIDTH, TROLL_DISPLAY_COMPONENT_HEIGHT);
    
    mScaleDown = AffineTransform::scale(1.f/static_cast<float>(TROLL_DISPLAY_COMPONENT_WIDTH), 1.f/static_cast<float>(TROLL_DISPLAY_COMPONENT_HEIGHT));
    mScaleUp = AffineTransform::scale(static_cast<float>(TROLL_DISPLAY_COMPONENT_WIDTH), static_cast<float>(TROLL_DISPLAY_COMPONENT_HEIGHT));

    mTranslateForward = AffineTransform::translation(3.f, 0.f);
    mTranslateBack = AffineTransform::translation(-3.f, 0.f);
    
    mIsSetting=false;
    mTabSelection = _p->getDisplayValues()[DTDisplayValueIDs[kDIsVal_DisplayPanelSelection]];
    ResetLines();
    mTimeProcessor.addListener(this);
    
    Array< Rectangle<int>> boxes = DTsetSubBox(getLocalBounds(), 5, 5);
    
    mTestLabel.setBounds(boxes[23]);
    mTestLabel.setJustificationType(Justification::bottomRight);
    mTestLabel.setColour(mTestLabel.textColourId, Colours::black);
    
    mTestLabel.setText("5132 ms", dontSendNotification);
    //addAndMakeVisible(mTestLabel);
}

DTTrollDisplayComponent::~DTTrollDisplayComponent()
{

    mTimeProcessor.removeListener(this);
}

void DTTrollDisplayComponent::paint (juce::Graphics& g)
{
    g.setColour(Colours::dimgrey);
    g.setOpacity(0.3f);
    for(auto* bgLine: mBackgroundLines){
        g.drawLine(*bgLine, 3.f);
    }

    //g.setColour(Colours::darkslategrey);
    g.setColour(DTColourIds::LrgDisplayGlow);
    //g.setOpacity(1.f);
    for (auto* line : mAtomicLines) {
        g.setOpacity(0.3f);
        g.drawLine(*line, 6.f);
        g.setOpacity(1.f);
        g.drawLine(*line, 3.f);
    }
}

void DTTrollDisplayComponent::setLines()
{
    //cycle though the delay indexes
    for (auto& mult : mTimeMultRef) {
        auto* line = mAtomicLines[mult.first];
        auto* bgLine = mBackgroundLines[mult.first];
        Point<float> start(mult.second, 1.f);
        Point<float> end(mult.second, 0.f);

        switch (mTabSelection) {
            case kDisPanel_Time:
                end.setY(1.f - mAtomicTrollRefs[mult.first]->mGUIAttributes[TrollParams::kTroll_Amplitude]);
                
                break;
                
            case kDisPanel_Level:
                end.setY(1.f - mAtomicTrollRefs[mult.first]->mGUIAttributes[TrollParams::kTroll_Amplitude]);
                break;
                
            case kDisPanel_EQ:{
                float lowFreq = mAtomicTrollRefs[mult.first]->mGUIAttributes[TrollParams::kTroll_LowPassFreq];
                float highFreq = mAtomicTrollRefs[mult.first]->mGUIAttributes[TrollParams::kTroll_HighPassFreq];
                end.setY(1.f - TrollParams::TrollAttributeNormalRanges[TrollParams::kTroll_LowPassFreq].convertTo0to1(lowFreq));
                start.setY(1.f - TrollParams::TrollAttributeNormalRanges[TrollParams::kTroll_HighPassFreq].convertTo0to1(highFreq));
            }
                break;
                
            case kDisPannel_Pitch:{
                float pitch = mAtomicTrollRefs[mult.first]->mGUIAttributes[TrollParams::kTroll_PitchFactor];
                start.setY(0.5f);
                end.setY(1.f - TrollParams::TrollAttributeNormalRanges[TrollParams::kTroll_PitchFactor].convertTo0to1(pitch));
            }
                break;
                
            case kDisPannel_Pan:{
                float Right = mAtomicTrollRefs[mult.first]->mGUIAttributes[TrollParams::kTroll_RightAtenuation];
                float Left = mAtomicTrollRefs[mult.first]->mGUIAttributes[TrollParams::kTroll_LeftAtenuation];
                if( mProcessor.getNonModParamAsInt(kNMod_PanManuelMode) == kPanOp_ManBal ||
                   mProcessor.getNonModParamAsInt(kNMod_PanManuelMode) == kPanOP_BezBal){
                    start.setY(0.5f);
                    if(Right > Left)
                        end.setY(0.5f + (1.f - Left)/2.f);
                    else if(approximatelyEqual(Right, Left))
                        end.setY(0.5f);
                    else
                        end.setY(0.5f - (1.f - Right)/2.f);
                }
                else{
                    end.setY(0.5f + TrollParams::TrollAttributeNormalRanges[TrollParams::kTroll_RightAtenuation].convertFrom0to1(Right));
                    start.setY(1.f - TrollParams::TrollAttributeNormalRanges[TrollParams::kTroll_LeftAtenuation].convertFrom0to1(Left));
                }
            }
                break;
                
            default:
                break;
        }//switch
        line->setStart(start);
        line->setEnd(end);
        line->applyTransform(mScaleUp);
        bgLine->setStart(line->getStartX(), getLocalBounds().toFloat().getHeight());
        bgLine->setEnd(line->getStartX(), 0.f);
        
        if(line->getStartX() <= 2.f)
        {
            line->applyTransform(mTranslateForward);
            bgLine->applyTransform(mTranslateForward);
        }
        else if (line->getStartX() >= getLocalBounds().toFloat().getWidth() - 2.f)
        {
            line->applyTransform(mTranslateBack);
            bgLine->applyTransform(mTranslateBack);
        }
        
    }//loop trough delays
    repaint();
    
}

void DTTrollDisplayComponent::ResetLines()
{
    while (mTimeMultRef.size() < mAtomicLines.size()) {
        mAtomicLines.removeLast();
        mBackgroundLines.removeLast();
    }
    
    while (mTimeMultRef.size() > mAtomicLines.size()) {
        mAtomicLines.add(new Line<float>(0.f,0.f,0.f,0.f));
        mBackgroundLines.add(new Line<float>(0.f,0.f,0.f,0.f));
    }
    
    jassert(mBackgroundLines.size() == mAtomicLines.size());
    
    setLines();
    
}

void DTTrollDisplayComponent::atomicMultUpdated(DTDSP::AtomicTimeProcessor* _p){
    ResetLines();
}
