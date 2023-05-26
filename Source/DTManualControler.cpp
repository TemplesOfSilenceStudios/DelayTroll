/*
  ==============================================================================

    DTManualControler.cpp
    Created: 1 Sep 2020 9:28:05am
    Author:  Sam Ferguson

  ==============================================================================
*/

#include <JuceHeader.h>

#include "DTUsedParameters.h"
#include "DTInterfaceDefines.h"
#include "DTHelperFunctions.h"
#include "DTManualControler.h"
#include "TrollPanDisplay.h"

using namespace DTManualControl;
//==============================================================================
DTManualControler::DTManualControler(DelayTrollAudioProcessor* _p, int _topControlParamIndex, DTManualModes _mode, int _bottomControlParamIndex):
mProcessorPtr(_p),
mTimeProcessor(_p->getAtomicTimeProcessor()),
mMults(mTimeProcessor.getMults()),
mQuantSlots(mTimeProcessor.getQuants()),
mPausedRef(_p->getAtomicPauseStatus()),
mUpdateRef(_p->getAtomicUpdateStatus()),
mMode(_mode),
mTopControlParamIndex(_topControlParamIndex),
mBottomControlParamIndex(_bottomControlParamIndex),
mAtomicTrollRef(_p->getAtomicTrolls()),
mAtomicHordRef(_p->getAtomicHord())
{
    mLineSelection = -1;
    mTopSelected = true;
    setSize(TROLL_DISPLAY_COMPONENT_WIDTH, TROLL_DISPLAY_COMPONENT_HEIGHT);
    mScaleDown = AffineTransform::scale(1.f/static_cast<float>(TROLL_DISPLAY_COMPONENT_WIDTH), 1.f/static_cast<float>(TROLL_DISPLAY_COMPONENT_HEIGHT));
    mScaleUp = AffineTransform::scale(static_cast<float>(TROLL_DISPLAY_COMPONENT_WIDTH), static_cast<float>(TROLL_DISPLAY_COMPONENT_HEIGHT));
    
    mAttString.clear();
    mAttString.append("Test", mFont, DTColourIds::PopUpText);
    
    mBubble.setColour(BubbleMessageComponent::ColourIds::backgroundColourId, DTColourIds::PopUpBG);
    mBubble.setColour(BubbleMessageComponent::ColourIds::outlineColourId, DTColourIds::PopUpText);
    mFont = Font("Courier New", 12.00f, Font::bold);
    addChildComponent(&mBubble);
    
}

DTManualControler::~DTManualControler()
{
}

void DTManualControler::paint(juce::Graphics& g){
    
}

void DTManualControler::mouseDown(const MouseEvent& e){
    if(e.getNumberOfClicks() >= 2){
        return;
    }

    
    Point<float> mousepoint(e.getPosition().toFloat());
    
    mousepoint.applyTransform(mScaleDown);
    
    mLineSelection = -1;
    
    //find the line that the user is selecting
    //DBG("Mouse down atomic manual control");
    for (auto mult : mMults) {
        if(mousepoint.getDistanceFrom(Point<float>(mult.second, mousepoint.getY())) <= 0.02f)
        {
            mLineSelection = mult.first;
            //DBG("point selected: "<< mLineSelection);
        }
    }
    
    if(mLineSelection == -1){return;}
    
    //set weather the user is selecting the top or bottom of a line
    if(mMode == kManMode_TopAndBottom){
        float top = mAtomicTrollRef[mLineSelection]->mGUIAttributes[mTopControlParamIndex];
        float bottom = mAtomicTrollRef[mLineSelection]->mGUIAttributes[mBottomControlParamIndex];
//        DBG("TOP AND BOTTOM GUI VALUES: "<< top<<" : "<<bottom);
        top = TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].convertTo0to1(top);
        bottom = TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].convertTo0to1(bottom);
//        DBG("TOP AND BOTTOM CONVERTED: "<< top<<" : "<<bottom);
        top = 1.f - top;
        bottom = 1.f - bottom;
//        DBG("TOP AND BOTTOM FLIPPED: "<< top<<" : "<<bottom);
//        DBG("MOUSEPOINT Y: "<<mousepoint.getY());
        float distanceToTop = mousepoint.getDistanceFrom(Point<float>(mousepoint.getX(), top));
        float distanceToBottom = mousepoint.getDistanceFrom(Point<float>(mousepoint.getX(), bottom));
//        DBG("TOP AND BOTTOM DISTANCES: "<< distanceToTop<<" : "<<distanceToBottom);
         mTopSelected = distanceToTop < distanceToBottom ? true:false;
//        if(mTopSelected)
//            DBG("TOP IS SELECTED");
//        else
//            DBG("BOTTOM IS SLECTED");
    }
    else if (mMode == kManMode_Split){
        mTopSelected = mousepoint.getY() < 0.5f ? true:false;
    }
    static_cast<TrollDisplayPanelBase*>(getParentComponent())->startTimer(20);
}
/**changes a points hight or x */
void DTManualControler::mouseDrag(const MouseEvent& e){
    if(!getLocalBounds().contains(e.getPosition()) || e.getNumberOfClicks() >= 2 || mLineSelection == -1){
        return;
    }
    //DBG("mouse Drage Begin");
    Point<float> newpoint = e.getPosition().toFloat();
    newpoint.applyTransform(mScaleDown);
    float value=-1.f;
    switch (mMode) {
        case  kManMode_TopOnly:{
            //just get the invers of the mouse position and use the normalrange to convert it to the proper value
            value = TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].convertFrom0to1(1.f-newpoint.getY());
            DTWaitForUpdate(mPausedRef, mUpdateRef);
            mAtomicTrollRef[mLineSelection]->setTrollTarget(mTopControlParamIndex, value);
            mAtomicTrollRef[mLineSelection]->setTrollGUITarget(mTopControlParamIndex, value);
            mPausedRef.store(false, std::memory_order_release);
        }
            break;
            
        case  kManMode_TopAndBottom:{
            //convert the oposite parameter to a value between 0-1 and use that to constrain the useres dragging value
            if(mTopSelected){
                float bottom = mAtomicTrollRef[mLineSelection]->mGUIAttributes[mBottomControlParamIndex];
                bottom = TrollParams::TrollAttributeNormalRanges[mBottomControlParamIndex].convertTo0to1(bottom);
                newpoint.setY(jmax(bottom, 1.f-newpoint.getY()));
                value = TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].convertFrom0to1(newpoint.getY());
                DTWaitForUpdate(mPausedRef, mUpdateRef);
                mAtomicTrollRef[mLineSelection]->setTrollTarget(mTopControlParamIndex, value);
                mAtomicTrollRef[mLineSelection]->setTrollGUITarget(mTopControlParamIndex, value);
//                if(mTopControlParamIndex == TrollParams::kTroll_LowPassFreq){
//                    DBG("Lowpass change");
//                }
                mPausedRef.store(false, std::memory_order_release);
            }
            else{
                float top = mAtomicTrollRef[mLineSelection]->mGUIAttributes[mTopControlParamIndex];
                top = TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].convertTo0to1(top);
                newpoint.setY(jmin(top, 1.f-newpoint.getY()));
                value = TrollParams::TrollAttributeNormalRanges[mBottomControlParamIndex].convertFrom0to1(newpoint.getY());
                DTWaitForUpdate(mPausedRef, mUpdateRef);
                mAtomicTrollRef[mLineSelection]->setTrollTarget(mBottomControlParamIndex, value);
                mAtomicTrollRef[mLineSelection]->setTrollGUITarget(mBottomControlParamIndex, value);
//                if(mTopControlParamIndex == TrollParams::kTroll_HighPassFreq){
//                    DBG("Highpass change");
//                }
                mPausedRef.store(false, std::memory_order_release);
            }}
            break;
        case  kManMode_Split:
            // rescale things from a 0-1 to a 0-0.5 and use the centre as the constraint
            if(mTopSelected){
                value = newpoint.getY() < 0.5f ? newpoint.getY():0.5f;
                value = 0.5f-value;
                value = TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].convertTo0to1(0.5f + value);
                DTWaitForUpdate(mPausedRef, mUpdateRef);
                mAtomicTrollRef[mLineSelection]->setTrollTarget(mTopControlParamIndex, value);
                mAtomicTrollRef[mLineSelection]->setTrollGUITarget(mTopControlParamIndex, value);
            }
            else{
                value = newpoint.getY() > 0.5f ? newpoint.getY():0.5f;
                value = value-0.5f;
                value = TrollParams::TrollAttributeNormalRanges[mBottomControlParamIndex].convertTo0to1(value);
                DTWaitForUpdate(mPausedRef, mUpdateRef);
                mAtomicTrollRef[mLineSelection]->setTrollTarget(mBottomControlParamIndex, value);
                mAtomicTrollRef[mLineSelection]->setTrollGUITarget(mBottomControlParamIndex, value);
            }
             mPausedRef.store(false, std::memory_order_release);
            //**********remove this Later, I think it can just go**************
            static_cast<TrollDisplayPanelBase*>(getParentComponent())->timerCallback();
            break;
        case  kManMode_Xeditable:{
            if(mLineSelection >= mMults.size() - 1 || mLineSelection <= 0){
                 return;
            }
            DTWaitForUpdate(mPausedRef, mUpdateRef);
            //get the current delay
            auto& thisMult = mMults.getReference(mLineSelection);
            //get some points representing the position of the previous and next delay and mage a range
            Point<float> beforPoint(mMults[mLineSelection-1].second, newpoint.getY());
            Point<float> nextPoint(mMults[mLineSelection+1].second, newpoint.getY());
            Range<float> innerRange(beforPoint.getX() + 0.01f, nextPoint.getX() - 0.01f);
            //clip the mouse position so it falls between the surrounding delays
            newpoint.setX(innerRange.clipValue(newpoint.getX()));
            
            thisMult.second = newpoint.getX();
            mTimeProcessor.processCombo();
            mTimeProcessor.updateTimes();
            
            //bubble message stuff
            mAttString.clear();
            String data(trunc(mAtomicHordRef.getTrollTimeFromMult(thisMult.second)));
            mAttString.append(data, mFont, DTColourIds::PopUpText);
            mSuffix = " ms";
            mAttString.append(mSuffix, mFont,DTColourIds::PopUpText);
            mBubble.showAt(Rectangle<int>(e.getPosition(), e.getPosition().translated(1, 1)),mAttString, 200 );
        }
            break;
        case  kManMode_QuantizedX:{
            
            if(mLineSelection >= mMults.size() - 1 || mLineSelection <= 0){
                 return;
            }
            DTWaitForUpdate(mPausedRef, mUpdateRef);
            //get the current delay
            auto& thisMult = mMults.getReference(mLineSelection);
            int quantIndex = -1;
            //find the quant slot that corrisponds to this delay
            for (int i = 0; i<mQuantSlots.size(); i++) {
                if(approximatelyEqual(mQuantSlots[i].second, thisMult.second)){
                    quantIndex = i;
                    //DBG("quant Dragging: "<<i);
                }
            }
            //get the current and surrounding quant slots and create some points based off then
            auto& lastQuant = mQuantSlots.getReference(quantIndex-1);
            auto& thisQuant = mQuantSlots.getReference(quantIndex);
            auto& nextQuant = mQuantSlots.getReference(quantIndex+1);

            Point<float> beforPoint(lastQuant.second, newpoint.getY());
            Point<float> currentPoint(thisQuant.second, newpoint.getY());
            Point<float> nextPoint(nextQuant.second, newpoint.getY());
            
            //check if the mouse is closer to the current next of previous quant and weather that quant is empty (first value of -1)
            if(newpoint.getDistanceFrom(beforPoint) < newpoint.getDistanceFrom(currentPoint) && lastQuant.first == -1)
            {
                //DBG("moving to last quant");
                lastQuant.first = mLineSelection;
                thisQuant.first = -1;
                thisMult.second = lastQuant.second;
                mTimeProcessor.processCombo();
                mTimeProcessor.updateTimes();
            }
            else if (newpoint.getDistanceFrom(nextPoint)<newpoint.getDistanceFrom(currentPoint) && nextQuant.first == -1)
            {
                //DBG("moving to next quant");
                nextQuant.first = mLineSelection;
                thisQuant.first = -1;
                thisMult.second = nextQuant.second;
                mTimeProcessor.processCombo();
                mTimeProcessor.updateTimes();
            }
            else{
                mPausedRef.store(false, std::memory_order_release);
            }
            
            //bubble message stuff
            mAttString.clear();
           
            float notfraction = mAtomicHordRef.getTrollTimeFromMult(thisMult.second)/mProcessorPtr->getBPMManager(0).getNoteFractionsInMS();
            float denominatior = mProcessorPtr->getBPMManager(0).getBPMInfo().mQuantizationMult;
            float wholenotes =floor(notfraction/denominatior);
            float numerator = notfraction - wholenotes * denominatior;
            //String data(trunc(mAtomicHordRef.getTrollTimeFromMult(thisMult.second)));
            String data(String(roundToInt(wholenotes))+" "+ String(roundToInt(numerator)) + "/"+ String(roundToInt(denominatior)));
            mAttString.append(data, mFont, DTColourIds::PopUpText);
            //mSuffix = " ms";
            //mAttString.append(mSuffix, mFont,DTColourIds::PopUpText);
            mBubble.showAt(Rectangle<int>(e.getPosition(), e.getPosition().translated(1, 1)),mAttString, 200 );
            
        }
            break;
            
        case kManMode_QuantizedY:{
            
            value = TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].convertFrom0to1(1.f-newpoint.getY());
            
            float centre = TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].convertFrom0to1(0.5f);
            
            //create a normalizabler range and then use the stnap to legalvalue function to force it into one or the other
            if(value >= centre){
                NormalisableRange<float> tempRange(centre,
                                                   TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].end,
                                                   (TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].end - centre)/12.f);
                value = tempRange.snapToLegalValue(value);
            }
            else{
                float interval =(centre - TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].start)/12.f;
                NormalisableRange<float> tempRange(TrollParams::TrollAttributeNormalRanges[mTopControlParamIndex].start,
                                                   centre,
                                                   interval);
                value = tempRange.snapToLegalValue(value);
            }
            
            DTWaitForUpdate(mPausedRef, mUpdateRef);
            mAtomicTrollRef[mLineSelection]->setTrollTarget(mTopControlParamIndex, value);
            mAtomicTrollRef[mLineSelection]->setTrollGUITarget(mTopControlParamIndex, value);
            mPausedRef.store(false, std::memory_order_release);
        }
            break;
            
        case kManMode_InvertedSplit:{
            //this is for pan mode or something like that
            if(newpoint.getY() <= 0.5f){
                value= newpoint.getY() * 2.f;
                DTWaitForUpdate(mPausedRef, mUpdateRef);
                mAtomicTrollRef[mLineSelection]->setTrollTarget(mBottomControlParamIndex, value);
                mAtomicTrollRef[mLineSelection]->setTrollGUITarget(mBottomControlParamIndex, value);
            }
            else{
                value= (1.f - newpoint.getY()) * 2.f;
                DTWaitForUpdate(mPausedRef, mUpdateRef);
                mAtomicTrollRef[mLineSelection]->setTrollTarget(mTopControlParamIndex, value);
                mAtomicTrollRef[mLineSelection]->setTrollGUITarget(mTopControlParamIndex, value);
            }
            
            mPausedRef.store(false, std::memory_order_release);
            //***********surly i dont need this if by definition the timer is running
           static_cast<TrollDisplayPanelBase*>(getParentComponent())->timerCallback();
            
        }
            break;
            
        default:
            break;
    }
}
/**just resets the slection index to -1*/
void DTManualControler::mouseUp(const MouseEvent& e){
    //should probably have a callback hear for safty
    //static_cast<TrollDisplayPanelBase*>(getParentComponent())->timerCallback();
    
    static_cast<TrollDisplayPanelBase*>(getParentComponent())->stopTimer();
}
