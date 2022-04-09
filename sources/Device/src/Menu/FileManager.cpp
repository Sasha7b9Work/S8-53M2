// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "Menu/FileManager.h"
#include "Menu/Menu.h"
#include "Settings/Settings.h"
#include "Settings/SettingsMemory.h"
#include "Display/Colors.h"
#include "Display/Display.h"
#include "Display/font/Font.h"
#include "Display/Screen/Grid.h"
#include "Utils/Math.h"
#include "Hardware/Sound.h"
#include "Hardware/FDrive/FDrive.h"
#include "Panel/Panel.h"
#include "Log.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Utils/Text/Text.h"
#include <cstring>


namespace FM
{
    const int RECS_ON_PAGE = 23;
    const int WIDTH_COL = 135;

    char currentDir[255] = "\\";
    int numFirstDir = 0;        // ����� ������� ����������� �������� � ����� ������. ����� ����� ���� �������� RECS_ON_PAGE ���������
    int numCurDir = 0;          // ����� ������������� ��������
    int numFirstFile = 0;       // ����� ������� ����������� ����� � ������ ������. ����� ����� ���� �������� RECS_ON_PAGE ������.
    int numCurFile = 0;         // ����� ������������� �����
    int numDirs = 0;
    int numFiles = 0;

    bool cursorInDirs = true;   // ���� true, �� ����� ��������� ���������� �� ���������
    int  needRedraw = 1;
    bool needOpen = false;

    bool FileIsExist(char name[256]);

    void DrawLongString(int x, int y, char *string, bool hightlight);

    void DrawDirs(int x, int y);

    void DrawFiles(int x, int y);

    void DecCurrentDir();

    void DecCurrentFile();

    void DrawHat(int x, int y, char *string, int num1, int num2);

    void DrawNameCurrentDir(int left, int top);

    void IncCurrentDir();

    void IncCurrentFile();
}



void FM::Init()
{
    std::strcpy(currentDir, "\\");
    numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
}


void FM::DrawLongString(int x, int y, char *string, bool hightlight)
{
    int length = Font::GetLengthText(string);

    Color::E color = COLOR_FILL;
    if (hightlight)
    {
        Region(WIDTH_COL + 9, 8).Fill(x - 1, y, color);
        color = COLOR_BACK;
    }

    if (length <= WIDTH_COL)
    {
        DisplayString(string).Decode().Draw(x, y, color);
    }
    else
    {
        DisplayString(string).Decode().DrawWithLimitation(x, y, color, x, y, WIDTH_COL, 10);
        String<>("...").Draw(x + WIDTH_COL + 3, y);
    }
}


void FM::DrawHat(int x, int y, char *string, int num1, int num2)
{
    Region(WIDTH_COL + 9, RECS_ON_PAGE * 9 + 11).Fill(x - 1, y, COLOR_BACK);
    String<>(string, num1, num2).Draw(x + 60, y, COLOR_FILL);
    Painter::DrawHLine(true, y + 10, x + 2, x + 140);
}


void FM::DrawDirs(int x, int y)
{
    Directory::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
    DrawHat(x, y, "������� : %d/%d", numCurDir + ((numDirs == 0) ? 0 : 1), numDirs);
    char nameDir[256];
    Directory directory;
    y += 12;

    if (directory.GetName(currentDir, numFirstDir, nameDir))
    {
        int  drawingDirs = 0;
        DrawLongString(x, y, nameDir, cursorInDirs && ( numFirstDir + drawingDirs == numCurDir));

        while (drawingDirs < (RECS_ON_PAGE - 1) && directory.GetNextName(nameDir))
        {
            drawingDirs++;
            DrawLongString(x, y + drawingDirs * 9, nameDir, cursorInDirs && ( numFirstDir + drawingDirs == numCurDir));
        }
    }

    directory.Close();
}


void FM::DrawFiles(int x, int y)
{
    Directory::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
    DrawHat(x, y, "���� : %d/%d", numCurFile + ((numFiles == 0) ? 0 : 1), numFiles);
    char nameFile[256];
    Directory directory;
    y += 12;

    if (directory.GetNameFile(currentDir, numFirstFile, nameFile))
    {
        int drawingFiles = 0;
        DrawLongString(x, y, nameFile, !cursorInDirs && (numFirstFile + drawingFiles == numCurFile));

        while (drawingFiles < (RECS_ON_PAGE - 1) && directory.GetNextNameFile(nameFile))
        {
            drawingFiles++;
            DrawLongString(x, y + drawingFiles * 9, nameFile, !cursorInDirs && (numFirstFile + drawingFiles == numCurFile));
        }
    }

    directory.Close();
}


bool FM::FileIsExist(char name[256])
{
    char nameFile[256];
    Directory::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
    Directory directory;

    if(directory.GetNameFile(currentDir, 0, nameFile))
    {
        while(directory.GetNextNameFile(nameFile))
        {
            if(std::strcmp(name + 2, nameFile) == 0)
            {
                return true;
            }
        }
    }
    return false;
}


void FM::DrawNameCurrentDir(int left, int top)
{
    Color::SetCurrent(COLOR_FILL);
    int length = Font::GetLengthText(currentDir);
    if (length < 277)
    {
        DisplayString(currentDir).Decode().Draw(left + 1, top + 1);
    }
    else
    {
        char *pointer = currentDir + 2;
        while (length > 277)
        {
            while (*pointer != '\\' && pointer < currentDir + 255)
            {
                pointer++;
            }
            if (pointer >= currentDir + 255)
            {
                return;
            }
            length = Font::GetLengthText(++pointer);
        }

        DisplayString(pointer).Decode().Draw(left + 1, top + 1);
    }
}


void FM::Draw()
{
    if (needRedraw == 0)
    {
        return;
    }

    int left = 1;
    int top = 1;
    int width = 297;
    int left2col = width / 2;

    if (needRedraw == 1)
    {
        Painter::BeginScene(COLOR_BACK);
        Menu::Draw();
        Rectangle(width, 239).Draw(0, 0, COLOR_FILL);
        Directory::GetNumDirsAndFiles(currentDir, &numDirs, &numFiles);
        DrawNameCurrentDir(left + 1, top + 2);
        Painter::DrawVLine(true, left2col, top + 16, 239, COLOR_FILL);
        Painter::DrawHLine(true, top + 15, 0, width);
    }

    if (needRedraw != 3)
    {
        DrawDirs(left + 2, top + 18);
    }

    if (needRedraw != 2)
    {
        DrawFiles(left2col + 3, top + 18);
    }

    Painter::EndScene();

    needRedraw = 0;
}


void FM::PressTab()
{
    needRedraw = 1;

    if (cursorInDirs)
    {
        if (numFiles != 0)
        {
            cursorInDirs = false;
        }
    }
    else
    {
        if (numDirs != 0)
        {
            cursorInDirs = true;
        }
    }
}


void FM::PressLevelDown()
{
    needRedraw = 1;

    if (!cursorInDirs)
    {
        return;
    }

    char nameDir[100];
    Directory directory;

    if (directory.GetName(currentDir, numCurDir, nameDir))
    {
        if (std::strlen(currentDir) + std::strlen(nameDir) < 250)
        {
            directory.Close();
            std::strcat(currentDir, "\\");
            std::strcat(currentDir, nameDir);
            numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
        }

    }

    directory.Close();
}


void FM::PressLevelUp()
{
    needRedraw = 1;

    if (std::strlen(currentDir) == 1)
    {
        return;
    }

    char *pointer = currentDir + std::strlen(currentDir);

    while (*pointer != '\\')
    {
        pointer--;
    }

    *pointer = '\0';
    numFirstDir = numFirstFile = numCurDir = numCurFile = 0;
    cursorInDirs = true;
}


void FM::IncCurrentDir()
{
    if (numDirs > 1)
    {
        numCurDir++;
        if (numCurDir > numDirs - 1)
        {
            numCurDir = 0;
            numFirstDir = 0;
        }
        if (numCurDir - numFirstDir > RECS_ON_PAGE - 1)
        {
            numFirstDir++;
        }
    }
}


void FM::DecCurrentDir()
{
    if (numDirs > 1)
    {
        numCurDir--;
        if (numCurDir < 0)
        {
            numCurDir = numDirs - 1;
            LIMITATION(numFirstDir, numDirs - RECS_ON_PAGE, 0, numCurDir);
        }
        if (numCurDir < numFirstDir)
        {
            numFirstDir = numCurDir;
        }
    }
}


void FM::IncCurrentFile()
{
    if (numFiles > 1)
    {
        numCurFile++;
        if (numCurFile > numFiles - 1)
        {
            numCurFile = 0;
            numFirstFile = 0;
        }
        if (numCurFile - numFirstFile > RECS_ON_PAGE - 1)
        {
            numFirstFile++;
        }
    }
}


void FM::DecCurrentFile()
{
    if (numFiles > 1)
    {
        numCurFile--;
        if (numCurFile < 0)
        {
            numCurFile = numFiles - 1;
            LIMITATION(numFirstFile, numFiles - RECS_ON_PAGE, 0, numCurFile);
        }
        if (numCurFile < numFirstFile)
        {
            numFirstFile = numCurFile;
        }
    }
}


void FM::RotateRegSet(int angle)
{
    if (cursorInDirs)
    {
        angle > 0 ? DecCurrentDir() : IncCurrentDir();
        needRedraw = 2;
    }
    else
    {
        angle > 0 ? DecCurrentFile() : IncCurrentFile();
        needRedraw = 3;
    }
}


String<> FM::GetNameForNewFile()
{
    int number = 1;

LabelNextNumber:

    String<> result(currentDir);
    result.Append("\\");

    int size = (int)std::strlen(FILE_NAME);
    if (size == 0)
    {
        return String<>("");
    }

    if (FILE_NAMING_MODE_IS_HAND)
    {
        LIMITATION(size, size, 1, 95);
        result.Append(FILE_NAME);
        result.Append(".");
        result.Append(MODE_SAVE_SIGNAL_IS_BMP ? "bmp" : "txt");
    }
    else
    {
        PackedTime time = HAL_RTC::GetPackedTime();
                           //  1          2           3         4           5             6
        uint values[] = {0, time.year, time.month, time.day, time.hours, time.minutes, time.seconds};

        char *ch = FILE_NAME_MASK;

        while (*ch)
        {
            if (*ch >= 0x30)        // ���� ��������� ������
            {
                result.Append(*ch);          // �� ���������� ��� � ��� �����
            }
            else
            {
                if (*ch == 0x07)    // ���� ����� ���� �������� ���������� �����
                {
                    String<> number_str = SU::Int2String(number, false, *(ch + 1));
                    result.Append(number_str);
                    ch++;
                }
                else
                {
                    if (*ch >= 0x01 && *ch <= 0x06)
                    {
                        String<> value_str = SU::Int2String((int)values[*ch], false, 2);
                        result.Append(value_str);
                    }
                }
            }
            ch++;
        }

        result.Append(MODE_SAVE_SIGNAL_IS_BMP ? ".bmp" : ".txt");

        if(FileIsExist(result.c_str()))
        {
            number++;
            goto LabelNextNumber;
        }
    }

    return result;
}
