/*
  ==============================================================================

    DTConstraintSections.h
    Created: 12 Aug 2020 3:27:12pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

/**Class representing a section of constrained values intended for use as part of an array but I wont tell you how to live your best life*/
class DTConstraintSection {
public:
    
    DTConstraintSection(int _index, Range<float> _sectionRange, Range<int> _indexRange):
    mIndex(_index),
    mSectionRange(_sectionRange),
    mIndexRange(_indexRange)
    {
        //DBG("Constraint section: "<<mIndex<<" with size of: "<< static_cast<float>(mIndexRange.getLength()));
        if(_index == 0)
        {
            mSpacer = mSectionRange.getLength()/static_cast<float>(mIndexRange.getLength());
        }
        else{
            mSpacer = mSectionRange.getLength()/static_cast<float>(mIndexRange.getLength());
        }
        //DBG("and a spacer of: "<<mSpacer);
    };
    
    DTConstraintSection(){};
    
    ~DTConstraintSection(){};
    
    /**returns an array of values between 0 and 1 that can be used to scale whatever is being mapped to this section of constrained values*/
    Array<float> getMultipliers(){
        mMults.clear();
        //DBG("Constraint Section: "<<mIndex);
        for(int i=0; i<mIndexRange.getLength(); i++){
            
            if(mIndex != 0){
                mMults.add( mSectionRange.getStart() + (mSpacer * static_cast<float>(i + 1)));
            }
            else
            {
                mMults.add( mSectionRange.getStart() + (mSpacer * static_cast<float>( i)));
            }
            //DBG("Position of: "<<i<<" at: "<< mMults[i]);
        }
        
        if(mIndex == 0)
        {
            mMults.add(mSectionRange.getStart() + (mSpacer * static_cast<float>(mIndexRange.getLength())));
            //DBG("Position of extra: "<<mIndexRange.getLength()<<" at: "<< mMults[mIndexRange.getLength()]);
        }
        
        return mMults;
    };
    
    void updateRange(Range<float> _sectionRange){
        mSectionRange = _sectionRange;
        
        mSpacer = mSectionRange.getLength()/static_cast<float>(mIndexRange.getLength());
    };
    
    
    float getRangeStart(){return mSectionRange.getStart();};
    float getRangeEnd(){return mSectionRange.getEnd();};
    int getIndexStart(){return mIndexRange.getStart();};
    int getIndexEnd(){return mIndexRange.getEnd();};
    void setIndexEnd(int _newIndexEnd){
        mIndexRange.setEnd(_newIndexEnd);
        mSpacer = mSectionRange.getLength()/static_cast<float>(mIndexRange.getLength());
    };
    
    /**gets an XMLElement representing the state of the section*/
    XmlElement* getXMLElement(){
        String sectionNumberString = "Section";
        sectionNumberString += mIndex;
        
        StringRef sectionNumberStringRef = sectionNumberString;
        XmlElement* sectionElement = new XmlElement(sectionNumberStringRef);
        
        Identifier indexId = "indexID";
        Identifier spacerID = "spacerID";
        Identifier sectionRangeStartID = "sectionRangeStartID";
        Identifier sectionRangeEndID = "sectionRangeEndID";
        Identifier indexRangeStartID = "indexRangeStartID";
        Identifier indexRangeEndID = "indexRangeEndID";

        
        sectionElement->setAttribute(indexId, mIndex);
        sectionElement->setAttribute(spacerID, mSpacer);
        sectionElement->setAttribute(sectionRangeStartID, mSectionRange.getStart());
        sectionElement->setAttribute(sectionRangeEndID, mSectionRange.getEnd());
        sectionElement->setAttribute(indexRangeStartID, mIndexRange.getStart());
        sectionElement->setAttribute(indexRangeEndID, mIndexRange.getEnd());
         
         return sectionElement;
    };
    
    /**sets the parameters of the section based on the provided XMLElement*/
    void setXMLElement(XmlElement* _xmlElement){
        
        mIndex = _xmlElement->getIntAttribute("indexID");
        mSpacer = static_cast<float>(_xmlElement->getDoubleAttribute("spacerID"));
        mSectionRange.setStart(static_cast<float>(_xmlElement->getDoubleAttribute("sectionRangeStartID")));
        mSectionRange.setEnd(static_cast<float>(_xmlElement->getDoubleAttribute("sectionRangeEndID")));
        mIndexRange.setStart(_xmlElement->getIntAttribute("indexRangeStartID"));
        mIndexRange.setEnd(_xmlElement->getIntAttribute("indexRangeEndID"));
    };

private:
    
    int mIndex;
    Range<float> mSectionRange;
    Range<int> mIndexRange;
    float mSpacer;
    Array<float> mMults;
   
};
