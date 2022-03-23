// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Painter.h"
#include "Hardware/Timer.h"
#include "Hardware/FSMC.h"
#include "Settings/Settings.h"
#include "Utils/Math.h"



static bool inverseColors = false;
static Color::E currentColor = Color::COUNT;
static bool framesElapsed = false;

static enum StateTransmit
{
    StateTransmit_Free,
    StateTransmit_NeedForTransmitFirst,  // ��� ����� ����� �������� ������ ���� - ���������� ������
    StateTransmit_NeedForTransmitSecond, // ��� ����� ����� �������� ������ � ����������� ����� - ������ �� ����������
    StateTransmit_InProcess
} stateTransmit = StateTransmit_Free;



void Painter_SendFrame(bool first)
{
    if (stateTransmit == StateTransmit_Free)
    {
        stateTransmit = (first ? StateTransmit_NeedForTransmitFirst : StateTransmit_NeedForTransmitSecond);
    }
}


void CalculateCurrentColor()
{
    if (currentColor == Color::FLASH_10)
    {
        Painter_SetColor(inverseColors ? gColorBack : gColorFill);
    }
    else if (currentColor == Color::FLASH_01)
    {
        Painter_SetColor(inverseColors ? gColorFill : gColorBack);
    }
}


void CalculateColor(uint8 *color)
{
    currentColor = (Color::E)*color;
    if (*color == Color::FLASH_10)
    {
        *color = inverseColors ? (uint8)gColorBack : (uint8)gColorFill;
    }
    else if (*color == Color::FLASH_01)
    {
        *color = inverseColors ? (uint8)gColorFill : (uint8)gColorBack;
    }
}


void InverseColor(Color::E *color)
{
    *color = (*color == Color::BLACK) ? Color::WHITE : Color::BLACK;
}


static void OnTimerFlashDisplay()
{
    inverseColors = !inverseColors;
}


void Painter_ResetFlash()
{
    Timer_Enable(kFlashDisplay, 400, OnTimerFlashDisplay);
    inverseColors = false;
}


void Painter_DrawRectangle(int x, int y, int width, int height)
{
    Painter_DrawHLine(y, x, x + width);
    Painter_DrawVLine(x, y, y + height);
    Painter_DrawHLine(y + height, x, x + width);
    if (x + width < SCREEN_WIDTH)
    {
        Painter_DrawVLine(x + width, y, y + height);
    }
}


void Painter_DrawDashedVLine(int x, int y0, int y1, int deltaFill, int deltaEmtpy, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmtpy))
    {
        return;
    }
    int y = y0;
    if (deltaStart != 0)                 // ���� ����� ����� �������� �� � ������ ������
    {
        y += (deltaFill + deltaEmtpy - deltaStart);
        if (deltaStart < deltaFill)     // ���� ������ ����� ���������� �� �����
        {
            Painter_DrawVLine(x, y0, y - 1);
        }
    }

    while (y < y1)
    {
        Painter_DrawVLine(x, y, y + deltaFill - 1);
        y += (deltaFill + deltaEmtpy);
    }
}


void Painter_DrawDashedHLine(int y, int x0, int x1, int deltaFill, int deltaEmpty, int deltaStart)
{
    if (deltaStart < 0 || deltaStart >= (deltaFill + deltaEmpty))
    {
        return;
    }
    int x = x0;
    if (deltaStart != 0)                // ���� ����� ����� �������� �� � ������ ������
    {
        x += (deltaFill + deltaEmpty - deltaStart);
        if (deltaStart < deltaFill)     // ���� ������ ����� ���������� �� �����
        {
            Painter_DrawHLine(y, x0, x - 1);
        }
    }

    while (x < x1)
    {
        Painter_DrawHLine(y, x, x + deltaFill - 1);
        x += (deltaFill + deltaEmpty);
    }
}


void Painter_SendToDisplay(uint8 *bytes, int numBytes)
{
    for (int i = 0; i < numBytes; i += 4)
    {
        while (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) == GPIO_PIN_RESET) {};
        Timer_PauseOnTicks(50);
        *ADDR_CDISPLAY = *bytes++;
        *ADDR_CDISPLAY = *bytes++;
        *ADDR_CDISPLAY = *bytes++;
        *ADDR_CDISPLAY = *bytes++;
    }
}


void Painter_SetColor(Color::E color)
{
    if (color != currentColor)
    {
        currentColor = color;
        if (currentColor > Color::COUNT)
        {
            CalculateColor((uint8 *)&color);
        }
        uint8 command[4] = {SET_COLOR};
        command[1] = (uint8)color;
        Painter_SendToDisplay(command, 4);
    }
}


void Painter_DrawHLine(int y, int x0, int x1)
{
    CalculateCurrentColor();
    uint8 command[8];
    command[0] = DRAW_HLINE;
    *(command + 1) = (uint8)y;
    *((int16*)(command + 2)) = (int16)x0;
    *((int16*)(command + 4)) = (int16)x1;
    Painter_SendToDisplay(command, 8);
}


void Painter_DrawVLine(int x, int y0, int y1)
{
    CalculateCurrentColor();
    uint8 command[8];
    command[0] = DRAW_VLINE;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (uint8)y0;
    *(command + 4) = (uint8)y1;
    Painter_SendToDisplay(command, 8);
}


void Painter_DrawVPointLine(int x, int y0, int y1, float delta, Color::E color)
{
    Painter_SetColor(color);
    int8 numPoints = (int8)((y1 - y0) / delta);
    uint8 command[6];
    command[0] = DRAW_VPOINT_LINE;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (uint8)y0;
    *(command + 4) = (uint8)delta;
    *(command + 5) = (uint8)numPoints;
    Painter_SendToDisplay(command, 6);
}


void Painter_DrawHPointLine(int y, int x0, int x1, float delta)
{
    for (int x = x0; x <= x1; x += (int)delta)
    {
        Painter_SetPoint(x, y);
    }
}


void Painter_SetPoint(int x, int y)
{
    uint8 command[4];
    command[0] = SET_POINT;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (uint8)y;
    Painter_SendToDisplay(command, 4);
}


static void DrawMultiVPointLineColorEmulate(int numLines, int y, uint16 x[], int delta, int count, Color::E color)
{
    for (int i = 0; i < numLines; i++)
    {
        Painter_DrawVPointLine(x[i], y, y + count * delta, (float)delta, color);
    }
}


void Painter_DrawMultiVPointLine(int numLines, int y, uint16 x[], int delta, int count, Color::E color) 
{
    /*
    BUG
    ���-�� � �������� ������� �������� ��� - ��� ��������� ������ ����� ��� ����������� ������
    � y > 66 ������� ������ � ������������� ��������
    */
    if(numLines > 20) 
    {
        return;
    }

    if (y > 66) 
    {
        DrawMultiVPointLineColorEmulate(numLines, y, x, delta, count, color);
        return;
    }

    Painter_SetColor(color);
    uint8 command[60];
    command[0] = DRAW_MULTI_VPOINT_LINES;
    *(command + 1) = (uint8)numLines;
    *(command + 2) = (uint8)y;
    *(command + 3) = (uint8)count;
    *(command + 4) = (uint8)delta;
    *(command + 5) = 0;
    uint8 *pointer = command + 6;
    for(int i = 0; i < numLines; i++) 
    {
        *((uint16*)pointer) = x[i];
        pointer += 2;
    }
    int numBytes = 1 + 1 + 1 + numLines * 2 + 1 + 1;
    while(numBytes % 4) 
    {
        numBytes++;
    }
    Painter_SendToDisplay(command, numBytes);
}


void Painter_DrawMultiHPointLine(int numLines, int x, uint8 y[], int delta, int count, Color::E color)
{
    if (numLines > 20)
    {
        return;
    }
    Painter_SetColor(color);
    uint8 command[30];
    command[0] = DRAW_MULTI_HPOINT_LINES_2;
    *(command + 1) = (uint8)numLines;
    *((uint16*)(command + 2)) = (uint8)x;
    *(command + 4) = (uint8)count;
    *(command + 5) = (uint8)delta;
    uint8 *pointer = command + 6;
    for (int i = 0; i < numLines; i++)
    {
        *pointer = y[i];
        pointer++;
    }
    int numBytes = 1 +     // command
        1 +     // numLines
        2 +     // x
        numLines +    // numLines
        1 +
        1;
    while (numBytes % 4)
    {
        numBytes++;
    }
    Painter_SendToDisplay(command, numBytes);
}


void Painter_DrawLine(int x0, int y0, int x1, int y1)
{
    if (x0 == x1)
    {
        Painter_DrawVLine(x0, y0, y1);
    }
    else if (y0 == y1)
    {
        Painter_DrawHLine(y0, x0, x1);
    }
}


void Painter_FillRegion(int x, int y, int width, int height)
{
    CalculateCurrentColor();
    uint8 command[8];
    command[0] = FILL_REGION;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (uint8)y;
    *((int16*)(command + 4)) = (int16)width;
    *(command + 6) = (uint8)height;
    Painter_SendToDisplay(command, 8);
}


void Painter_DrawVolumeButton(int x, int y, int width, int height, int thickness, Color::E normal, Color::E bright, Color::E dark, bool isPressed, bool inShade)
{
    if (inShade)
    {
        thickness = 1;
    }
    Painter_FillRegionC(x + thickness, y + thickness, width - thickness * 2, height - thickness * 2, normal);
    if (isPressed || inShade)
    {
        for (int i = 0; i < thickness; i++)
        {
            Painter_DrawHLineC(y + i, x + i, x + width - i, dark);
            Painter_DrawVLine(x + i, y + 1 + i, y + height - i);
            Painter_DrawVLineC(x + width - i, y + 1 + i, y + height - i, bright);
            Painter_DrawHLine(y + height - i, x + 1 + i, x + width - i);
        }
    }
    else
    {
        for (int i = 0; i < thickness; i++)
        {
            Painter_DrawHLineC(y + i, x + i, x + width - i, bright);
            Painter_DrawVLine(x + i, y + 1 + i, y + height - i);
            Painter_DrawVLineC(x + width - i, y + 1 + i, y + height - i, dark);
            Painter_DrawHLine(y + height - i, x + 1 + i, x + width - i);
        }
    }
}


void Painter_SetBrightnessDisplay(int16 brightness)
{
    float recValue = 1601.0f;
    if (brightness < 100)
    {
        recValue = 64.0f + (600.0f - 63.0f) / 100.0f / 100.0f * brightness * brightness;
    }
    uint8 command[4];
    command[0] = SET_BRIGHTNESS;
    *((uint16*)(command + 1)) = (uint16)recValue;
    Painter_SendToDisplay(command, 4);
}


void Painter_DrawVLineArray(int x, int numLines, uint8 *y0y1, Color::E color)
{
    Painter_SetColor(color);
    uint8 command[255 * 2 + 4 + 4];
    command[0] = DRAW_VLINES_ARRAY;
    *((int16*)(command + 1)) = (int16)x;
    if (numLines > 255)
    {
        numLines = 255;
    }
    *(command + 3) = (uint8)numLines;
    for (int i = 0; i < numLines; i++)
    {
        *(command + 4 + i * 2) = *(y0y1 + i * 2);
        *(command + 4 + i * 2 + 1) = *(y0y1 + i * 2 + 1);
    }
    int numBytes = numLines * 2 + 4;
    while (numBytes % 4)
    {
        numBytes++;
    }
    Painter_SendToDisplay(command, numBytes);
}


void Painter_DrawSignal(int x, uint8 data[281], bool modeLines)
{
    uint8 command[284];
    command[0] = (uint8)(modeLines ? DRAW_SIGNAL_LINES : DRAW_SIGNAL_POINTS);
    *((int16*)(command + 1)) = (int16)x;
    for (int i = 0; i < 281; i++)
    {
        *(command + 3 + i) = data[i];
    }
    Painter_SendToDisplay(command, 284);
}


void Painter_BeginScene(Color::E color)
{
    if (stateTransmit == StateTransmit_NeedForTransmitFirst || stateTransmit == StateTransmit_NeedForTransmitSecond)
    {
        bool needForLoadFontsAndPalette = stateTransmit == StateTransmit_NeedForTransmitFirst;
        stateTransmit = StateTransmit_InProcess;
        if(needForLoadFontsAndPalette) 
        {
            Painter_LoadFont(TypeFont_5);
            Painter_LoadFont(TypeFont_8);
            Painter_LoadFont(TypeFont_UGO);
            Painter_LoadFont(TypeFont_UGO2);
        }
    }

    Painter_FillRegionC(0, 0, 319, 239, color);
}


static void Painter_RunDisplay()
{
    uint8 command[4];
    command[0] = RUN_BUFFER;
    Painter_SendToDisplay(command, 4);
}


void Painter_EndScene()
{
    if (!framesElapsed)
    {
        framesElapsed = true;
        return;
    }
    uint8 command[4];
    command[0] = END_SCENE;
    Painter_SendToDisplay(command, 4);
    if (stateTransmit == StateTransmit_InProcess)
    {
        stateTransmit = StateTransmit_Free;
    }

    Painter_RunDisplay();
}


Color::E GetColor(int x, int y)
{
    uint8 command[4];
    command[0] = GET_POINT;
    *((int16*)(command + 1)) = (int16)x;
    *(command + 3) = (uint8)y;
    Painter_SendToDisplay(command, 4);

    return (Color::E)(command[0] & 0x0f);
}


void Painter_DrawPicture(int x, int y, int width, int height, uint8 *address)
{
    uint8 command[4];
    command[0] = DRAW_PICTURE;
    *((uint16*)(command + 1)) = (uint8)x;
    *(command + 3) = (uint8)y;
    Painter_SendToDisplay(command, 4);
    *((uint16*)(command)) = (uint8)width;
    *(command + 2) = (uint8)height;
    *(command + 3) = 0;
    Painter_SendToDisplay(command, 4);
    for (int i = 0; i < width * height / 2 / 4; i++)
    {
        *(command) = *address++;
        *(command + 1) = *address++;
        *(command + 2) = *address++;
        *(command + 3) = *address++;
        Painter_SendToDisplay(command, 4);
    }
}
