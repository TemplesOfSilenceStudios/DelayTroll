/*
  ==============================================================================

    DTPresetManager.h
    Created: 7 Apr 2019 4:08:39pm
    Author:  Sam Ferguson

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#define PRESET_FILE_EXTENTION ".dtf"

class DTPresetManager
{
public:
    DTPresetManager(AudioProcessor* inProcessor);
    ~DTPresetManager();
    
    static const String mUserPresetDirectory;
    
    /** saves the current parameters to an xml file */
    void getXmlForPreset(XmlElement* inElement, OwnedArray<Value>& _nonModValues);
    
    /** loads the processors parameters with the values saved in an xml */
    void loadPresetForXml(XmlElement* inElement, OwnedArray<Value>& _nonModValues);
    
    /** returns the total number of presets */
    int getNumberOfPresets(bool _includeUserPresets);
    
    /** returns the preset name at inPresetIndex */
    String getPresetName(int inPresetIndex);
    
    /** creates a new preset */
    void createNewPreset();
    
    /** saves the current preset */
    void savePreset();
    
    /**deletes the current preset*/
    void deletePreset(int _indexToRemove);
    
    /** saves a new preset with inPresetName */
    void saveAsPreset(String inPresetName);
    
    /** loads the preset at inPresetIndex */
    void loadPreset(int inPresetIndex);
    
    /** returns true if the current preset is saved */
    bool getIsCurrentPresetSaved();
    
    /**returns true if the current preset is a factory preset*/
    bool getIsCurrentPresetFactory();
    
    /** returns the current preset name */
    String getCurrentPresetName();
    
private:
    
    //internal
    void storeLocalPresets();
    
    bool mCurrentPresetIsSaved;
    bool mCurrentPresetFactory;
    
    int mNumberofFactoryPresets;
    int mInitalizedIndexNumber;
    
    String mCurrentPresetName;
    
    File mCurrentlyLoadedPreset;
    
    Array<File> mLocalPresets;
    
    XmlElement* mCurrentPresetXml;
    
    AudioProcessor* mProcessor;
    
};
