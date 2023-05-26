/*
  ==============================================================================

    DTModulationDefines.h
    Created: 20 Apr 2019 10:02:45am
    Author:  Sam Ferguson

 some Arrats and enums to keep the modulation values in sync
 
  ==============================================================================
*/

#pragma once

static const int DTnumOfMods = 10;
static const int DTModulationUpdateFrequency = 50;

enum DTMods
{
    kMod_Bezier =0,
    kMod_LFO = DTnumOfMods,
    kMod_EQLowPassBez = DTnumOfMods*2,
    kMod_EQHighPassBez,
    kMod_EQBPBez,
    kMod_LevelBez,
    kMod_PitchBez,
    kMod_LeftPanBez,
    kMod_RightPanBez,
    kMod_BalPanBez,
    kMod_TotalNumOfMods,

};

enum DTModulationTypes
{
    kModType_Bezier = 0,
    kModType_LFO,
    kModType_TotalNumOfMods
};

static const int DTmodArraySize = 100;

enum DTHzRange{
    kHz_MinHz = 0,
    kHz_MaxHz,
    kHz_Interval,
    kHz_TotalNumofHzRanges,
};

static double DTDefaultHzRanges[kHz_TotalNumofHzRanges] =
{
    0.01,            //kHz_MinHz
    5.0,            //kHz_MaxHz
    0.005             //kHz_Interval
};

static NormalisableRange<double> DTDefualtHzRange(DTDefaultHzRanges[kHz_MinHz],DTDefaultHzRanges[kHz_MaxHz],DTDefaultHzRanges[kHz_Interval]);

enum DTLFOTypes
{
    kLFO_Sine = 0,
    kLFO_Saw,
    kLFO_Triangle,
    kLFO_Square,
    kLFO_Noise,
    kLFO_TotalNumOfLFOTypes,
};

static StringArray DTLFOTypeIDs("Sine",
                                "Saw",
                                "Triangle",
                                "Square",
                                "Noise");

enum DTAssignableParams
{
    kAss_Feedback = 0,
    KAss_InputLevel,
    kAss_OutputLevel,
    kAss_WetDry,
    kAss_SmoothFactor,
    kAss_MinTime,
    kAss_MaxTime,
    kAss_Gravity,
    kAss_TotalNumOFAssignableParams,
};

static StringArray DTAssignableParamsName("Feedback",
                                          "Input Level",
                                          "Output Level",
                                          "Wet/Dry",
                                          "Smooth factor",
                                          "First Tap",
                                          "Last Tap",
                                          "Gravity");

static String DTAssignableParamsID[kAss_TotalNumOFAssignableParams] =
{
    "Feedback",
    "InputLevel",
    "OutputLevel",
    "WetDry",
    "SmoothFactor",
    "MinDelayTime",
    "MaxDelayTime",
    "GravityExpo",
};


static float DTAssignOriginalValues[kAss_TotalNumOFAssignableParams] =
{
    0.f,          //Feedback
    1.f,          //InputLevel
    1.f,          //OutputLevel
    0.5f,           //wet dry
    0.001f,      //Smooth Factor
    0.f,          //MinDelayTime
    2000.0f,       //MaxDelayTime
    1.4f           //GravityExpo
};

static Array<NormalisableRange<float>> DTAssignParamRanges
{
    NormalisableRange<float>(0.0f, 2.0f),                       //Feedback
    NormalisableRange<float>(0.0f, 1.0f),                       //inputLevel
    NormalisableRange<float>(0.0f, 1.0f),                       //OutputLevel
    NormalisableRange<float>(0.0f, 1.0f),                       //Wet Dry
    NormalisableRange<float>(0.00001f, 0.002f),                 //Smooth Factor
    NormalisableRange<float>(0.0f, 10000.0f, 1, 0.3f),          //MinDelayTime
    NormalisableRange<float>(0.0f, 10000.0f, 1, 0.3f),          //MaxDelayTime
    NormalisableRange<float>(0.005f, 1.995f)                    //GravetyExpo
};

//must be kept in line with DTMods proably a bad idea but Ill fix it in Delay troll 2...
enum DTBezierCurves
{
    kBez_EQLowPass =0,
    kBez_EQHighPass,
    kBez_EQBP,
    kBez_Level,
    kBez_Pitch,
    kBez_LeftPan,
    kBez_RightPan,
    kBez_BalPan,
    kBez_TotalNumOfBez,

};

static Array<std::vector<Point<float>>> DTBezierDefaultNodes =
{
    std::vector<Point<float>>{                          //EQ Lowpass
        Point<float>(0.f, 0.f),
        Point<float>(0.25f,0.1f),
        Point<float>(0.5f,0.2f),
        Point<float>(1.f,0.3f)
    },
    std::vector<Point<float>>{                          //EQ HighPass
        Point<float>(0.f, 1.f),
        Point<float>(0.25f, 0.9f),
        Point<float>(0.5f, 0.8f),
        Point<float>(1.f, 0.7f)
    },
    std::vector<Point<float>>{                          //EQ BandPass
        Point<float>(0.f, 0.5f),
        Point<float>(0.33f, 0.5f),
        Point<float>(0.66f, 0.5f),
        Point<float>(1.f, 0.5f)
    },
    std::vector<Point<float>>{                          //Level
        Point<float>(0.f, 0.f),
        Point<float>(0.1f, 0.4f),
        Point<float>(0.5f, 0.8f),
        Point<float>(1.f, 0.85f)
    },
    std::vector<Point<float>>{                          //Pitch
        Point<float>(0.f, 0.5f),
        Point<float>(0.33f, 0.5f),
        Point<float>(0.66f, 0.5f),
        Point<float>(1.f, 0.5f)
    },
    std::vector<Point<float>>{                          //Left Pan
        Point<float>(0.f, 0.f),
        Point<float>(0.33f, 0.f),
        Point<float>(0.66f, 0.f),
        Point<float>(1.f, 0.f)
    },
    std::vector<Point<float>>{                          //Right Pan
        Point<float>(0.f, 1.f),
        Point<float>(0.33f, 1.f),
        Point<float>(0.66f, 1.f),
        Point<float>(1.f, 1.f)
    },
    std::vector<Point<float>>{                          //Balence Pan
        Point<float>(0.f, 0.5f),
        Point<float>(0.2f, 0.f),
        Point<float>(0.4f, 1.f),
        Point<float>(0.6f, 0.f),
        Point<float>(0.8f, 1.f),
        Point<float>(1.f, 0.5f)
    },
    
};

static Array<std::vector<Point<float>>> DTBezierInitalizedNodes =
{
    std::vector<Point<float>>{                          //EQ Lowpass
        Point<float>(0.f, 0.f),
        Point<float>(0.33f,0.f),
        Point<float>(0.66f,0.f),
        Point<float>(1.f,0.f)
    },
    std::vector<Point<float>>{                          //EQ HighPass
        Point<float>(0.f, 1.f),
        Point<float>(0.33f, 1.f),
        Point<float>(0.66f, 1.f),
        Point<float>(1.f, 1.f)
    },
    std::vector<Point<float>>{                          //EQ BandPass
        Point<float>(0.f, 0.5f),
        Point<float>(0.33f, 0.5f),
        Point<float>(0.66f, 0.5f),
        Point<float>(1.f, 0.5f)
    },
    std::vector<Point<float>>{                          //Level
        Point<float>(0.f, 0.f),
        Point<float>(0.33f, 0.f),
        Point<float>(0.66f, 0.f),
        Point<float>(1.f, 0.f)
    },
    std::vector<Point<float>>{                          //Pitch
        Point<float>(0.f, 0.5f),
        Point<float>(0.33f, 0.5f),
        Point<float>(0.66f, 0.5f),
        Point<float>(1.f, 0.5f)
    },
    std::vector<Point<float>>{                          //Left Pan
        Point<float>(0.f, 0.f),
        Point<float>(0.33f, 0.f),
        Point<float>(0.66f, 0.f),
        Point<float>(1.f, 0.f)
    },
    std::vector<Point<float>>{                          //Right Pan
        Point<float>(0.f, 1.f),
        Point<float>(0.33f, 1.f),
        Point<float>(0.66f, 1.f),
        Point<float>(1.f, 1.f)
    },
    std::vector<Point<float>>{                          //Balence Pan
        Point<float>(0.f, 0.5f),
        Point<float>(0.33f, 0.5f),
        Point<float>(0.66f, 0.5f),
        Point<float>(1.f, 0.5f)
    },
    
};
