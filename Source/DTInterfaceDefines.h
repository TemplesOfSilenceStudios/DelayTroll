/*
  ==============================================================================

    DTInterfaceDefines.h
    Created: 19 Apr 2019 12:58:52pm
    Author:  Sam Ferguson

 defines arrrays and enums centralized to make for easy modification
 
  ==============================================================================
*/

#pragma once

#define EDITOR_PANEL_EXP_HEIGHT         900

#define EDITOR_PANEL_WIDTH              900
#define EDITOR_PANEL_HEIGHT             556

#define TOP_PANEL_WIDTH                 EDITOR_PANEL_WIDTH
#define TOP_PANEL_HEIGHT                90

#define MAIN_MOD_MENU_PANEL_WIDTH       EDITOR_PANEL_WIDTH
#define MAIN_MOD_MENU_PANEL_HEIGHT      90

#define MAIN_PANEL_WIDTH                EDITOR_PANEL_WIDTH
#define MAIN_PANEL_HEIGHT               EDITOR_PANEL_HEIGHT - TOP_PANEL_HEIGHT - MAIN_MOD_MENU_PANEL_HEIGHT

#define MAIN_MOD_PANEL_WIDTH            EDITOR_PANEL_WIDTH
#define MAIN_MOD_PANEL_HEIGHT           (EDITOR_PANEL_WIDTH - EDITOR_PANEL_HEIGHT) + MAIN_MOD_MENU_PANEL_HEIGHT

#define LEFT_PANEL_WIDTH                MAIN_PANEL_WIDTH/4 +2
#define LEFT_PANEL_HEIGHT               MAIN_PANEL_HEIGHT

#define RIGHT_PANEL_WIDTH               LEFT_PANEL_WIDTH - 3
#define RIGHT_PANEL_HEIGHT              MAIN_PANEL_HEIGHT

#define MAIN_DISPLAY_PANEL_WIDTH        MAIN_PANEL_WIDTH - LEFT_PANEL_WIDTH - RIGHT_PANEL_WIDTH
#define MAIN_DISPLAY_PANEL_HEIGHT       MAIN_PANEL_HEIGHT

#define DISPLAY_MENU_PANEL_WIDTH        MAIN_DISPLAY_PANEL_WIDTH
#define DISPLAY_MENU_PANEL_HEIGHT       30

#define TROLL_CONTROL_PANEL_WIDTH       TROLL_DISPLAY_PANEL_WIDTH
#define TROLL_CONTROL_PANEL_HEIGHT      119

#define TROLL_DISPLAY_COMPONENT_BUMPER  40
#define TROLL_DISPLAY_COMPONENT_LBUMPER 8
#define TROLL_DISPLAY_COMP_HORZ_BUMPER  55
#define TROLL_DISPLAY_GRID_LABEL_BUMPER 30

#define TROLL_DISPLAY_PANEL_WIDTH       MAIN_DISPLAY_PANEL_WIDTH
#define TROLL_DISPLAY_PANEL_HEIGHT      MAIN_DISPLAY_PANEL_HEIGHT - DISPLAY_MENU_PANEL_HEIGHT - TROLL_CONTROL_PANEL_HEIGHT

#define TROLL_DISPLAY_COMPONENT_WIDTH   TROLL_DISPLAY_PANEL_WIDTH - TROLL_DISPLAY_GRID_LABEL_BUMPER - TROLL_DISPLAY_COMP_HORZ_BUMPER*2
#define TROLL_DISPLAY_COMPONENT_HEIGHT  TROLL_DISPLAY_PANEL_HEIGHT - TROLL_DISPLAY_COMPONENT_BUMPER -TROLL_DISPLAY_COMPONENT_LBUMPER

#define MODULATION_ASSIGN_PANEL_WIDTH   200
#define MODULATION_ASSIGN_PANEL_HEIGHT  MAIN_MOD_PANEL_HEIGHT - MAIN_MOD_MENU_PANEL_HEIGHT

#define MODULATION_ASSIGN_PANEL_BUMPER  20

#define MODULATION_PANEL_WIDTH          MAIN_MOD_PANEL_WIDTH - MODULATION_ASSIGN_PANEL_WIDTH
#define MODULATION_PANEL_HEIGHT         MAIN_MOD_PANEL_HEIGHT - MAIN_MOD_MENU_PANEL_HEIGHT

#define MOD_DISPLAY_PANEL_WIDTH         MODULATION_PANEL_WIDTH - MODULATION_ASSIGN_PANEL_WIDTH
#define MOD_DISPLAY_PANEL_HEIGHT        MODULATION_PANEL_HEIGHT

#define DISPLAY_PANEL_HORIZONTAL_BUMPER 26
#define DISPLAY_PANEL_VERTICAL_BUMPER   38
#define DISPLAY_PANEL_LOWER_BUMPER      31

#define MOD_INNER_DISPLAY_PANEL_WIDTH   MOD_DISPLAY_PANEL_WIDTH - DISPLAY_PANEL_HORIZONTAL_BUMPER - 6
#define MOD_INNER_DISPLAY_PANEL_HEIGHT  MOD_DISPLAY_PANEL_HEIGHT - DISPLAY_PANEL_VERTICAL_BUMPER - DISPLAY_PANEL_LOWER_BUMPER

#define MODULATION_FREQ_CONTROL_WIDTH   MODULATION_ASSIGN_PANEL_WIDTH
#define MODULATION_FREQ_CONTROL_HEIGHT  125

#define LABEL_BUMPER                    25

const Point<float> DTDialCentrePoint(450.f, 43.f);

enum DTDisplayPanels
{
    kDisPanel_Time = 0,
    kDisPanel_Level,
    kDisPanel_EQ,
    kDisPannel_Pitch,
    kDisPannel_Pan,
    kDisPanel_TotalNumDisplayPanels,
};

enum DTControlPanels
{
    kControlPanel_Time = 0,
    kControlPanel_Level,
    kControlPanel_EQ,
    kControlPanel_Pitch,
    kControlPanel_Pan,
    kControlPanel_TotalNumDisplayPanels,
    
};

static StringArray DTDisplayPanelIDs("     Time     ",
                                     "     Level    ",
                                     "      EQ      ",
                                     "      Pitch     ",
                                     "       Pan      ");

enum DTModulationPanels
{
    kModPanel_Bezier = 0,
    kModPanel_LFO,
    kModPanel_TotalNumModPanels,
};

enum DTButtons
{
    kButton_Expand = 0,
    kButton_Left,
    kButton_Right,
    kButton_ModulationTabs,
    kButton_ModulationSave,
    kButton_ModulationSaveAs,
    kButton_ModulationDelete,
    kButton_TotalNumButtons,
};

static String DTButtonNames[kButton_TotalNumButtons] =
{
    "Expand",           //    kButton_Expand
    "Left",             //    kButton_Left
    "Right",            //    kButton_Right
    "ModulationTabs",   //    kButton_ModulationTabs
    "Save",             //    kButton_ModulationSave
    "SaveAs",           //    kButton_ModulationSaveAs
    "Delete"            //    kButton_ModulationDelete
};

enum DTDisplayValues
{
    kDisVal_bezierEditingIndex = 0,
    kDisVal_LFOEditingIndex,
    kDisVal_LFONumEditingIndex,
    kDisVal_bezierPresetMenu,
    kDisVal_LFOPresetMenu,
    kDisVal_bezierRateChoiceMenu,
    kDisVal_LFORateChoiceMenu,
    kDisVal_numAssigningIndex,
    kDisVal_modulationCategoryMenu,
    kDisVal_bezierViewportRowNum,
    kDisVal_EQBPModeState,
    kDIsVal_DisplayPanelSelection,
    kDisVal_PlotEditingIndex,
    kDisVal_TotalNumDisplayValues,
};

static Identifier DTDisplayValueIDs[kDisVal_TotalNumDisplayValues]
{
    "bezierEditingIndex",
    "LFOEditingIndex",
    "LFONumEditingIndex",
    "bezierPresetMenu",
    "LFOPresetMenu",
    "bezierRateChoiceMenu",
    "LFORateChoiceMenu",
    "numAssigningIndex",
    "modulationCategoryMenu",
    "bezierViewportRowNum",
    "EQBPModeState",
    "DisplayPanelSelection",
    "plotEditingIndex"
};

static int DTDisplayValueDefault[kDisVal_TotalNumDisplayValues]
{
    0,
    0,
    1,
    0,
    0,
    1,
    1,
    1,
    0,
    1,
    1,
    0,
    0
};

static int DTFrequencyMarkers[9]
{
    35,
    75,
    150,
    300,
    600,
    1200,
    2600,
    6000,
    12000
};
static const int DTNumberofDBMarkers{9};
static int DTDBMarkers[DTNumberofDBMarkers]
{
    0,
    -3,
    -6,
    -9,
    -12,
    -18,
    -24,
    -32,
    -48
};
static const StringArray DTPanBalLabels("80%",
                                        "60%",
                                        "40%",
                                        "20%",
                                        "0%",
                                        "20%",
                                        "40%",
                                        "60%",
                                        "80%");
static const StringArray DTPitchLabels("+11",
                                       "+10",
                                       "+9",
                                       "+8",
                                       "+7",
                                       "+6",
                                       "+5",
                                       "+4",
                                       "+3",
                                       "+2",
                                       "+1",
                                       " 0",
                                       "-1",
                                       "-2",
                                       "-3",
                                       "-4",
                                       "-5",
                                       "-6",
                                       "-7",
                                       "-8",
                                       "-9",
                                       "-10",
                                       "-11");

namespace DTColourIds {
const Colour LrgDisplayGlow         {0xffa9b344};
const Colour LrgDisplayDimGlow      {0xff858c35};
const Colour SmlDisplayGlow         {0xfff8f45d};
const Colour NodePoints             {0xff172626};
const Colour TextGlow               {0xffffb966};
const Colour TextGlowTrans          {0x00ffb966};
//const Colour PopUpBG                {0xff726646};
const Colour PopUpBG                {0xffE6E6E5};
//const Colour PopUpText              {0xff262625};
const Colour PopUpText              {0xff5b4d4d};
const Colour DialText               {0xff999593};
}
