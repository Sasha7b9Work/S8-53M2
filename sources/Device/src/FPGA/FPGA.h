// 2022/02/11 17:48:42 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Panel/Controls.h"
#include "Settings/SettingsTrig.h"
#include "Settings/SettingsService.h"
#include "FPGA/SettingsFPGA.h"
#include "FPGA/TypesFPGA.h"
#include "FPGA/StructuresFPGA.h"


namespace FPGA
{
    static const int MAX_POINTS = 1024;

    // ���� COMPACT == true - ����� ��������� ������ ����������� ��������� ���������� �� 2 �������� �
    // ������������ �����
    static bool COMPACT = true;

    extern bool AUTO_FIND_IN_PROGRESS;

    void Init();

    // ���������� ���������� ����������� �������� � �������.
    void SetNumSignalsInSec(int numSigInSec);

    void Update();

    // ��������� ���� ����� ����������.
    void OnPressStartStop();

    // ������ �������� ����� ����������.
    void Start();

    // ��������� ������� ����� ����������.
    void Stop(bool pause);

    // ���������� true, ���� ������ ��������� �� � �������� ����� ����������.
    bool IsRunning();

    // ������� ������. ����� ��� ������ �������������, ��� ���������� ������� ����� �� �������� ���������������.
    void ClearData();

    // ���������� ���������� ���������, �� ������� ����� �������������� ������ � ������ �������������.
    void SetNumberMeasuresForGates(int number);

    // ������������� ��������� �������������.
    void SwitchingTrig();

    // ���������� ��������� ����� ����� ��������� ����� - ����� ��������� ������ ������������� �� ��������� �����
    void TemporaryPause();

    // ����� � ���������� ������� ������������� �� ���������� ���������� �������
    void FindAndSetTrigLevel();

    // ������ ������� ����������.
    void ProcedureCalibration();

    namespace FreqMeter
    {
        // �������� �������� �������.
        float GetFreq();
    }

    namespace AutoFinder
    {
        // ��������� ������� ������ �������.
        void StartAutoFind();
    }
}


namespace BUS_FPGA
{
    void WriteToHardware(uint8 *address, uint8 value, bool restart);

    // ������ � ������� ���� ������ ��������. restart - true ��������, ��� ����� ������ ����� �������� ����� ���������,
    // ���� �� ����� ������ �� ��������� � ������ �����.
    void Write(uint16 *address, uint16 value, bool restart);

    void WriteAnalog(TypeWriteAnalog::E type, uint data);

    void WriteDAC(TypeWriteDAC::E type, uint16 data);
}
