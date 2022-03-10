// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Menu/Menu.h" 
#include "Settings/Settings.h"
#include "Pages/Definition.h"
#include "Display/DisplayTypes.h"
#include "MenuItems.h"
#include "Tables.h"
#include "Display/Display.h"
#include "Utils/GlobalFunctions.h"
#include "Data/Measures.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "FPGA/FPGA.h"
#include "Hardware/Sound.h"
#include "Panel/Panel.h"
#include "FDrive/FDrive.h"
#include "Menu/FileManager.h"
#include <string.h>



namespace Menu
{
    bool showDebugMenu = true;
    bool needClosePageSB = true;

    // ��� ������� ������ � ��� ������������ � ��� ���������� � �������� ��� �� �������� ������� ������� ������.
    Key::E pressButton = Key::None;
    // ��� ���������� ������ � ��� ������������ � ��� ���������� � �������� ��� �� ���������  ������� ���������� ������.
    Key::E releaseButton = Key::None;
    // ����, �� ������� ����� ��������� ����� ��������� - �������� �������� ���������� �������, ���� - ����������� - "-" - �����, "+" - ������
    int angleRegSet = 0;

    static const int stepAngleRegSet = 2;

    static const int  SIZE_BUFFER_FOR_BUTTONS = 5;
    static Key::E bufferForButtons[SIZE_BUFFER_FOR_BUTTONS] = {Key::None, Key::None, Key::None, Key::None, Key::None};
    static const Key::E sampleBufferForButtons[SIZE_BUFFER_FOR_BUTTONS] = {Key::F5, Key::F4, Key::F3, Key::F2, Key::F1};

    // ��������� ��������� ������� ������.
    void ProcessingShortPressureButton();
    // ��������� ��������� ������ ����.
    void ProcessingPressButton();
    // ��������� �������� ������ �����.
    void ProcessingReleaseButton();
    // ��������� �������� ����� ���������.
    void ProcessingRegulatorSet();
    // ��������/��������� ��������� ����� ���������, ���� ����������.
    void SwitchSetLED();
    // ��������� ��������� ������� �� ������� NamePage � ������� page.
    void ShortPress_Page(Item *page);
    // ��������� ��������� ������� �� ������� Choice � ������� choice.
    void ShortPress_Choice(Item *choice);

    void ShortPress_Time(Item *time);
    // ��������� ��������� ������� �� ������� Button � ������� button.
    void ShortPress_Button(Item *button);
    // ��������� ��������� ������� �� ������� Governor � ������� governor.
    void ShortPress_Governor(Item *governor);
    // ��������� ��������� ������� �� ������� GovernorColor � ������� governorColor.
    void ShortPress_GovernorColor(Item *governorColor);

    void ShortPress_IP(Item *item);

    void ShortPress_MAC(Item *item);

    void ShortPress_ChoiceReg(Item *choice);

    void ShortPress_SmallButton(Item *smallButton);
    // ��������� �������� ������� �� ������� ���� item.
    void FuncOnLongPressItem(Item *item);

    void FuncOnLongPressItemTime(Item *item);
    // ��������� �������� ������� �� ������� Button � ������� button.
    void FuncOnLongPressItemButton(Item *button);

    // ���������� true, ���� �������� ��������� ������ ������
    bool NeedForFireSetLED();

    void OpenFileManager();
    // ��������� ������� ������� ��������������� �������� ����.
    void OnTimerAutoHide();

    void ProcessButtonForHint(Key::E button);
}


void Menu::UpdateInput()
{
    ProcessingShortPressureButton();
    ProcessingRegulatorSet();
    ProcessingPressButton();
    ProcessingReleaseButton();
    SwitchSetLED();

    if (FM::needOpen)
    {
        FM::needOpen = false;
        OpenFileManager();
    }
};


void Menu::ProcessButtonForHint(Key::E button)
{
    if (button == Key::Menu)
    {
        Hint::string = LANG_RU ?
            "������ ���� ��������� ��������� �������:\n"
            "1. ��� �������� ���� ������� ���� ������� � ���������� � ������� 0.5� ��������� ����.\n"
            "2. ��� �������� ���� ��������� ������ � ������� 0.5� ��������� ����.\n"
            "3. ��� ��������� \"������\x99����� �� ����\x99���������\" ������� ���������� �������� ����������� ������ ����. ���� ������� �������� �������� ��������, ���� �����������.\n"
            "4. ��� ��������� \"������\x99����� �� ����\x99�����������\" ������� ���������� �������� �������� ������ ����. ���� ������� �������� ��������� � ������� ������, ���������� ������� �� ���������� ������� ����.\n"
            "5. ���� ���� ��������� � ������ ����� ������, �� ������� ��������� ��������."
            :
        "MENU button performs the following functions:\n"
            "1. At the closed menu pressing or pressing with deduction during 0.5s opens the menu.\n"
            "2. At the open menu deduction of the button during 0.5s closes the menu.\n"
#ifndef WIN32
#pragma push
#pragma diag_suppress 192
#endif
            "3. At control \"SERVICE\x99Mode btn MENU\x99\x43lose\" current becomes the page of the previous level of the menu. If the root page is current, the menu is closed.\n"
#ifndef WIN32
#pragma pop
#endif
            "4. At control \"SERVICE\x99Mode btn MENU\x99Toggle\" current becomes the page of the current level of the menu. If the current page the last in the current level, happens transition to the previous level of the menu.\n"
            "5. If the menu is in the mode of small buttons, pressing closes the page.";

    } else if (button == Key::Cursors)
    {
        Hint::string = LANG_RU ?
            "������ ������� ��������� ���� ��������� ���������."
            :
            "������� button to open the menu cursor measurements.";
    }
    else if (button == Key::Display)
    {
        Hint::string = LANG_RU ?
            "������ ������� ��������� ���� �������� �������."
            :
            "DISPLAY button opens the display settings menu.";
    }
    else if (button == Key::Memory)
    {
        Hint::string = LANG_RU ?
            "1. ��� ��������� \"������\x99����� ��\x99��� �� ������\x99����\" ��������� ���� ������ � �������.\n"
            "2. ��� ��������� \"������\x99����� ��\x99��� �� ������\x99����������\" ��������� ������ �� ����-����."
            :
            "1. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Menu\" opens a menu of memory\n"
            "2. When setting \"MEMORY-EXT\x99STORAGE\x99Mode btn MEMORY\x99Save\" saves the signal to the flash drive";
    }
    else if (button == Key::Measures)
    {
        Hint::string = LANG_RU ?
            "������ ����� ��������� ���� �������������� ���������."
            :
            "����� button opens a menu of automatic measurements.";
    }
    else if (button == Key::Help)
    {
        Hint::string = LANG_RU ?
            "1. ������ ������ ��������� ���� ������.\n"
            "2. ������� � ��������� ������ ������ � ������� 0.5� �������� � ��������� ����� ������ ���������."
            :
            "1. ������ button opens the help menu.\n"
            "2. Pressing and holding the ������ for 0.5s enables and disables output mode hints.";
    }
    else if (button == Key::Service)
    {
        Hint::string = LANG_RU ?
            "������ ������ ��������� ���� ��������� ������������."
            :
            "������ button opens a menu of service options.";
    }
    else if (button == Key::Start)
    {
        Hint::string = LANG_RU ?
            "������ ����/�TO� ��������� � ������������� ������� ����� ����������."
            :
            "����/�TO� button starts and stops the process of gathering information.";
    }
    else if (button == Key::ChannelA)
    {
        Hint::string = LANG_RU ?
            "1. ������ �����1 ��������� ���� �������� ������ 1.\n"
            "2. ������� � ��������� ������ �����1 � ������� 0.5� ������������� �������� ������ 1 �� ��������� 0�."
            :
            "1. �����1 button opens the settings menu of the channel 1.\n"
            "2. Pressing and holding the button �����1 for 0.5c for the offset of the vertical channel 1 0V.";
    }
    else if (button == Key::ChannelB)
    {
        Hint::string = LANG_RU ?
            "1. ������ �����2 ��������� ���� �������� ������ 2.\n"
            "2. ������� � ��������� ������ �����2 � ������� 0.5� ������������� �������� ������ 2 �� ��������� 0�."
            :
            "1. �����2 button opens the settings menu of the channel 2.\n"
            "2. Pressing and holding the button �����2 for 0.5c for the offset of the vertical channel 2 0V.";
    }
    else if (button == Key::Time)
    {
        Hint::string = LANG_RU ?
            "1. ������ ���� ��������� ���� �������� ���������.\n"
            "2. ������� � ��������� ������ ���� � ������� 0.5� ������������� �������� �� ����������� 0�."
            :
            "1. ���� button open the settings menu sweep.\n"
            "2. Pressing and holding the button ���� for 0.5s Is the offset horizontal 0s.";
    }
    else if (button == Key::Trig)
    {
    Hint::string = LANG_RU ?
            "1. ������ ����� ��������� ���� �������� �������������.\n"
            "2. ������� � ��������� � ������� 0.5� ������ ����� ��� ��������� \"������\x99��� ���� �����\x99�����������\" ���������� �������������� ��������� ������ �������������.\n"
            "3. ������� � ��������� � ������� 0.5� ������ ����� ��� ��������� \"������\x99��� ���� �����\x99����� ������\" ������������� ������� ������������� 0�."
            :
            "1. ����� button opens a menu settings synchronization.\n"
#ifndef WIN32
#pragma push
#pragma diag_suppress 192
#endif
            "2. Pressing and holding the button ����� for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99\x41utolevel\" automatically adjust the trigger level.\n"
#ifndef WIN32
#pragma pop
#endif
            "3. Pressing and holding the button ����� for 0.5s when setting \"SERVICE\x99Mode long TRIG\x99SReset trig level\" sets the trigger level 0V.";
    }
}


void Menu::Handlers::PressButton(Key::E button)
{
    Sound::ButtonPress();

    if (Hint::show)
    {
        ProcessButtonForHint(button);
        return;
    }

    if (pressButton == Key::Start)
    {
    } 
    else if (!Menu::IsShown())
    {
        for (int i = SIZE_BUFFER_FOR_BUTTONS - 1; i > 0; i--)
        {
            bufferForButtons[i] = bufferForButtons[i - 1];
        }
        bufferForButtons[0] = button;
      
        if (memcmp(bufferForButtons, sampleBufferForButtons, SIZE_BUFFER_FOR_BUTTONS * sizeof(Key::E)) == 0)
        {
            showDebugMenu = true;
            Display::ShowWarningGood(Warning::MenuDebugEnabled);
        }
    }

    pressButton = button;
};


void Menu::Handlers::ReleaseButton(Key::E button)
{
    Sound::ButtonRelease();

    if (!Hint::show)
    {
        releaseButton = button;
    }
};


void Menu::Handlers::RotateRegSetRight()
{   
    if (!Hint::show)
    {
        angleRegSet++;
        Display::Redraw();
    }
};


void Menu::Handlers::RotateRegSetLeft()
{
    if (!Hint::show)
    {
        angleRegSet--;
        Display::Redraw();
    }
};


Item *Item::UnderKey()
{
    return (Item *)underKey;
};


void Menu::SetAutoHide(bool)
{
    if(!IsShown())
    {
        return;
    }
    if(SettingsDisplay::TimeMenuAutoHide() == 0)
    {
        Timer::Disable(TypeTimer::MenuAutoHide);
    }
    else
    {
        Timer::Enable(TypeTimer::MenuAutoHide, SettingsDisplay::TimeMenuAutoHide(), OnTimerAutoHide);
    }
}


char* Menu::StringNavigation(char buffer[100])
{
    if(SHOW_STRING_NAVIGATION && IsShown())
    {
        buffer[0] = 0;
        pchar titles[10] = {0};
        int numTitle = 0;
        Item *item = Item::Opened();

        if(IsMainPage(item))
        {
            return 0;
        }

        while(!IsMainPage(item))
        {
            titles[numTitle++] = item->Title();
            item = item->Keeper();
        }

        for(int i = 9; i >= 0; i--)
        {
            if(titles[i])
            {
                strcat(buffer, titles[i]);
                if(i != 0)
                {
                    strcat(buffer, " - ");
                }
            }
        }

        return buffer;
    }
    return 0;
}


void Menu::OnTimerAutoHide()
{
    Show(false);
    Timer::Disable(TypeTimer::MenuAutoHide);
}


const Page *Page::ForButton(Key::E button)
{
    static const void *pages[] = {
        PageTrig::self,         // Key::Trig
        PageCursors::self,      // Key::Cursors
        PageMeasures::self,     // Key::Measures
    };

    return (const Page *)pages[button];
}


void Menu::ProcessingShortPressureButton()
{
    Display::Redraw();
    Menu::SetAutoHide(true);

    do
    {
        const Page *page = Page::ForButton(button);

        if (page)
        {
            page->SetCurrent(true);
            page->Open(true);
            Show(true);
        }
    } while (false);
}


void Menu::ProcessingRegulatorSet()
{
    if (angleRegSet == 0)
    {
        return;
    }

    if (IsShown() || Item::Opened()->GetType() != TypeItem::Page)
    {
        Item *item = Item::Current();
        TypeItem::E type = item->GetType();

        if (Item::Opened()->GetType() == TypeItem::Page && (type == TypeItem::ChoiceReg ||
            type == TypeItem::Governor || type == TypeItem::IP || type == TypeItem::MAC))
        {
            if (angleRegSet > stepAngleRegSet || angleRegSet < -stepAngleRegSet)
            {
                item->Change(angleRegSet);
                angleRegSet = 0;
            }

            return;
        }
        else
        {
            item = Item::Opened();
            type = item->GetType();
            if (IsMinimize())
            {
                Page::RotateRegSetSB(angleRegSet);
            }
            else if (type == TypeItem::Page || type == TypeItem::IP || type == TypeItem::MAC || type == TypeItem::Choice || type == TypeItem::ChoiceReg || type == TypeItem::Governor)
            {
                if (item->ChangeOpened(angleRegSet))
                {
                    angleRegSet = 0;
                }
                return;
            }
            else if (type == TypeItem::GovernorColor)
            {
                item->Change(angleRegSet);
            }
            else if (type == TypeItem::Time)
            {
                angleRegSet > 0 ? ((Time *)item)->IncCurrentPosition() : ((Time *)item)->DecCurrentPosition();
            }
        }
    }

    angleRegSet = 0;
}


void Menu::ProcessingPressButton()
{
    if (pressButton == Key::Start && !MODE_WORK_IS_LATEST)
    {
        FPGA::OnPressStartStop();
    } 
    else if((pressButton >= Key::F1 && pressButton <= Key::F5) || pressButton == Key::Menu)
    {
        if (pressButton != Key::Menu)
        {
            Item::underKey = Item::UnderKey(pressButton);
        }
    }
    pressButton = Key::None;
}


void Menu::ProcessingReleaseButton()
{
    if(releaseButton >= Key::F1 && releaseButton <= Key::F5 || pressButton == Key::Menu)
    {
        Item::underKey = nullptr;
        releaseButton = Key::None;
    }
}


void Menu::ShortPress_Page(Item *item)
{
    Page *page = (Page *)item;

    if (page->funcOnPress)
    {
        page->funcOnPress();
    }

    if (!page->IsActive())
    {
        return;
    }

    page->SetCurrent(true);

    ((Page*)page)->Open(!page->IsOpened());
}


void Menu::ShortPress_Choice(Item *choice)
{
    if (!choice->IsActive())
    {
        ((Choice *)choice)->FuncOnChanged(false); //-V1027
    }
    else if (!choice->IsOpened())
    {
        choice->SetCurrent(Item::Current() != choice);
        ((Choice *)choice)->StartChange(1); //-V1027
    }
    else
    {
        ((Choice *)choice)->ChangeValue(-1); //-V1027
    }
}


void Menu::ShortPress_ChoiceReg(Item *choice)
{
    if(!choice->IsActive()) 
    {
        ((Choice *)choice)->FuncOnChanged(false); //-V1027
    } 
    else if(Item::Opened() != choice)
    {
        choice->SetCurrent(Item::Current() != choice);
    }
}


void Menu::FuncOnLongPressItemButton(Item *button)
{
    ShortPress_Button(button);
}


void Menu::ShortPress_Button(Item *button)
{
    if(!button->IsActive())
    {
        return;
    }
    
    button->SetCurrent(true);
    ((Button*)button)->funcOnPress();
}


void Menu::FuncOnLongPressItem(Item *item)
{
    if (Item::Current() != item)
    {
        item->SetCurrent(true);
    }

    item->Open(!item->IsOpened());
}


void Menu::FuncOnLongPressItemTime(Item *time)
{
    if (Item::Current() != time)
    {
        time->SetCurrent(true);
    }

    if(time->IsOpened() && *((Time*)time)->curField == iSET)
    {
        ((Time *)time)->SetNewTime();
    }

    time->Open(!time->IsOpened());

    ((Time *)time)->SetOpened();
}


void Menu::ShortPress_Time(Item *time)
{
    if(!time->IsOpened())
    {
        time->SetCurrent(true);
        ((Time *)time)->SetOpened();
        time->Open(true);
    }
    else
    {
        ((Time *)time)->SelectNextPosition();
    }
}


void Menu::ShortPress_Governor(Item *governor)
{
    Governor *gov = (Governor*)governor;

    if(!governor->IsActive())
    {
        return;
    }

    if(Item::Opened() == gov)
    {
        gov->NextPosition();
    }
    else
    {
        gov->SetCurrent(Item::Current() != gov);
    }
}


void Menu::ShortPress_IP(Item *item)
{
    if (Item::Opened() == item)
    {
        ((IPaddress*)item)->NextPosition();
    }
}


void Menu::ShortPress_MAC(Item *item)
{
    if (Item::Opened() == item)
    {
        CircleIncreaseInt8(&MACaddress::cur_digit, 0, 5);
    }
}


void Menu::ShortPress_GovernorColor(Item *governorColor)
{
    if(!governorColor->IsActive())
    {
        return;
    }

    GovernorColor *governor = (GovernorColor*)governorColor; //-V1027

    if(Item::Opened() == (Item *)governor) //-V1027
    {
        CircleIncreaseInt8(&(governor->colorType->currentField), 0, 3);
    }
    else
    {
        FuncOnLongPressItem((Item *)governorColor);
    }
}


void Menu::ShortPress_SmallButton(Item *smallButton)
{
    SmallButton *sb = (SmallButton *)smallButton;

    if (sb)
    {
        pFuncVV func = sb->funcOnPress;

        if (func)
        {
            func();
            Item::underKey = (Item *)smallButton;
        }
    }
}


void Menu::ExecuteFuncForShortPressOnItem(Item *item)
{
    typedef void(*pFuncMenuVpV)(Item *);

    static const pFuncMenuVpV shortFunction[TypeItem::Count] =
    {
        nullptr,                            // TypeItem::None
        &Menu::ShortPress_Choice,           // TypeItem::Choice
        &Menu::ShortPress_Button,           // TypeItem::Button
        &Menu::ShortPress_Page,             // TypeItem::Page
        &Menu::ShortPress_Governor,         // TypeItem::Governor
        &Menu::ShortPress_Time,             // TypeItem::Time
        &Menu::ShortPress_IP,               // TypeItem::IP
        nullptr,                            // Item_SwitchButton
        &Menu::ShortPress_GovernorColor,    // TypeItem::GovernorColor
        nullptr,                            // Item_Formula
        &Menu::ShortPress_MAC,              // TypeItem::MAC
        &Menu::ShortPress_ChoiceReg,        // TypeItem::ChoiceReg
        &Menu::ShortPress_SmallButton       // TypeItem::SmallButton
    };
 
    pFuncMenuVpV func = shortFunction[item->GetType()];

    if (func)
    {
        (func)(item);
    }
}


void Menu::ExecuteFuncForLongPressureOnItem(Item *item)
{
    typedef void(*pFuncMenuVpV)(Item *);

    static const pFuncMenuVpV longFunction[TypeItem::Count] =
    {
        0,                                  // TypeItem::None
        &Menu::FuncOnLongPressItem,         // TypeItem::Choice
        &Menu::FuncOnLongPressItemButton,   // TypeItem::Button
        &Menu::FuncOnLongPressItem,         // TypeItem::Page
        &Menu::FuncOnLongPressItem,         // TypeItem::Governor
        &Menu::FuncOnLongPressItemTime,     // TypeItem::Time
        &Menu::FuncOnLongPressItem,         // TypeItem::IP
        0,                                  // Item_SwitchButton
        &Menu::FuncOnLongPressItem,         // TypeItem::GovernorColor
        0,                                  // Item_Formula
        &Menu::FuncOnLongPressItem,         // TypeItem::MAC
        &Menu::FuncOnLongPressItem,         // TypeItem::ChoiceReg
        &Menu::ShortPress_SmallButton       // TypeItem::SmallButton
    };

    if (item->IsActive())
    {
        pFuncMenuVpV func = longFunction[item->GetType()];
        if (func)
        {
            (func)(item);
        }
    }
}


void Menu::ChangeStateFlashDrive()
{
    if(!FDrive::isConnected)
    {
        if(Page::NameOpened() == NamePage::SB_FileManager)
        {
            ((Page *)Item::Opened())->ShortPressOnItem(0);
        }
    }
    else if(FLASH_AUTOCONNECT)
    {
        FM::needOpen = true;
    }
}


extern const Page pService;


void Time::Open()
{
    Display::ShowWarningGood(Warning::TimeNotSet);
    Menu::Handlers::ShortPressureButton(Key::Service);
    Menu::UpdateInput();
    Display::Update();

    for (int i = 0; i < 2; i++)
    {
        Menu::Handlers::RotateRegSetRight();
        Menu::UpdateInput();
        Display::Update();
    }

    Menu::Handlers::ShortPressureButton(Key::F4);
    Menu::UpdateInput();
    Display::Update();
}


bool Menu::NeedForFireSetLED()
{
    if (!IsShown())
    {
        return false;
    }
    NamePage::E name = Page::NameOpened();

    if (name == NamePage::SB_MeasTuneMeas && MEAS_NUM == MN_1 && !PageMeasures::choiceMeasuresIsActive)
    {
        return false;
    }

    if (
        name == NamePage::SB_MathCursorsFFT                             ||
        name == NamePage::SB_MeasTuneMeas                               ||
        name == NamePage::SB_MemLatest                                  || 
        name == NamePage::SB_MemInt                                     ||
        ((name == NamePage::SB_MathFunction) && !DISABLED_DRAW_MATH)    ||
        ((name == NamePage::SB_Curs) && PageCursors::NecessaryDrawCursors())
        )
    {
        return true;
    }
    
    TypeItem::E typeCurrentItem = Item::Current()->GetType();
    if (typeCurrentItem == TypeItem::Governor    ||
        typeCurrentItem == TypeItem::ChoiceReg)
    {
        return true;
    }

    TypeItem::E typeOpenedItem = Item::TypeOpened();

    if (typeOpenedItem == TypeItem::Choice       ||
        (typeOpenedItem == TypeItem::Page && ((Page *)Item::Opened())->NumSubPages() > 1)
        )
    {
        return true;
    }
    
    return false;
}


void Menu::SwitchSetLED()
{
    static bool first = true;
    static bool prevState = false;  // true - �����, false - �� �����

    bool state = NeedForFireSetLED();

    if (first)
    {
        first = false;
        Panel::EnableLEDRegSet(state);
        prevState = state;
    }
    else if (prevState != state)
    {
        Panel::EnableLEDRegSet(state);
        prevState = state;
    }
}


void Menu::OpenFileManager()
{
    angleRegSet = 0;
    for (int i = 0; i < 10; i++)
    {
        Handlers::ShortPressureButton(Key::Menu);
        UpdateInput();
        Display::Update(false);
    }

    if (!IsShown())
    {
        Handlers::ShortPressureButton(Key::Menu);
        UpdateInput();
        Display::Update(false);
    }

    for (int i = 0; i < 5 * stepAngleRegSet + 1; i++)
    {
        Handlers::RotateRegSetLeft();
        UpdateInput();
        Display::Update(false);
    }

    angleRegSet = 0;

    for (int i = 0; i < 2 * stepAngleRegSet + 1; i++)
    {
        Handlers::RotateRegSetRight();
        UpdateInput();
        Display::Update(false);
    }

    angleRegSet = 0;

    Handlers::ShortPressureButton(Key::F2);
    UpdateInput();
    Display::Update(false);

    Handlers::ShortPressureButton(Key::F4);
    UpdateInput();
    Display::Update(false);

    for (int i = 0; i < stepAngleRegSet + 1; i++)
    {
        Handlers::RotateRegSetLeft();
        UpdateInput();
        Display::Update(false);
    }

    for (int i = 0; i < 2; i++)
    {
        Handlers::ShortPressureButton(Key::F1);
        UpdateInput();
        Display::Update(false);
    }
}
