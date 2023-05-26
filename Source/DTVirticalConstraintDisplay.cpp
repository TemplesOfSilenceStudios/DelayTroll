/*
  ==============================================================================

    DTVirticalConstraintDisplay.cpp
    Created: 31 Aug 2020 3:03:26pm
    Author:  Sam Ferguson

  ==============================================================================
*/

//#include <JuceHeader.h>
#include "DTVirticalConstraintDisplay.h"
#include "DTInterfaceDefines.h"
#include "DTHelperFunctions.h"

//==============================================================================
DTVirticalConstraintDisplay::DTVirticalConstraintDisplay(DelayTrollAudioProcessor* _p):
mProcessor(*_p),
mTimeProcessor(_p->getAtomicTimeProcessor()),
mConstraintSectionsRef(_p->getAtomicTimeProcessor().getConstraintSections()),
mMultsRef(_p->getAtomicTimeProcessor().getMults()),
mPause(mProcessor.getAtomicPauseStatus()),
mUpdate(mProcessor.getAtomicUpdateStatus())
{
    mLineSelection = -1;
    setSize(TROLL_DISPLAY_COMPONENT_WIDTH, TROLL_DISPLAY_COMPONENT_HEIGHT);
    mScaleDown = AffineTransform::scale(1.f/static_cast<float>(TROLL_DISPLAY_COMPONENT_WIDTH), 1.f/static_cast<float>(TROLL_DISPLAY_COMPONENT_HEIGHT));
    mScaleUp = AffineTransform::scale(static_cast<float>(TROLL_DISPLAY_COMPONENT_WIDTH), static_cast<float>(TROLL_DISPLAY_COMPONENT_HEIGHT));
    
    mLines.clear();
    
    //sets the lines off of the sections
    for(int i = 1; i < mConstraintSectionsRef.size(); i++)
    {
        Line<float> newLine(mConstraintSectionsRef[i]->getRangeStart(),
                            static_cast<float>(getLocalBounds().getBottom()),
                            mConstraintSectionsRef[i]->getRangeStart(),
                            static_cast<float>(getLocalBounds().getY()));
        
        newLine.applyTransform(mScaleUp);
        
        mLines.set(mConstraintSectionsRef[i]->getIndexStart(),
                   Line<float>(newLine));
    }
    
    mTimeProcessor.addListener(this);
}

DTVirticalConstraintDisplay::~DTVirticalConstraintDisplay()
{
    mTimeProcessor.removeListener(this);
}

void DTVirticalConstraintDisplay::paint (juce::Graphics& g)
{
    //g.setColour(DTColourIds::LrgDisplayDimGlow);
    g.setColour(DTColourIds::PopUpBG);
    //g.setColour(Colours::black);
    g.setOpacity(0.75f);
    for(auto line: mLines)
    {
        g.drawLine(line, 2.f);
    }
    g.setOpacity(1.f);
}


void DTVirticalConstraintDisplay::mouseDown(const MouseEvent& e){
    if(e.getNumberOfClicks() >= 2){
        return;
    }
    
    Point<float> mousepoint(e.getPosition().toFloat());

    mLineSelection = -1;
    
    //check to see what lines the user is selecting, using the key to match the delay index
    auto it = mLines.begin();
    do {
        if(mousepoint.getDistanceFrom(Point<float>(it.getValue().getEndX(), mousepoint.getY())) <= 10.f){
            mLineSelection = it.getKey();
            return;;
        }
    } while (it.next());
    
    mousepoint.applyTransform(mScaleDown);
    
    int trollLineIndex = 0;
    Point<float> trollPoint;
    float closestpoint = 10000.f;
    //if no line was selected then we need to see what delay index we want to make the new constriant line at
    for (auto mult : mMultsRef) {
        trollPoint = Point<float>(mult.second, mousepoint.getY());
        
        if(trollPoint.getDistanceFrom(mousepoint) < closestpoint){
            closestpoint = trollPoint.getDistanceFrom(mousepoint);
            trollLineIndex = mult.first;
        }
    }
    
    if(trollLineIndex != 0 && trollLineIndex != mMultsRef.size() - 1){
        //now the line selection is the new index and we make a new line and set it
         mLineSelection = trollLineIndex;
         Line<float> newLine(mMultsRef[trollLineIndex].second, 1.f, mMultsRef[trollLineIndex].second, 0.f);
         newLine.applyTransform(mScaleUp);
         mLines.set(trollLineIndex, newLine);
         
         int sectionIndex = 0;
         int lastKey = 0;
         float lastX = 0.f;
         auto newit = mLines.begin();
         
        //clear the contstraint sections and rebuild the sections to acomedate the new line
         DTWaitForUpdate(mPause, mUpdate);
         mConstraintSectionsRef.clear();
         do {
             mConstraintSectionsRef.add(new DTConstraintSection(sectionIndex,
                                                                Range<float>(lastX, mMultsRef[newit.getKey()].second),
                                                                Range<int>(lastKey, newit.getKey())));
             
             lastKey = newit.getKey();
             lastX = mMultsRef[newit.getKey()].second;
             sectionIndex++;
         } while (newit.next());
         mConstraintSectionsRef.add(new DTConstraintSection(sectionIndex,
                                                            Range<float>(lastX, 1.f),
                                                            Range<int>(lastKey, mMultsRef.size() - 1)));
        mTimeProcessor.updateMult();
        mCurrentMousePoint = e.getPosition().toFloat();
        // startTimer(50);
     }
}
/**changes a points hight */
void DTVirticalConstraintDisplay::mouseDrag(const MouseEvent& e){
//    if(e.getNumberOfClicks() >= 2){
//        return;
//    }
    mCurrentMousePoint = e.getPosition().toFloat();
    if(mLineSelection < 0 ||
       !getLocalBounds().contains(e.getPosition()) ||
       e.getNumberOfClicks() >= 2 ||
       mConstraintSectionsRef.size() != mLines.size()+1 ||
       e.getLengthOfMousePress() < 10)
     {
         return;
     }
     
     Line<float> currentLine = mLines[mLineSelection];
     
     float startY = currentLine.getStartY();
     float endY = currentLine.getEndY();
     
     
     
     float newX = mCurrentMousePoint.getX();
     float minX = 0.f;
     float maxX = 0.f;
     
     auto it = mLines.begin();
     //find the previouse line
     while (it.getKey() < mLineSelection) {
         minX = it.getValue().getEndX()+1.f;
         
         if(!it.next()){
             minX = 0.f;
             break;
         }
     }
     
     it.reset();
     //go through the line selections till you get the current line
     while (it.getKey() <= mLineSelection) {
         //increment the itterator or set max to right if you cant
         if(!it.next()){
             maxX = static_cast<float>(getRight());
             break;
         }
         //set the max value, should be the line that is next in the list right after the mLineSelection as we incremented the itterator with it.next()
         maxX = it.getValue().getEndX();
     }
     //set a range ang clip the new x value
     Range<float> clipper(minX,maxX);
     
     newX = clipper.clipValue(newX);
     //set the line
     mLines.set(mLineSelection, Line<float>(newX, startY, newX, endY));
     //updates the constraint sections
     auto newit = mLines.begin();
     int sectionIndex = 0;
     float lastX = 0.f;
     DTWaitForUpdate(mPause, mUpdate);
    
     do {
         Line<float> templine = newit.getValue();
         templine.applyTransform(mScaleDown);
         mConstraintSectionsRef[sectionIndex]->updateRange(Range<float>(lastX, templine.getEndX()));
         lastX = templine.getEndX();
         sectionIndex++;
     } while (newit.next());
    //jassert(mConstraintSectionsRef.size() - 1 == sectionIndex);
     mConstraintSectionsRef[mConstraintSectionsRef.size() - 1]->updateRange(Range<float>(lastX, 1.f));
     mTimeProcessor.updateMult();
     
     repaint();
}

void DTVirticalConstraintDisplay::timerCallback(){
    if(mLineSelection < 0)
    {
        return;
    }
    //timer ment to repalace the mouse dragging
    Line<float> currentLine = mLines[mLineSelection];
    
    float startY = currentLine.getStartY();
    float endY = currentLine.getEndY();

    float newX = mCurrentMousePoint.getX();
    float minX = 0.f;
    float maxX = 0.f;
    
    auto it = mLines.begin();
    
    while (it.getKey() < mLineSelection) {
        minX = it.getValue().getEndX()+1.f;
        
        if(!it.next()){
            minX = 0.f;
            break;
        }
    }
    
    it.reset();
    
    while (it.getKey() <= mLineSelection) {
        if(!it.next()){
            maxX = static_cast<float>(getRight());
            break;
        }
        maxX = it.getValue().getEndX();
    }
    
    Range<float> clipper(minX,maxX);
    
    newX = clipper.clipValue(newX);
    
    mLines.set(mLineSelection, Line<float>(newX, startY, newX, endY));
    
    auto newit = mLines.begin();
    int sectionIndex = 0;
    float lastX = 0.f;
    DTWaitForUpdate(mPause, mUpdate);
   
    do {
        Line<float> templine = newit.getValue();
        templine.applyTransform(mScaleDown);
        mConstraintSectionsRef[sectionIndex]->updateRange(Range<float>(lastX, templine.getEndX()));
        lastX = templine.getEndX();
        sectionIndex++;
    } while (newit.next());
    
    mConstraintSectionsRef[sectionIndex]->updateRange(Range<float>(lastX, 1.f));
    mTimeProcessor.updateMult();
    
    repaint();
}

void DTVirticalConstraintDisplay::mouseDoubleClick(const MouseEvent& e){
    if(mLineSelection != -1)
    {
        //DBG("removing Line");
        if(mLines.size() <= 1){
            return;
        }
        //removes a line
        mLines.remove(mLineSelection);
        //updates the constraint section
        auto newit = mLines.begin();
        int sectionIndex = 0;
        int lastKey = 0;
        float lastX = 0.f;
        
        DTWaitForUpdate(mPause, mUpdate);
        mConstraintSectionsRef.clear();
        do {
            mConstraintSectionsRef.add(new DTConstraintSection(sectionIndex, Range<float>(lastX, mMultsRef[newit.getKey()].second),Range<int>(lastKey, newit.getKey()) ));
            
            lastKey = newit.getKey();
            lastX = mMultsRef[newit.getKey()].second;
            sectionIndex++;
        } while (newit.next());
        
        mConstraintSectionsRef.add(new DTConstraintSection(sectionIndex, Range<float>(lastX, 1.f), Range<int>(lastKey, mMultsRef.size() - 1)));
        
        mTimeProcessor.updateMult();
        repaint();
    }
}

void DTVirticalConstraintDisplay::mouseUp(const MouseEvent& e){
    //stopTimer();
    //one last drag for good measure
    mouseDrag(e);
}
