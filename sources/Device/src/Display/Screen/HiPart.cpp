// 2022/03/23 13:03:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Screen/HiPart.h"
#include "Settings/Settings.h"
#include "Menu/Pages/Definition.h"
#include "Utils/Math.h"
#include "Utils/GlobalFunctions.h"
#include "Display/Grid.h"
#include "FPGA/StructuresFPGA.h"
#include "Display/Symbols.h"
#include <cmath>


namespace HiPart
{
    // ������� �������� �������� ��������� ���������.
    static void WriteCursors();

    static void DrawHiRightPart();
}


void HiPart::Draw()
{
    WriteCursors();
    DrawHiRightPart();
}


void HiPart::WriteCursors()
{
    char buffer[20];
    int startX = 43;

    if (MODE_WORK_IS_DIRECT)
    {
        startX += 29;
    }

    int x = startX;
    int y1 = 0;
    int y2 = 9;

    if (PageCursors::NecessaryDrawCursors())
    {
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 3;
        Chan::E source = CURS_SOURCE;
        Color::E colorText = ColorChannel(source);

        if (!CURS_CNTRL_U_IS_DISABLE(source))
        {
            PText::Draw(x, y1, "1:", colorText);
            PText::Draw(x, y2, "2:");
            x += 7;
            PText::Draw(x, y1, PageCursors::GetCursVoltage(source, 0, buffer));
            PText::Draw(x, y2, PageCursors::GetCursVoltage(source, 1, buffer));
            x = startX + 49;
            float pos0 = Math::VoltageCursor(PageCursors::GetCursPosU(source, 0), SET_RANGE(source), SET_RSHIFT(source));
            float pos1 = Math::VoltageCursor(PageCursors::GetCursPosU(source, 1), SET_RANGE(source), SET_RSHIFT(source));
            float delta = std::fabsf(pos1 - pos0);
            PText::Draw(x, y1, ":dU=");
            PText::Draw(x + 17, y1, Voltage2String(delta, false, buffer));
            PText::Draw(x, y2, ":");
            PText::Draw(x + 10, y2, PageCursors::GetCursorPercentsU(source, buffer));
        }

        x = startX + 101;
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 3;

        if (!CURS_CNTRL_T_IS_DISABLE(source))
        {
            Color::SetCurrent(colorText);
            PText::Draw(x, y1, "1:");
            PText::Draw(x, y2, "2:");
            x += 7;
            PText::Draw(x, y1, PageCursors::GetCursorTime(source, 0, buffer));
            PText::Draw(x, y2, PageCursors::GetCursorTime(source, 1, buffer));
            x = startX + 153;
            float pos0 = Math::TimeCursor(CURS_POS_T0(source), SET_TBASE);
            float pos1 = Math::TimeCursor(CURS_POS_T1(source), SET_TBASE);
            float delta = std::fabsf(pos1 - pos0);
            PText::Draw(x, y1, ":dT=");
            char buf[20];
            PText::Draw(x + 17, y1, Time2String(delta, false, buf));
            PText::Draw(x, y2, ":");
            PText::Draw(x + 8, y2, PageCursors::GetCursorPercentsT(source, buf));

            if (CURSORS_SHOW_FREQ)
            {
                int width = 65;
                int x0 = Grid::Right() - width;
                Painter::DrawRectangle(x0, GRID_TOP, width, 12, COLOR_FILL);
                Painter::FillRegion(x0 + 1, GRID_TOP + 1, width - 2, 10, COLOR_BACK);
                PText::Draw(x0 + 1, GRID_TOP + 2, "1/dT=", colorText);
                char buff[20];
                PText::Draw(x0 + 25, GRID_TOP + 2, Freq2String(1.0f / delta, false, buff));
            }
        }
    }
}


void HiPart::DrawHiRightPart()
{
    // �������������
    int y = 2;

    static const int xses[3] = {280, 271, 251};
    int x = xses[MODE_WORK];

    if (!MODE_WORK_IS_LATEST)
    {
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);

        x += 2;

        if (TrigLev::fireLED)
        {
            Painter::FillRegion(x, 1 + y, GRID_TOP - 3, GRID_TOP - 7);
            PText::Draw(x + 3, 3 + y, LANG_RU ? "��" : "Tr", COLOR_BACK);
        }
    }

    // ����� ������
    static pchar strings_[][2] =
    {
        {"���",     "MEAS"},
        {"����",    "LAST"},
        {"����",    "INT"}
    };

    if (!MODE_WORK_IS_DIRECT)
    {
        x += 18;
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);
        x += 2;
        PText::Draw(LANG_RU ? x : x + 3, -1, LANG_RU ? "�����" : "mode");
        PText::DrawStringInCenterRect(x + 1, 9, 25, 8, strings_[MODE_WORK][LANG]);
    }
    else
    {
        x -= 9;
    }

    if (!MODE_WORK_IS_LATEST)
    {

        x += 27;
        Painter::DrawVLine(x, 1, GRID_TOP - 2, COLOR_FILL);

        x += 2;
        y = 1;
        if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Work)
        {
            PText::Draw4SymbolsInRect(x, 1, SYMBOL_PLAY);
        }
        else if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop)
        {
            Painter::FillRegion(x + 3, y + 3, 10, 10);
        }
        else if (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Wait)
        {
            int w = 4;
            int h = 14;
            int delta = 4;
            x = x + 2;
            Painter::FillRegion(x, y + 1, w, h);
            Painter::FillRegion(x + w + delta, y + 1, w, h);
        }
    }
}