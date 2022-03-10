// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Panel.h"
#include "Panel/Controls_Old.h"
#include "FPGA/FPGA.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Display/Display.h"
#include "Hardware/Timer.h"
#include "Log.h"
#include "Hardware/Sound.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Containers/Queue.h"
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_gpio.h>
#include <stdio.h>
#include <string.h>
#include "Panel/PanelFunctions.cpp"


namespace Panel
{
    const uint8 LED_TRIG     = 1;
    const uint8 LED_REG_SET  = 2;
    const uint8 LED_CHAN_A   = 3;
    const uint8 LED_CHAN_B   = 4;

    KeyOld::E pressedKey = KeyOld::Empty;
    volatile KeyOld::E pressedButton = KeyOld::Empty;         // ��� ������������ ��� ������������ ������� ������ ��� ����������� ������

    Queue<uint8> data_for_send;                         // ����� ������ ��� ��������� � ������

    uint timePrevPressButton = 0;
    uint timePrevReleaseButton = 0;

    Queue<uint16> input_buffer;

    bool isRunning = true;

    // ����������� ������ �� ����� ������ � ������, ������������ ������ ���������
    uint16 TranslateCommand(const uint8 *data, uint size);

    void OnTimerPressedKey();

    KeyOld::E ButtonIsRelease(uint16 command);

    KeyOld::E ButtonIsPress(uint16 command);

    Regulator::E RegulatorLeft(uint16 command);

    Regulator::E RegulatorRight(uint16 command);

    Regulator::E RegulatorIsPress(uint16 command);

    static void(*funcOnKeyDown[KeyOld::Count])() =
    {
        0,
        EmptyFuncVV,    // KeyOld::ChannelA
        EmptyFuncVV,    // KeyOld::Service
        EmptyFuncVV,    // KeyOld::ChannelB
        EmptyFuncVV,    // KeyOld::Display
        EmptyFuncVV,    // KeyOld::Time
        EmptyFuncVV,    // KeyOld::Memory                                         
        EmptyFuncVV,    // B_Sinchro
        StartDown,      // KeyOld::Start
        EmptyFuncVV,    // KeyOld::Cursors
        EmptyFuncVV,    // KeyOld::Measures
        PowerDown,      // KeyOld::Power
        EmptyFuncVV,    // KeyOld::Help
        EmptyFuncVV,    // KeyOld::Menu
        EmptyFuncVV,    // KeyOld::F1
        EmptyFuncVV,    // KeyOld::F2
        EmptyFuncVV,    // KeyOld::F3
        EmptyFuncVV,    // KeyOld::F4
        EmptyFuncVV     // KeyOld::F5
    };

    static void (*funcOnKeyUp[KeyOld::Count])() =
    {
        0,
        EmptyFuncVV,    // KeyOld::ChannelA
        EmptyFuncVV,    // KeyOld::Service
        EmptyFuncVV,    // KeyOld::ChannelB
        EmptyFuncVV,    // KeyOld::Display
        EmptyFuncVV,    // KeyOld::Time
        EmptyFuncVV,    // KeyOld::Memory
        EmptyFuncVV,    // B_Sinchro
        EmptyFuncVV,    // KeyOld::Start
        EmptyFuncVV,    // KeyOld::Cursors
        EmptyFuncVV,    // KeyOld::Measures
        EmptyFuncVV,    // KeyOld::Power
        EmptyFuncVV,    // KeyOld::Help
        EmptyFuncVV,    // KeyOld::Menu
        EmptyFuncVV,    // KeyOld::F1
        EmptyFuncVV,    // KeyOld::F2
        EmptyFuncVV,    // KeyOld::F3
        EmptyFuncVV,    // KeyOld::F4
        EmptyFuncVV     // KeyOld::F5
    };

    static void (*funcOnLongPressure[KeyOld::Count])() =
    {
        0,
        ChannelALong,   // KeyOld::ChannelA
        EmptyFuncVV,    // KeyOld::Service
        ChannelBLong,   // KeyOld::ChannelB
        EmptyFuncVV,    // KeyOld::Display
        TimeLong,       // KeyOld::Time
        EmptyFuncVV,    // KeyOld::Memory
        TrigLong,       // B_Sinchro
        EmptyFuncVV,    // KeyOld::Start
        EmptyFuncVV,    // KeyOld::Cursors
        EmptyFuncVV,    // KeyOld::Measures
        EmptyFuncVV,    // KeyOld::Power
        HelpLong,       // KeyOld::Help
        MenuLong,       // KeyOld::Menu
        F1Long,         // KeyOld::F1
        F2Long,         // KeyOld::F2
        F3Long,         // KeyOld::F3
        F4Long,         // KeyOld::F4
        F5Long          // KeyOld::F5
    };

    static void (*funcOnRegulatorPress[8])() =
    {
        ChannelALong,   // 20 - Regulator::RangeA
        ChannelALong,   // 21 - Regulator::RShiftA
        ChannelBLong,   // 22 - Regulator::RangeB
        ChannelBLong,   // 23 - Regulator::RShiftB
        TimeLong,       // 24 - Regulator::TBase
        TimeLong,       // 25 - Regulator::TShift
        TrigLong,       // 26 - Regulator::TrigLev
        MenuLong        // 27 - Regulator::Set
    };

    static void (*funculatorLeft[Regulator::Set + 1])() =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        RangeLeftA,     // Regulator::RangeA
        RShift0Left,    // Regulator::RShiftA
        RangeLeftB,     // Regulator::RangeB
        RShift1Left,    // Regulator::RShiftB
        TBaseLeft,      // Regulator::TBase
        TShiftLeft,     // Regulator::TShift
        TrigLevLeft,    // Regulator::TrigLev
        SetLeft         // Regulator::Set
    };

    static void (*funculatorRight[Regulator::Set + 1])() =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        RangeRightA,    // Regulator::RangeA
        RShift0Right,   // Regulator::RShiftA
        RangeRightB,    // Regulator::RangeB
        RShift1Right,   // Regulator::RShiftB
        TBaseRight,     // Regulator::TBase
        TShiftRight,    // Regulator::TShift
        TrigLevRight,   // Regulator::TrigLev
        SetRight        // Regulator::Set
    };
}



KeyOld::E Panel::ButtonIsRelease(uint16 command)
{
    KeyOld::E button = KeyOld::Empty;

    if(command < KeyOld::Count && command > KeyOld::Empty)
    {
        if(TIME_MS - timePrevReleaseButton > 100)
        {
            button = (KeyOld::E)command;
            timePrevReleaseButton = TIME_MS;
        }
    }

    return button;
}


KeyOld::E Panel::ButtonIsPress(uint16 command)
{
    KeyOld::E button = KeyOld::Empty;

    if (((command & 0x7f) < KeyOld::Count) && ((command & 0x7f) > KeyOld::Empty))
    {
        if(TIME_MS - timePrevPressButton > 100)
        {
            button = (KeyOld::E)(command & 0x7f);
            timePrevPressButton = TIME_MS;
        }
    }

    return button;
}


Regulator::E Panel::RegulatorIsPress(uint16 command)
{
    if ((command & 0xC0) != 0xC0)
    {
        return Regulator::Empty;
    }

    return (Regulator::E)(command & 0x3F);
}


Regulator::E Panel::RegulatorLeft(uint16 command)
{
    if(Regulator::IsLeft(command))
    {
        return (Regulator::E)command;
    }

    return Regulator::Empty;
}


Regulator::E Panel::RegulatorRight(uint16 command)
{
    if(Regulator::IsRight(command))
    {
        return (Regulator::E)(command & 0x7f);
    }
    return Regulator::Empty;
}


void Panel::OnTimerPressedKey()
{
    if(pressedKey != KeyOld::Empty)
    {
        void (*func)() = funcOnLongPressure[pressedKey];
        Menu::Handlers::ReleaseButton(pressedKey);

        if(func != 0)
        {
            func();
        }

        pressedKey = KeyOld::Empty;
    }
    Timer::Disable(TypeTimer::PressKey);
}


KeyOld::E Panel::WaitPressingButton()
{
    Timer::PauseOnTime(500);

    input_buffer.Clear();

    pressedButton = KeyOld::Empty;

    while (pressedButton == KeyOld::Empty)
    {
        Panel::Update();
    };

    input_buffer.Clear();

    return pressedButton;
}


void Panel::ProcessingCommandFromPIC(uint16 command)
{
    KeyOld::E releaseButton = ButtonIsRelease(command);
    KeyOld::E pressButton = ButtonIsPress(command);
    Regulator::E regLeft = RegulatorLeft(command);
    Regulator::E regRight = RegulatorRight(command);
    Regulator::E regPress = RegulatorIsPress(command);

    if (pressButton != KeyOld::Empty)
    {
        pressedButton = pressButton;
    }

    if(!isRunning)
    {
        return;
    }

    if(releaseButton != KeyOld::Empty)
    {
        Menu::Handlers::ReleaseButton(releaseButton);

        funcOnKeyUp[releaseButton]();

        if(pressedKey != KeyOld::Empty)
        {
            Menu::Handlers::ShortPressureButton(releaseButton);
            pressedKey = KeyOld::Empty;
        }
    }
    else if(pressButton != KeyOld::Empty)
    {
        funcOnKeyDown[pressButton]();
        Menu::Handlers::PressButton(pressButton);
        pressedKey = pressButton;
        Timer::Enable(TypeTimer::PressKey, 500, OnTimerPressedKey);
    }
    else if(regLeft != Regulator::Empty)
    {
         funculatorLeft[regLeft]();
    }
    else if(regRight != Regulator::Empty)
    {
        funculatorRight[regRight]();
    }

    if (regPress != Regulator::Empty)
    {
        int index = regPress - Regulator::RangeA;

        Sound::ButtonPress();

        funcOnRegulatorPress[index]();
    }
}


void Panel::EnableLEDChannelA(bool enable)
{
    uint8 data = LED_CHAN_A;

    if (enable)
    {
        data |= 0x80;
    }

    TransmitData(data);
}


void Panel::EnableLEDChannelB(bool enable)
{
    uint8 data = LED_CHAN_B;

    if (enable)
    {
        data |= 0x80;
    }

    TransmitData(data);
}


void Panel::EnableLEDTrig(bool enable)
{
    static uint timeEnable = 0;
    static bool first = true;
    static bool fired = false;

    if(first)
    {
        Panel::TransmitData(LED_TRIG);
        Display::EnableTrigLabel(false);
        timeEnable = TIME_MS;
        first = false;
    }

    if(enable)
    {
        timeEnable = TIME_MS;
    }

    if(enable != fired)
    {
        if(enable)
        {
            Panel::TransmitData(LED_TRIG | 0x80);
            Display::EnableTrigLabel(true);
            fired = true;
        }
        else if(TIME_MS - timeEnable > 100)
        {
            Panel::TransmitData(LED_TRIG);
            Display::EnableTrigLabel(false);
            fired = false;
        }
    }
}


void Panel::TransmitData(uint8 data)
{
    if (data_for_send.Size() < 20)
    {
        data_for_send.Push(data);
    }
}


uint16 Panel::NextData()
{
    return data_for_send.Front();
}


void Panel::Disable()
{
    isRunning = false;
}


void Panel::Enable()
{
    isRunning = true;
}


void Panel::Init()
{
    EnableLEDRegSet(false);
    EnableLEDTrig(false);
}


void Panel::EnableLEDRegSet(bool enable)
{
    uint8 data = LED_REG_SET;

    if (enable)
    {
        data |= 0x80;
    }

    TransmitData(data);
}


uint16 Panel::TranslateCommand(const uint8 *data, uint)
{
    static const int NUM_BUTTONS = 27;

    volatile uint8 data1 = data[1];
    
    if(data1 == 24)
    {
        data1 = data1;
    }

    static const uint16 commands[NUM_BUTTONS] =
    {
        KeyOld::Empty,
        KeyOld::F1,
        KeyOld::F2,
        KeyOld::F3,
        KeyOld::F4,
        KeyOld::F5,
        KeyOld::ChannelA,
        KeyOld::ChannelB,
        KeyOld::Time,
        KeyOld::Trig,
        KeyOld::Cursors,
        KeyOld::Measures,
        KeyOld::Display,
        KeyOld::Help,
        KeyOld::Start,
        KeyOld::Memory,
        KeyOld::Service,
        KeyOld::Menu,
        KeyOld::Power,
        Regulator::RangeA,
        Regulator::RangeB,
        Regulator::RShiftA,
        Regulator::RShiftB,
        Regulator::TBase,
        Regulator::TShift,
        Regulator::TrigLev,
        Regulator::Set
    };

    uint16 command = 0;

    uint8 key = data[1];

    if (key > 0 && key < NUM_BUTTONS)
    {
        command = commands[key];

        if (command >= 20)
        {
            if (data[2] == 1)       { command |= 0xC0; }        // ������� �����
            else if (data[2] == 3)  { }                         // ������� �����
            else if (data[2] == 4)  { command |= 0x80; }        // ������� ������
            else                    { command = 0; }
        }
        else
        {
            if (data[2] == 0)       { command |= 0x80; }
        }
    }

    return command;
}


void Panel::CallbackOnReceiveSPI5(const uint8 *data, uint size)
{
    uint16 command = TranslateCommand(data, size);

    if (command != 0)
    {
        input_buffer.Push(command);
        Settings::NeedSave();
    }
}


void Panel::Update()
{
    if (!input_buffer.IsEmpty())
    {
        uint16 command = input_buffer.Front();
        ProcessingCommandFromPIC(command);
    }
}
