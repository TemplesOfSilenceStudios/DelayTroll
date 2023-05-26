/*
  ==============================================================================

    AtomicTroll.h
    Created: 27 Aug 2020 9:00:13am
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

namespace TrollParams{
/**indexies of parameters specific to each troll*/
enum TrollAttributes{
    kTroll_DelayTime = 0,
    kTroll_Amplitude,
    kTroll_HighPassFreq,
    kTroll_LowPassFreq,
    kTroll_PitchFactor,
    kTroll_LeftAtenuation,
    kTroll_RightAtenuation,
    kTroll_TotalNumOfAttributes
};

static float DefaultTrollAttributeValues[kTroll_TotalNumOfAttributes]{
    0.f,                            //    kTroll_DelayTime = 0,
    0.f,                            //    kTroll_Amplitude,
    20000.f,                        //    kTroll_HighPassFreq,
    20.f,                           //    kTroll_LowPassFreq,
    1.f,                            //    kTroll_PitchFactor,
    1.f,                            //    kTroll_LeftAtenuation,
    1.f                             //    kTroll_RightAtenuation,
};
/**custom converter function for the pitch normalizable range*/
static float PitchConvertFrom(float start, float end, float mapValue){return mapValue >=0.5f?((mapValue-0.5f) * 2.f)+1.f:mapValue + 0.5f;};
/**custom converter function for the pitch normalizable range*/
static float PitchConvertTo(float start, float end, float mapValue){return mapValue >=1.f?(mapValue-1.f)/2.f+0.5f:mapValue-0.5f;};

/**normal ranges to help convert between 0 to 1 values, right now these are for GUI, may need a new when I start measuring amplidude in DB*/
static NormalisableRange<float> TrollAttributeNormalRanges[kTroll_TotalNumOfAttributes]{
    NormalisableRange<float>(0.f, 10000.0f),                                    //time
    NormalisableRange<float>(0.f, 1.f),                                         //amp
    NormalisableRange<float>(20.f, 20000.f, 0.f, 0.195834f),                    //high pass
    NormalisableRange<float>(20.f, 20000.f, 0.f, 0.195834f),                    //lowPass
    NormalisableRange<float>(0.5f, 2.f, PitchConvertFrom, PitchConvertTo),      //pitch
    NormalisableRange<float>(0.5f, 1.f),                                        //left pan
    NormalisableRange<float>(0.f, 0.5f)                                         //Right pan
};

}
/**class representing a signle delay and all the information assosiated with it*/
class AtomicTroll {
    
public:
    /**class representing a signle delay and all the information assosiated with it*/
    AtomicTroll(Array<float> _CurrentValues):
    mIndex(0),
    mNewTarget(0.f),
    mMult(-1.f),
    mDBScalar(pow(10.f,0.6f))
    {
        jassert(_CurrentValues.size() == TrollParams::kTroll_TotalNumOfAttributes);
        for(int i = 0; i < TrollParams::kTroll_TotalNumOfAttributes; i++)
        {
            mAttributes[i] = _CurrentValues[i];
            //mAtomicAttributes[i].set(_CurrentValues[i]);
            setTrollTarget(i, _CurrentValues[i]);
            mGUIAttributes[i] = _CurrentValues[i];
        }
    };
    
    ~AtomicTroll(){};
    


    /**updates the troll attribues to the target attributes, should only be called by processor during runtime*/
    inline void updateToTargets()
    {
        for ( mIndex = 0; mIndex < TrollParams::kTroll_TotalNumOfAttributes; mIndex ++) {
            mNewTarget = mAtomicAttributes[mIndex].get();
            if(mNewTarget != mAttributes[mIndex])
            {
                mAttributes[mIndex] = mNewTarget;
            }
        }
    };
    
    /**sets a new target for the atribue at the provided index*/
    inline void setTrollTarget(int _index, float _newValue){
        switch (_index) {
            case TrollParams::kTroll_DelayTime:
                mAtomicAttributes[_index].set(_newValue);
                break;
            case TrollParams::kTroll_Amplitude:
                mAtomicAttributes[_index].set(pow(mDBScalar, log2(_newValue)));
                //DBG(mIndex <<": withLevel: "<<mAtomicAttributes[_index].get());
                break;
            case TrollParams::kTroll_HighPassFreq:
                mAtomicAttributes[_index].set(_newValue);
                break;
            case TrollParams::kTroll_LowPassFreq:
                mAtomicAttributes[_index].set(_newValue);
                break;
            case TrollParams::kTroll_PitchFactor:
                mAtomicAttributes[_index].set(_newValue);
                break;
            case TrollParams::kTroll_LeftAtenuation:
                mAtomicAttributes[_index].set(pow(mDBScalar, log2(_newValue)));
                //DBG(mIndex<<": withPan: "<<mAtomicAttributes[_index].get());
                break;
            case TrollParams::kTroll_RightAtenuation:
                mAtomicAttributes[_index].set(pow(mDBScalar, log2(_newValue)));
                break;
                
            default:
                break;
        }
    };
    
    /**directly sets a GUI attribute*/
    inline void setTrollGUITarget(int _index, float _newValue){mGUIAttributes[_index] = _newValue;};
    
    /**returns the trolls position as a % of the way between min and max times*/
    inline float& getMult(){return mMult;};

    //current values
    float mAttributes[TrollParams::kTroll_TotalNumOfAttributes];
    float mGUIAttributes[TrollParams::kTroll_TotalNumOfAttributes];
    
    /**gets an xml element based off the current attribute set*/
    inline XmlElement* getTrollXMLElement(int _index){
        String trollID = "trollID";
        trollID += _index;
        StringRef trollStringRef = trollID;
        XmlElement* trollElement = new XmlElement(trollStringRef);
        
        for(int i = 0; i<TrollParams::kTroll_TotalNumOfAttributes; i++)
        {
            Identifier attributeID = String("attributeID" + String(i));
            trollElement->setAttribute(attributeID, mGUIAttributes[i]);
        }
        
        return trollElement;
    };
    
    /**sets the attributes of based on the provided XMLElement*/
    inline void setTrollXMLElement(XmlElement* _xmlElement){
        for (int i = 0; i<_xmlElement->getNumAttributes(); i++) {
            //this is an odd way to do this I wonder why???
            String tester = _xmlElement->getAttributeName(i).upToLastOccurrenceOf(StringRef("D"), true, false);
            if(tester == "attributeID")
            {
                setTrollGUITarget(i, static_cast<float>(_xmlElement->getDoubleAttribute(_xmlElement->getAttributeName(i))));
                setTrollTarget(i, static_cast<float>(_xmlElement->getDoubleAttribute(_xmlElement->getAttributeName(i))));
            }
        }
    };
    
    
private:
    
    int mIndex;
    float mNewTarget;
    float mMult;
    NormalisableRange<float> mMultRange;
    const float mDBScalar;
    
    //targetValues
    Atomic<float> mAtomicAttributes[TrollParams::kTroll_TotalNumOfAttributes];
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AtomicTroll)
};
