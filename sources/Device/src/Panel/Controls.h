// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


// �������������� ������.
typedef enum
{
    B_Empty     = 0,        // ������ �� ������
    B_Channel0  = 1,        // ����� 1
    B_Service   = 2,        // ������
    B_Channel1  = 3,        // ����� 2
    B_Display   = 4,        // �������
    B_Time      = 5,        // ����
    B_Memory    = 6,        // ������
    B_Trig      = 7,        // �����
    B_Start     = 8,        // ����/�����
    B_Cursors   = 9,        // �������
    B_Measures  = 10,       // �����
    B_Power     = 11,       // �������
    B_Help      = 12,       // ������
    B_Menu      = 13,       // ����
    B_F1        = 14,       // 1
    B_F2        = 15,       // 2
    B_F3        = 16,       // 3
    B_F4        = 17,       // 4
    B_F5        = 18,       // 5
    B_NumButtons            // ����� ���������� ������
} PanelButton;

PanelButton& operator++(PanelButton &button);

// �������������� �����������.
typedef enum
{
    R_Empty     = 0,    // ��������� �� ��������
    R_Range0    = 20,   // �����/��� ���. 1
    R_RShift0   = 21,   
    R_Range1    = 22,   // �����/��� ���. 2
    R_RShift1   = 23,
    R_TBase     = 24,   // �����/���
    R_TShift    = 25,
    R_TrigLev   = 26,   // �������
    R_Set       = 27    // ���������
} Regulator;

// ����������� �������� �����������.
typedef enum
{
    Direction_Left,     // �������� ���������� �����
    Direction_Right,    // �������� ���������� ������
    Direction_None      // �������� �����������
} RegulatorDirection;


#define NUM_REGULATORS (R_Set - R_Range0 + 1)


const char* NameButton(PanelButton button);
