// 2022/02/18 15:30:12 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FPGA/FPGA.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Panel/Panel.h"
#include "Hardware/HAL/HAL.h"
#include "FPGA/Storage.h"
#include "Utils/Math.h"
#include "Utils/ProcessingSignal.h"
#include <stm32f4xx_hal.h>
#include <string.h>


#define WRITE_AND_OR_INVERSE(addr, data, ch)                                                                        \
    if(SET_INVERSE(ch))                                                                                             \
    {                                                                                                               \
        data = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(data, ValueFPGA::MIN, ValueFPGA::MAX));  \
    }                                                                                                               \
    *addr = data;


namespace FPGA
{
    static const int num_Kr = 200;
    const int Kr[] = {num_Kr / 2, num_Kr / 5, num_Kr / 10, num_Kr / 20, num_Kr / 50};

    uint timeStart = 0;

    DataSettings ds;

    uint8 dataRel0[FPGA::MAX_POINTS] = {0};   // ����� ������������ ��� ������ ������ ������� ������.
    uint8 dataRel1[FPGA::MAX_POINTS] = {0};   // ����� ������������ ��� ������ ������ ������� ������.

    int additionShift = 0;

    uint8 ReadFlag();

    void WriteToDAC(TypeWriteDAC::E, uint16);

    void WriteToAnalog(TypeWriteAnalog::E type, uint data);

    void AutoFind();

    void ReadPoint();

    bool ProcessingData();

    // ��������� ������.
    // necessaryShift - ������� ����, ��� ������ ����� �������.
    // saveToStorage - ����� � ������ ������������� ��� ��������, ��� ���� ��������� ���������
    void DataRead(bool necessaryShift, bool saveToStorage);

    bool FindWave(Chan::E);

    void ReadRandomizeMode();

    void ReadRealMode(bool necessaryShift);

    // ����������� ������.
    void InverseDataIsNecessary(Chan::E, uint8 *data);

    int CalculateShift();

    bool CalculateGate(uint16 rand, uint16 *min, uint16 *max);

    Range::E AccurateFindRange(Chan::E);

    TBase::E AccurateFindTBase(Chan::E);

    // ��������� ��������� � ���������� ����� �� ���������� ��������� SSettings.
    void LoadSettings();

    uint8 CalculateMinWithout0(uint8 buffer[100]);

    uint8 CalculateMaxWithout255(uint8 buffer[100]);

    TBase::E FindTBase(Chan::E);

    TBase::E CalculateTBase(float freq_);



    namespace FreqMeter
    {
        extern float freq;           // �������, ���������� ��������.

        float CalculateFreqFromCounterFreq();

        float CalculateFreqFromCounterPeriod();

        void Update(uint16 flag);
    }
}



void FPGA::Init()
{

}


void FPGA::Update()
{
    ReadFlag();

    if (gStateFPGA.needCalibration)              // ���� ����� � ����� ���������� -
    {
        FPGA::ProcedureCalibration();            // ��������� �.
        gStateFPGA.needCalibration = false;
    }
    if (FPGA_TEMPORARY_PAUSE)
    {
        return;
    }

    if (AUTO_FIND_IN_PROGRESS)
    {
        AutoFind();
        return;
    }

    if ((FPGA_CAN_READ_DATA == 0) || (StateWorkFPGA::GetCurrent() == StateWorkFPGA::Stop))
    {
        return;
    }

    if (SET_SELFRECORDER)
    {
        ReadPoint();
    }

    ProcessingData();

    FPGA_CAN_READ_DATA = 0;
}


bool FPGA::ProcessingData()
{
    bool retValue = false;

    int num = (TBase::InRandomizeMode() && (!START_MODE_IS_SINGLE) && SAMPLE_TYPE_IS_EQUAL) ? Kr[SET_TBASE] : 1;

    for (int i = 0; i < num; i++)
    {
        uint8 flag = ReadFlag();

        if (FPGA_CRITICAL_SITUATION)
        {
            if (gTimerMS - timeStart > 500)
            {
                SwitchingTrig();
                TRIG_AUTO_FIND = 1;
                FPGA_CRITICAL_SITUATION = 0;
            }
            else if (_GET_BIT(flag, FL_TRIG))
            {
                FPGA_CRITICAL_SITUATION = 0;
            }
        }
        else if (_GET_BIT(flag, FL_DATA))
        {
            if (set.debug.showRegisters.flag)
            {
                char buffer[9];
                LOG_WRITE("���� ���������� %s", Bin2String(flag, buffer));
            }

            Panel::EnableLEDTrig(true);
            FPGA::Stop(true);
            DataRead(_GET_BIT(flag, BIT_SIGN_SHIFT_POINT), (num == 1) || (i == num - 1));
            retValue = true;

            if (!START_MODE_IS_SINGLE)
            {
                Start();
                StateWorkFPGA::SetCurrent(StateWorkFPGA::Work);
            }
            else
            {
                FPGA::Stop(false);
            }
        }
        else
        {
            if (flag & (1 << 2))
            {
                if (START_MODE_IS_AUTO)
                {
                    FPGA_CRITICAL_SITUATION = 1;
                }
                timeStart = gTimerMS;
            }
        }

        Panel::EnableLEDTrig(_GET_BIT(flag, FL_TRIG) ? true : false);
    }

    return retValue;
}


void FPGA::SwitchingTrig()
{
    static const uint16 value[2][2] =
    {
        {1, 0},
        {0, 1}
    };

    HAL_FMC::Write(WR_TRIG_F, value[TRIG_POLARITY][0]);
    HAL_FMC::Write(WR_TRIG_F, value[TRIG_POLARITY][1]);
}


void FPGA::Start()
{

}


bool FPGA::IsRunning()
{
    return false;
}


void FPGA::Stop(bool)
{

}


void FPGA::OnPressStartStop()
{

}


void FPGA::SetAdditionShift(int)
{

}


void FPGA::SetNumSignalsInSec(int)
{

}


void FPGA::FindAndSetTrigLevel()
{

}


void FPGA::TemporaryPause()
{

}


void FPGA::StartAutoFind()
{

}


void FPGA::SetNumberMeasuresForGates(int)
{

}


void FPGA::WriteToDAC(TypeWriteDAC::E, uint16)
{

}


void FPGA::WriteToAnalog(TypeWriteAnalog::E, uint)
{

}


void FPGA::WriteToHardware(uint8 *, uint8, bool)
{

}


void FPGA::WriteToHardware(uint16 *, uint16, bool)
{

}


void FPGA::AutoFind()
{
    if (!FindWave(Chan::A))
    {
        if (!FindWave(Chan::B))
        {
            Display::ShowWarningBad(SignalNotFound);
        }
    }

    Init();
    Start();

    AUTO_FIND_IN_PROGRESS = 0;
}


void FPGA::DataRead(bool necessaryShift, bool saveToStorage)
{
    Panel::EnableLEDTrig(false);
    FPGA_IN_PROCESS_READ = 1;

    if ((TBase::E)ds.tBase < TBase::_100ns)
    {
        ReadRandomizeMode();
    }
    else
    {
        ReadRealMode(necessaryShift);
    }

    static uint prevTime = 0;

    if (saveToStorage || (gTimerMS - prevTime > 500))
    {
        prevTime = gTimerMS;
        if (!TBase::InRandomizeMode())
        {
            InverseDataIsNecessary(Chan::A, dataRel0);
            InverseDataIsNecessary(Chan::B, dataRel1);
        }

        Storage::AddData(dataRel0, dataRel1, ds);

        if (TRIG_MODE_FIND_IS_AUTO && TRIG_AUTO_FIND)
        {
            FPGA::FindAndSetTrigLevel();
            TRIG_AUTO_FIND = 0;
        }
    }
    FPGA_IN_PROCESS_READ = 0;
}


void FPGA::ReadRandomizeMode()
{
    int Tsm = CalculateShift();
    if (Tsm == TShift::EMPTY)
    {
        return;
    };

    int step = Kr[SET_TBASE];
    int index = Tsm - step - additionShift;

    if (index < 0)
    {
        index += step;        // WARN
    }

    uint8 *pData0 = &dataRel0[index];
    const uint8 *const pData0Last = &dataRel0[FPGA::MAX_POINTS - 1];
    uint8 *pData1 = &dataRel1[index];
    const uint8 *const pData1Last = &dataRel1[FPGA::MAX_POINTS - 1];

    const uint8 *const first0 = &dataRel0[0];
    const uint8 *const last0 = pData0Last;
    const uint8 *const first1 = &dataRel1[0];
    const uint8 *const last1 = pData1Last;

    int numAve = NUM_AVE_FOR_RAND;

    if (sDisplay_NumAverage() > numAve)
    {
        numAve = sDisplay_NumAverage();
    }

    if (START_MODE_IS_SINGLE || SAMPLE_TYPE_IS_REAL)
    {
        FPGA::ClearData();
    }

    BitSet16 dataA;
    BitSet16 dataB;

    while (pData0 < &dataRel0[FPGA::MAX_POINTS])
    {
        dataA.half_word = *RD_ADC_A;
        dataB.half_word = *RD_ADC_B;

        if (pData0 >= first0 && pData0 <= last0)
        {
            WRITE_AND_OR_INVERSE(pData0, dataA.byte[0], Chan::A);
        }

        if (pData1 >= first1 && pData1 <= last1)
        {
            WRITE_AND_OR_INVERSE(pData1, dataB.byte[0], Chan::B);
        }

        pData0 += step;
        pData1 += step;

        if (pData0 >= first0 && pData0 <= last0)
        {
            WRITE_AND_OR_INVERSE(pData0, dataA.byte[1], Chan::A);
        }

        if (pData1 >= first1 && pData1 <= last1)
        {
            WRITE_AND_OR_INVERSE(pData1, dataB.byte[1], Chan::B);
        }

        pData0 += step;
        pData1 += step;
    }

    if (START_MODE_IS_SINGLE || SAMPLE_TYPE_IS_REAL)
    {
        Processing::InterpolationSinX_X(dataRel0, SET_TBASE);
        Processing::InterpolationSinX_X(dataRel1, SET_TBASE);
    }
}


void FPGA::ReadRealMode(bool necessaryShift)
{
    uint8 *p0 = &dataRel0[0];
    uint8 *p1 = &dataRel1[0];
    uint8 *endP = &dataRel0[FPGA::MAX_POINTS];

    if (ds.peakDet != PeackDetMode::Disable)
    {
        uint8 *p0min = p0;
        uint8 *p0max = p0min + 512;
        uint8 *p1min = p1;
        uint8 *p1max = p1min + 512;

        BitSet16 data;

        while (p0max < endP && FPGA_IN_PROCESS_READ)
        {
            data.half_word = *RD_ADC_B;
            *p1max++ = data.byte0;
            *p1min++ = data.byte1;

            data.half_word = *RD_ADC_A;
            *p0max++ = data.byte0;
            *p0min++ = data.byte1;
        }
    }
    else
    {
        BitSet16 data;

        while (p0 < endP && FPGA_IN_PROCESS_READ)
        {
            data.half_word = *RD_ADC_B;
            *p1++ = data.byte0;
            *p1++ = data.byte1;

            data.half_word = *RD_ADC_A;
            *p0++ = data.byte0;
            *p0++ = data.byte1;
        }

        int shift = 0;
        if (SET_TBASE == TBase::_100ns || SET_TBASE == TBase::_200ns)
        {
            shift = CalculateShift();
        }
        else if (necessaryShift)
        {
            shift = -1;
        }
        if (shift != 0)
        {
            if (shift < 0)
            {
                shift = -shift;
                for (int i = FPGA::MAX_POINTS - shift - 1; i >= 0; i--)
                {
                    dataRel0[i + shift] = dataRel0[i];
                    dataRel1[i + shift] = dataRel1[i];
                }
            }
            else
            {
                for (int i = shift; i < FPGA::MAX_POINTS; i++)
                {
                    dataRel0[i - shift] = dataRel0[i];
                    dataRel1[i - shift] = dataRel1[i];
                }
            }
        }
    }
}


void FPGA::InverseDataIsNecessary(Chan::E ch, uint8 *data)
{
    if (SET_INVERSE(ch))
    {
        for (int i = 0; i < FPGA::MAX_POINTS; i++)
        {
            data[i] = (uint8)((int)(2 * ValueFPGA::AVE) - Math::Limitation<uint8>(data[i], ValueFPGA::MIN, ValueFPGA::MAX));
        }
    }
}


int FPGA::CalculateShift()            // \todo �� ������ ������������ �������
{
    uint16 rand = HAL_ADC3::GetValue();

    uint16 min = 0;
    uint16 max = 0;

    if (SET_TBASE == TBase::_200ns)
    {
        return rand < 3000 ? 0 : -1;    // set.debug.altShift; \todo ������������ �� ������ ������� ��������������� ��������. �� PageDebug ����� 
                                        // ���������������, ����� ����������������� ��� �������������
    }

    if (!CalculateGate(rand, &min, &max))
    {
        return TShift::EMPTY;
    }

    if (TBase::InRandomizeMode())
    {
        float tin = (float)(rand - min) / (float)(max - min) * 10e-9f;
        int retValue = (int)(tin / 10e-9f * (float)Kr[SET_TBASE]);
        return retValue;
    }

    if (SET_TBASE == TBase::_100ns && rand < (min + max) / 2)
    {
        return 0;
    }

    return -1;  // set.debug.altShift;      \todo ������������ �� ������ ������� ��������������� ��������. �� PageDebug ����� ���������������, 
                                            //����� ����������������� ��� �������������
}


bool FPGA::CalculateGate(uint16 rand, uint16 *eMin, uint16 *eMax)
{
    if (FPGA_FIRST_AFTER_WRITE)
    {
        FPGA_FIRST_AFTER_WRITE = 0;
        return false;
    }

    if (rand < 500 || rand > 3500)
    {
        LOG_WRITE("������!!! ������� %d", rand);
        return false;
    }

    static const int numValues = 1000;

    static float minGate = 0.0f;
    static float maxGate = 0.0f;
    static int numElements = 0;
    static uint16 min = 0xffff;
    static uint16 max = 0;

    numElements++;
    if (rand < min)
    {
        min = rand;
    }
    if (rand > max)
    {
        max = rand;
    }

    if (minGate == 0)
    {
        *eMin = min;
        *eMax = max;
        if (numElements < numValues)
        {
            return true;
        }
        minGate = min;
        maxGate = max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    if (numElements == numValues)
    {
        minGate = 0.9f * minGate + 0.1f * min;
        maxGate = 0.9f * maxGate + 0.1f * max;
        numElements = 0;
        min = 0xffff;
        max = 0;
    }

    *eMin = (uint16)minGate; //-V519
    *eMax = (uint16)maxGate; //-V519

    return true;
}


bool FPGA::FindWave(Chan::E ch)
{
    Settings settings = set;    // ��������� ���������� ���������

    Stop(false);
    SET_ENABLED(ch) = true;
    TrigSource::Set((TrigSource::E)ch);
    TrigLev::Set((TrigSource::E)ch, TrigLev::ZERO);
    RShift::Set(ch, RShift::ZERO);
    ModeCouple::Set(ch, ModeCouple::AC);
    Range::E range = AccurateFindRange(ch);

    if (range != Range::Count)
    {
        SET_RANGE(ch) = range;
        TBase::E tBase = AccurateFindTBase(ch);
        if (tBase != TBase::Count)
        {
            SET_TBASE = tBase;
            TRIG_SOURCE = (TrigSource::E)ch;
            return true;
        }
    }

    set = settings;
    FPGA::LoadSettings();
    return false;
}


Range::E FPGA::AccurateFindRange(Chan::E ch)
{
    /*
    �������� ������.
    ������������� �������� 50 ��.
    ������������� �������� ����.
    �������� ������� ��������.
    1. ������������� �������� 20�/���.
    �� ��������� ��, �������� ��������� ����� � ���������� ������ � ��������� �� 50 ���� - 2 ������ � ���������. �� ��� �����
        �� ������ �������� ���� ����� ������ ������� ������� 10 ��.
    ��������� ��������� ���������� �� ����� �� ������� ������. ���� ����� - ���� ���������� �������� (��������, 2� ������ 1�) � ����������
        � � �������� ����������.
    ���� �� ����� - ���� ��������� �������� � ��������� ���� � 1.
    */

    uint8 buffer[100];  // ���� ����� ��������� �����

    TBase::Set(TBase::_50ms);
    ModeCouple::Set(ch, ModeCouple::AC);
    PeackDetMode::E peackDetMode = PEAKDET;
    PeackDetMode::Set(PeackDetMode::Enable);

    for (int range = Range::Count - 1; range >= 0; range--)
    {
        FPGA::Stop(false);
        Range::Set(ch, (Range::E)range);
        Timer::PauseOnTime(10);
        FPGA::Start();

        for (int i = 0; i < 50; i++)
        {
            while (_GET_BIT(HAL_FMC::Read(RD_FL), BIT_POINT_READY) == 0) {};

            HAL_FMC::Read(RD_ADC_A);
            HAL_FMC::Read(RD_ADC_B);
        }

        BitSet16 data;

        if (ch == Chan::A)
        {
            for (int i = 0; i < 100; i += 2)
            {
                while (_GET_BIT(HAL_FMC::Read(RD_FL), BIT_POINT_READY) == 0) {};

                HAL_FMC::Read(RD_ADC_B);

                data.half_word = *RD_ADC_A;
                buffer[i] = data.byte0;
                buffer[i + 1] = data.byte1;
            }
        }
        else
        {
            for (int i = 0; i < 100; i += 2)
            {
                while (_GET_BIT(HAL_FMC::Read(RD_FL), BIT_POINT_READY) == 0) {};

                data.half_word = *RD_ADC_B;

                buffer[i] = data.byte0;
                buffer[i + 1] = data.byte1;

                HAL_FMC::Read(RD_ADC_A);
            }
        }

        if (CalculateMinWithout0(buffer) < ValueFPGA::MIN || CalculateMaxWithout255(buffer) > ValueFPGA::MAX)
        {
            if (range < Range::_20V)
            {
                range++;
            }
            PeackDetMode::Set(peackDetMode);
            return (Range::E)range;
        }

        uint8 min = ValueFPGA::AVE - 30;
        uint8 max = ValueFPGA::AVE + 30;

        if (range == Range::_2mV && CalculateMinWithout0(buffer) > min && CalculateMaxWithout255(buffer) < max)
        {
            PeackDetMode::Set(peackDetMode);
            return Range::Count;
        }
    }

    PeackDetMode::Set(peackDetMode);
    return Range::Count;
}


TBase::E FPGA::AccurateFindTBase(Chan::E ch)
{
    for (int i = 0; i < 5; i++)
    {
        TBase::E tBase = FindTBase(ch);
        TBase::E secondTBase = FindTBase(ch); //-V656

        if (tBase == secondTBase && tBase != TBase::Count)
        {
            return tBase;
        }
    }
    return TBase::Count;
}


uint8 FPGA::CalculateMinWithout0(uint8 buffer[100])
{
    // \todo �� ����� ���������� ��� ����������� ����, ����� ��� ������� ���������� �������� 0 � 255 � ��������� ������
    // ��� ����� �������� �� ������ ����, ����� ���� ��������.
    uint8 min = 255;

    for (int i = 1; i < 100; i++)
    {
        if (buffer[i] > 0 && buffer[i] < min)
        {
            min = buffer[i];
        }
    }
    return min;
}


uint8 FPGA::CalculateMaxWithout255(uint8 buffer[100])
{
    // \todo �� ����� ���������� ��� ����������� ����, ����� ��� ������� ���������� �������� 0 � 255 � ��������� ������
    // ��� ����� �������� �� ������ ����, ����� ���� ��������.
    uint8 max = 0;
    for (int i = 1; i < 100; i++)
    {
        if (buffer[i] < 255 && buffer[i] > max)
        {
            max = buffer[i];
        }
    }
    return max;
}


TBase::E FPGA::FindTBase(Chan::E ch)
{
    TrigInput::Set(TrigInput::Full);
    Timer::PauseOnTime(10);
    FPGA::Stop(false);
    float fr = FreqMeter::CalculateFreqFromCounterFreq();

    TrigInput::Set(fr < 1e6f ? TrigInput::LPF : TrigInput::Full);

    fr = FreqMeter::CalculateFreqFromCounterFreq();

    TBase::E tBase = TBase::Count;

    if (fr >= 50.0f)
    {
        tBase = CalculateTBase(fr);
        TBase::Set(tBase);
        FPGA::Start();
        TrigInput::Set(fr < 500e3f ? TrigInput::LPF : TrigInput::HPF);
        return tBase;
    }
    else
    {
        TrigInput::Set(TrigInput::LPF);
        FreqMeter::freq = FreqMeter::CalculateFreqFromCounterPeriod();

        if (fr > 0.0f)
        {
            tBase = CalculateTBase(fr);
            TBase::Set(tBase);
            Timer::PauseOnTime(10);
            FPGA::Start();
            return tBase;
        }
    }

    return TBase::Count;
}


TBase::E FPGA::CalculateTBase(float freq_)
{
    if (freq_ >= 100e6f)      { return TBase::_2ns; }
    else if (freq_ >= 40e6f)  { return TBase::_5ns; }
    else if (freq_ >= 20e6f)  { return TBase::_10ns; }
    else if (freq_ >= 10e6f)  { return TBase::_20ns; }
    else if (freq_ >= 3e6f)   { return TBase::_50ns; }
    else if (freq_ >= 2e6f)   { return TBase::_100ns; }
    else if (freq_ >= 900e3f) { return TBase::_200ns; }
    else if (freq_ >= 400e3f) { return TBase::_500ns; }
    else if (freq_ >= 200e3f) { return TBase::_1us; }
    else if (freq_ >= 90e3f)  { return TBase::_2us; }
    else if (freq_ >= 30e3f)  { return TBase::_5us; }
    else if (freq_ >= 20e3f)  { return TBase::_10us; }
    else if (freq_ >= 10e3f)  { return TBase::_20us; }
    else if (freq_ >= 4e3f)   { return TBase::_50us; }
    else if (freq_ >= 2e3f)   { return TBase::_100us; }
    else if (freq_ >= 1e3f)   { return TBase::_200us; }
    else if (freq_ >= 350.0f) { return TBase::_500us; }
    else if (freq_ >= 200.0f) { return TBase::_1ms; }
    else if (freq_ >= 100.0f) { return TBase::_2ms; }
    else if (freq_ >= 40.0f)  { return TBase::_5ms; }
    else if (freq_ >= 20.0f)  { return TBase::_10ms; }
    else if (freq_ >= 10.0f)  { return TBase::_20ms; }
    else if (freq_ >= 4.0f)   { return TBase::_50ms; }
    else if (freq_ >= 2.0f)   { return TBase::_100ms; }
    return TBase::_200ms;
}


uint8 FPGA::ReadFlag()
{
    uint16 flag = HAL_FMC::Read(RD_FL);

    FreqMeter::Update(flag);

    return flag;
}


void FPGA::ClearData()
{
    memset(dataRel0, 0, FPGA::MAX_POINTS);
    memset(dataRel1, 0, FPGA::MAX_POINTS);
}


void FPGA::ReadPoint()
{
    if (_GET_BIT(ReadFlag(), BIT_POINT_READY))
    {
        BitSet16 dataA;
        BitSet16 dataB;

        dataB.half_word = *RD_ADC_B;
        dataA.half_word = *RD_ADC_A;

        Display::AddPoints(dataA.byte0, dataA.byte1, dataB.byte0, dataB.byte1);
    }
}
