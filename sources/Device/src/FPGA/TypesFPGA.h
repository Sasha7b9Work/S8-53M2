#pragma once

/** @file FPGA_Types.h
    @brief ���� �������� �������, ������ � � ���������, ����������� ��� ������ � ��������
    @verbatim
    ����� ������� ��������� ������� ���������� ��������� ������� 2-15
    ������� WR_START ����������� ������ ��� ��� ������� ����� ��������������
    ������� WR_STOP ����������� ��� ����������� ����� �������������� ����� ����� ������


    ��� ���������� �������� ������� � �������������, ���������� ������� � ������������� ������������ ������ PG2, PG3, PG5, PG7
    PG5 - ����� ������� (���������)
    PG7 - ����� �������� (���������)
    PG2 - �������� ��������
    PG3 - ������

    �������� ������� ������ - ������.
    ����� ������� ���������� ������� ��������� �������� ������ ���� ����� ����.
    ������ ���������� �� ����� ��������� ��������.
    ���������� AD5314.
                            __      __      __      __         __      __      __      __      __
                           /  \    /  \    /  \    /  \       /  \    /  \    /  \    /  \    /  \
    PG2       ____________/    \__/    \__/    \__/    \__/  /    \__/    \__/    \__/    \__/    \_____________
              _________                                                                                _________
    PG5 (PG7)          \___________________________________   ________________________________________/
                             ____    ____    ____    ____       ____    ____    ____    ____    ____                                                              
    PG3       ______________/    \__/    \__/    \__/    \_   _/    \__/    \__/    \__/    \__/    \___________
                            \____/  \____/  \____/  \____/     \____/  \____/  \____/  \____/  \____/                                                    
                             DB15    DB14    DB13    DB12       DB4     DB3     DB2     DB1     DB0

    @endverbatim 
*/


enum TypeWriteAnalog
{
    TypeWriteAnalog_All,
    TypeWriteAnalog_Range0,
    TypeWriteAnalog_Range1,
    TypeWriteAnalog_TrigParam,
    TypeWriteAnalog_ChanParam0,
    TypeWriteAnalog_ChanParam1
};
