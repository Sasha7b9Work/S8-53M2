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


#define WR_START                (HAL_FMC::_ADDR_FPGA + 0x00)    // ������ ����� ���������. ��� ������� ����� �������� 1.
#define WR_RAZV                 (HAL_FMC::_ADDR_FPGA + 0x01)     // ��������� ������� �������������. ������������ � TBase::Load().
#define WR_PRED                 (HAL_FMC::_ADDR_FPGA + 0x02)
#define WR_POST                 (HAL_FMC::_ADDR_FPGA + 0x03)
#define WR_TRIG_F               (HAL_FMC::_ADDR_FPGA + 0x04)
#define WR_UPR                  (HAL_FMC::_ADDR_FPGA + 0x05)
#define WR_STOP                 (HAL_FMC::_ADDR_FPGA + 0x1f)

//#define RD_ADDR_LAST_RECORD     (HAL_FMC::_ADDR_FPGA + 0x08) // ������ ������ ��������� ������
#define RD_FREQ_LOW             (HAL_FMC::_ADDR_FPGA + 0x10)
#define RD_FREQ_HI              (HAL_FMC::_ADDR_FPGA + 0x18)
#define RD_PERIOD_LOW           (HAL_FMC::_ADDR_FPGA + 0x20)
#define RD_PERIOD_HI            (HAL_FMC::_ADDR_FPGA + 0x28)
#define RD_FL                   (HAL_FMC::_ADDR_FPGA + 0x30)
    #define BIT_DATA_READY          0
    #define BIT_TRIG                1

struct TypeWriteAnalog
{
    enum E
    {
        All,
        Range0,
        Range1,
        TrigParam,
        ChanParam0,
        ChanParam1
    };
};


struct TypeWriteDAC
{
    enum E
    {
        RShiftA,
        RShiftB,
        TrigLev
    };
};


// ��� �������� �������� �������, ��������� � ���, ������������� ������ ������� ����� (-5 ������ �� ������). ���� �������� == 0, ������, ��� ���. ��� ����� ��� ������� ������������� � ����������� ������ p2p, � ����� ��� tShift ����� ���������� �������.
struct ValueFPGA
{
    static const uint8 MIN = 2;
    static const uint8 AVE = 127;   // ��� �������� �������� �������, ��������� � ���, ������������� ������ �����. ���� �������� == 0, ������, ��� ���. ��� ����� ��� ������� ������������� � ����������� ������ p2p, � ����� ��� tShift ����� ���������� �������.
    static const uint8 MAX = 227;   // ��� �������� �������� �������, ��������� � ���, ������������� ������� ������� ����� (+5 ������ �� ������). ���� �������� == 0, ������, ��� ���. ��� ����� ��� ������� ������������� � ����������� ������ p2p, � ����� ��� tShift ����� ���������� �������.
};
