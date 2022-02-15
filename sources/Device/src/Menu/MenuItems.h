// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Display.h"
#include "Display/Colors.h"
#include "defines.h"

                                            
#define MAX_NUM_SUBITEMS_IN_CHOICE  12  // ������������ ���������� ��������� ������ � �������� Choice.
#define MAX_NUM_ITEMS_IN_PAGE       15  // ������������ ���������� ������� �� ��������.
#define MENU_ITEMS_ON_DISPLAY       5   // ������� ������� ���� ���������� �� ������ �� ���������.
#define MAX_NUM_CHOICE_SMALL_BUTTON 6   // ������������ ���������� ��������� ��������� ������ + 1


// ������ ���� ������� ����
enum TypeItem
{
    Item_None,           
    Item_Choice,        // ����� ������ - ��������� ������� ���� �� ���������� �������� ��������.
    Item_Button,        // ������.
    Item_Page,          // ��������.
    Item_Governor,      // ��������� - ��������� ������� ����� ����� �������� �������� �� ������� ��������� ����������.
    Item_Time,          // ��������� ������ �����.
    Item_IP,            // ��������� ������ IP-�����.
    Item_SwitchButton,
    Item_GovernorColor, // ��������� ������� ����.
    Item_Formula,       // ��������� ������� ���� � ������������ ��� �������������� ������� (��������� � ��������)
    Item_MAC,           // MAC-�����
    Item_ChoiceReg,     // ������� ������, � ������� ����� �������������� �� �������, � ������
    Item_SmallButton,   // ������ ��� ������ ����� ������
    Item_NumberItems
};

// ����� ID ���� �������������� � ���� �������
enum NamePage
{
    Page_Display,               // ������
    Page_DisplayAccumulation,   // ������� - ����������
    Page_DisplayAverage,        // ������� - ����������
    Page_DisplayGrid,           // ������� - �����
    Page_DisplayRefresh,        // ������� - ����������
    Page_MainPage,              // ������� �������� ����. �� �� ������ ��� ���������
    Page_Debug,                 // �������
    Page_DebugConsole,          // ������� - �������
    Page_DebugADC,              // ������� - ���
    Page_DebugADCbalance,       // ������� - ��� - ������
    Page_DebugADCstretch,       // ������� - ��� - ��������
    Page_DebugADCrShift,        // ������� - ��� - ���. ��������
    Page_DebugADCaltShift,      // ������� - ��� - ���. ����. ���.
    Page_DebugShowRegisters,    // ������� - ��������
    Page_DebugRandomizer,       // ������� - ����-���
    Page_DebugInformation,      // ������� - ����������
    Page_DebugSound,            // ������� - ����
    Page_DebugAltera,           // ������� - �������
    Page_Channel0,              // ����� 1
    Page_Channel1,              // ����� 2
    Page_Cursors,               // �������
    Page_Trig,                  // �����.
    Page_Memory,                // ������
    Page_MemoryExt,             // ������ - �����. ��
    Page_Measures,              // ���������
    Page_MeasuresField,         // ��������� - ����
    Page_Service,               // ������
    Page_ServiceDisplay,        // ������ - �������
    Page_ServiceDisplayColors,  // ������ - ������� - �����
    Page_ServiceCalibrator,     // ������ - ����������
    Page_ServiceEthernet,       // ������ - ETHERNET
    Page_Help,                  // ������
    Page_Time,                  // ���������
    Page_Math,                  // ������ - ����������
    Page_MathFFT,
    Page_TrigAuto,              // ����� - ��� �����
    Page_SB_Curs,               // ������� �������� ������� - ���������
    Page_SB_MeasTuneMeas,       // ������� �������� ��������� - ���������
    Page_SB_MeasTuneField,      // ������� �������� ��������� - ���� - ������
    Page_SB_MemLatest,          // ������� �������� ������ - ���������
    Page_SB_MemInt,             // ������� �������� ������ - ����� ��
    Page_SB_MemExtSetMask,      // ������� �������� ������ - ����� �� - �����
    Page_SB_MemExtSetName,      // ���������� ����� ����������� ����� �� ������ ��� ������� ����� ����� ��� ��������������� ����� ����� �� - ��� �����
    Page_SB_FileManager,        // ������� �������� ������ - ����� �� - �����
    Page_SB_Help,               // ������� �������� HELP
    Page_SB_MathCursorsFFT,     // ������� �������� ������ - ���������� - �������
    Page_SB_MathFunction,       // ������ - ���������� - �������
    Page_SB_Information,        // ������ - ����������
    Page_SB_SerialNumber,       // ������ - �/�
    Page_NumPages,
    Page_NoPage
};             // ��� ���������� ������� ����� �������� ������������� SettingsMenu � SSettings.c

class Page;

#define COMMON_PART_MENU_ITEM                                                                                     \
    TypeItem            type;             /* ��� ����� */                                                     \
    const class Page*   keeper;           /* ����� ��������, ������� �����������. ��� Page_MainPage = 0 */    \
    pFuncBV             funcOfActive;     /* ������� �� ������ ������� */                                     \
    pchar          titleHint[4];     /* �������� �������� �� ������� � ���������� ������. ����� ��������� ��� ������ ������ */



struct ControlStruct
{
    COMMON_PART_MENU_ITEM
};
class Control
{
public:
    COMMON_PART_MENU_ITEM

public:
    Control(const ControlStruct *str);
    Control(TypeItem type_, const Page* keeper_, pFuncBV funcOfActive_, pchar tileRU, pchar titleEN, pchar hintRU, pchar hintEN);
    // ���������� true, ���� ������, ��������������� ������� �������� ����, ��������� � ������� ���������.
    bool IsPressed();
};


#define TITLE(item) ((item)->titleHint[LANG])
#define HINT(item) ((item)->titleHint[2 + LANG])

typedef void * pVOID;
typedef pVOID arrayItems[MAX_NUM_ITEMS_IN_PAGE];


struct PageStruct
{
    COMMON_PART_MENU_ITEM
    NamePage            name;
    const arrayItems    items;
    pFuncVV             funcOnPress;
    pFuncVV             funcOnDraw;
    pFuncVI             funcRegSetSB;
};

// ��������� �������� ����.
class Page : public     Control
{
public:
    NamePage            name;                               // ��� �� ������������ NamePage
    const arrayItems   *items;                              // ����� ��������� �� ������ ���� �������� (� ������� ��������)
                                                            // ��� �������� ����� ������  ����� �������� 6 ���������� �� SmallButton : 0 - B_Menu, 1...5 - B_F1...B_F5
    pFuncVV             funcOnPress;                        // ����� ���������� ��� ������� �� �������� ��������
    pFuncVV             funcOnDraw;                         // ����� ���������� ����� ��������� ������
    pFuncVI             funcRegSetSB;                       // � �������� ����� ������ ���������� ��� �������� ����� ���������

    //Page(PageStruct *pageStruct);
    
    Page(const Page *keeper_, pFuncBV funcOfActive_,
         pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, NamePage name_,
         const arrayItems *items_, pFuncVV funcOnPress_ = 0, pFuncVV funcOnDraw_ = 0, pFuncVI funcRegSetSB_ = 0);

    void DrawOpened(int layer, int yTop);

    void Draw(int x, int y);
};


// ��������� ������.
class Button : public Control
{
public:
    pFuncVV         funcOnPress;    // �������, ������� ���������� ��� ������� �� ������.

    Button(const Page *keeper_, pFuncBV funcOfActive_,
            pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN, pFuncVV funcOnPress_);

    void Draw(int x, int y);
};


class StructHelpSmallButton
{
public:
    pFuncVII    funcDrawUGO;    // ��������� �� ������� ��������� ����������� �������� ������
    pchar helpUGO[2];     // ������� � ������� �����������.
};

typedef StructHelpSmallButton arrayHints[MAX_NUM_CHOICE_SMALL_BUTTON];


// ��������� ������ ��� ��������������� ������ ����.
class SmallButton : public Control
{
public:
    pFuncVV             funcOnPress;    // ��� ������� ��������� ��� ��������� ������� ������. ���������� true, ���� ���� ��
    pFuncVII            funcOnDraw;     // ��� ������� ���������� ��� ��������� ������ � ����� � ������������ x, y.
    const arrayHints   *hintUGO;

    SmallButton(const Page *keeper_, pFuncBV funcOfActive_,
                pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
                pFuncVV funcOnPress_, pFuncVII funcOnDraw_, const arrayHints *hintUGO_ = 0);

    void Draw(int x, int y);
};


// ��������� ���������.
class Governor : public Control
{ 
public:
    int16*      cell;
    int         minValue;               // ���������� ��������, ������� ����� ��������� ���������.
    int16       maxValue;               // ������������ ��������.
    pFuncVV     funcOfChanged;          // �������, ������� ����� �������� ����� ����, ��� �������� ���������� ����������.
    pFuncVV     funcBeforeDraw;         // �������, ������� ���������� ����� ����������

    Governor(const Page *keeper_, pFuncBV funcOfActive_,
             pchar titleRU, pchar titleEN, pchar hintRU, pchar hintEN,
             int16 *cell_, int16 minValue_, int16 maxValue_, pFuncVV funcOfChanged_ = 0, pFuncVV funcBeforeDraw_ = 0);

    void Draw(int x, int y, bool opened);
};


struct IPaddressStruct
{
    ControlStruct str;
    uint8* ip0;
    uint8* ip1;
    uint8* ip2;
    uint8* ip3;
    pFuncVB funcOfChanged;
    uint16 *port;
};

class  IPaddress : public Control
{
public:
    uint8*  ip0;
    uint8*  ip1;
    uint8*  ip2;
    uint8*  ip3;
    pFuncVB funcOfChanged;
    uint16* port;

    IPaddress(const IPaddressStruct *str);

    void Draw(int x, int y, bool opened);
};


class  MACaddress
{
public:
    COMMON_PART_MENU_ITEM
    uint8*  mac0;
    uint8*  mac1;
    uint8*  mac2;
    uint8*  mac3;
    uint8*  mac4;
    uint8*  mac5;
    pFuncVB funcOfChanged;

    void Draw(int x, int y, bool opened);
};

// ��������� ������� ���� ��� ��������� ������������� � ������ �������������� �������
#define FIELD_SIGN_MEMBER_1_ADD 0
#define FIELD_SIGN_MEMBER_1_MUL 1
#define FIELD_SIGN_MEMBER_2_ADD 2
#define FIELD_SIGN_MEMBER_2_MUL 3
#define POS_SIGN_MEMBER_1       0
#define POS_KOEFF_MEMBER_1      1
#define POS_SIGN_MEMBER_2       2
#define POS_KOEFF_MEMBER_2      3


class Formula
{
public:
    COMMON_PART_MENU_ITEM
    int8*           function;                   // ����� ������, ��� �������� Function, �� ������� ������ ���� ��������
    int8*           koeff1add;                  // ����� ������������ ��� ������ ����� ��� ��������
    int8*           koeff2add;                  // ����� ������������ ��� ������ ����� ��� ��������
    int8*           koeff1mul;                  // ����� ������������ ��� ������ ����� ��� ���������
    int8*           koeff2mul;                  // ����� ������������ ��� ������ ����� ��� ���������
    int8*           curDigit;                   // ������� ������ : 0 - ���� ������� �����, 1 - ����������� ������� �����,
                                                //  2 - ���� ������� �����, 3 - ����������� ������� �����
    pFuncVV         funcOfChanged;              // ��� ������� ���������� ����� ��������� ��������� �������� ����������.

    void Draw(int x, int y, bool opened);
};


class  GovernorColor
{
public:
    COMMON_PART_MENU_ITEM
    ColorType*      colorType;                  // ��������� ��� �������� �����.

    void Draw(int x, int y, bool opened);
};


class Choice
{
public:
    COMMON_PART_MENU_ITEM
    pchar names[MAX_NUM_SUBITEMS_IN_CHOICE][2];   // �������� ������ �� ������� � ���������� ������.
    int8*       cell;                                   // ����� ������, � ������� �������� ������� �������� ������.
    pFuncVB	    funcOnChanged;                          // ������� ������ ���������� ����� ��������� �������� ��������.
    pFuncVII    funcForDraw;                            // ������� ���������� ����� ��������� ��������. 
    pchar NameSubItem(int i);
    // ���������� ��� �������� �������� ������ �������� choice, ��� ��� �������� � �������� ���� ���������.
    pchar NameCurrentSubItem();
    // ���������� ��� ���������� �������� ������ �������� choice, ��� ��� �������� � �������� ���� ���������.
    pchar NameNextSubItem();

    pchar NamePrevSubItem();
    // ���������� ���������� ��������� ������ � �������� �� ������ choice.
    int NumSubItems();

    void StartChange(int delta);
    // ������������ ��������� ���� ��������.
    float Step();
    // �������� �������� choice � ����������� �� �������� � ����� delta.
    void ChangeValue(int delta);

    void FuncOnChanged(bool active);

    void FuncForDraw(int x, int y);

    void Draw(int x, int y, bool opened);
};


#define iEXIT   0
#define iDAY    1
#define iMONTH  2
#define iYEAR   3
#define iHOURS  4
#define iMIN    5
#define iSEC    6
#define iSET    7

// ������������� � ���������� �����.
struct Time
{
    COMMON_PART_MENU_ITEM
    int8*       curField;       // ������� ���� ���������. 0 - �����, 1 - ���, 2 - ���, 3 - ����, 4 - ����, 5 - �����, 6 - ���, 7 - ����������.
    int8*       hours;
    int8*       minutes;
    int8*       seconds;
    int8*       month;
    int8*       day;
    int8*       year;

    void Draw(int x, int y, bool opened);
};
