// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#include "Generator.h"
#include "Math.h"
#include "Settings/Settings.h"


#include <stdlib.h>
#include <math.h>


static void SetParametersWave(Chan::E ch, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise);
static void StartNewWave(Chan::E ch);
static uint8 GetSampleWave(Chan::E ch);


const SGenerator Generator = 
{
    SetParametersWave,
    StartNewWave,
    GetSampleWave,
};


static float NewNoiseValue(Chan::E ch);
static uint8 GetSampleSinusWave(Chan::E ch, int numSample);
static uint8 GetSampleMeanderWave(Chan::E ch, int numSample);


static TypeWave type[2] = {Wave_Sinus, Wave_Meander};
static float freq[2] = {1000.0f, 500.0f};
static float angle[2] = {0.05f, 0.1f};
static float ampl[2] = {1.0f, 0.5f};
static float amplNoise[2] = {0.1f, 0.1f};
static int numSample[2] = {0, 0};

void SetParametersWave(Chan::E ch, TypeWave typeWave, float frequency, float startAngle, float amplWave, float amplNoise_)
{
    type[channel] = typeWave;
    freq[channel] = frequency;
    angle[channel] = startAngle;
    ampl[channel] = amplWave;
    amplNoise[channel] = amplNoise_;
}

void StartNewWave(Chan::E ch)
{
    numSample[0] = numSample[1] = 0;
}

uint8 GetSampleWave(Chan::E ch)
{
    return (type[channel] == Wave_Sinus) ? GetSampleSinusWave(channel, (numSample[channel])++) : GetSampleMeanderWave(channel, (numSample[channel])++);
}

uint8 GetSampleSinusWave(Chan::E ch, int numSample_)
{
    float dT = numSample_ * TSHIFT_2_ABS(1, SET_TBASE);
    float voltage = ampl[channel] * sin(2 * M_PI * freq[channel] * dT + angle[channel]) + NewNoiseValue(channel);
    return Math_VoltageToPoint(voltage, SET_RANGE(channel), SET_RSHIFT(channel));
}

uint8 GetSampleMeanderWave(Chan::E ch, int numSample_)
{
    return 0;
}

float NewNoiseValue(Chan::E ch)
{
    static float prevNoise[2] = {0.0f, 0.0f};            // ����� �������� �������� ���� �� ���������� �����, ����������� ��� ������� ���� � ������� �����.

    float noise = prevNoise[channel];

    float halfAmplNoiseAbs = ampl[channel] * amplNoise[channel] / 2.0f;

    float deltaRand = halfAmplNoiseAbs;

    noise += Math_RandFloat(-deltaRand, deltaRand);

    while (noise < -halfAmplNoiseAbs)
    {
        noise += Math_RandFloat(0, deltaRand * 2);
    }

    while (noise > halfAmplNoiseAbs)
    {
        noise -= Math_RandFloat(0, deltaRand * 2);
    }

    prevNoise[channel] = noise;

    return noise;
}
