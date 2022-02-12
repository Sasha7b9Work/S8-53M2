// 2022/2/11 20:59:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL_PINS.h"


/*
                         |      ����                  |                        �����                                    |

    ��� �������������    |                            | ADC1 ; ADC_CHANNEL_0 ; EXTI4_IRQHandler ; in - PA0 ; it - PH4   |
    ������� �������      |                            | DAC1 ; DAC1_CHANNEL_1 ; out - PA4                               |
    ����                 | DAC1 ; DMA1                | DAC2 ; DMA_CHANNEL_7 ; DAC_CHANNEL_2 ; DMA1_Stream6 ; out - PA5 |
    ������               | TIM6 ; TIM6_DAC_IRQHandler |                                                                 |
    ������               |                            |                                                                 |
    USB                  |                            |                                                                 |
    Flash                |                            |                                                                 |
    ������������         | TIM2                       |                                                                 |

    OTP                  |                            |                                                                 |
    RTC                  |                            |                                                                 |
    ROM                  |                            |                                                                 |
    LTDC                 |                            |                                                                 |
    LAN                  |                            |                                                                 |
    ����                 |                            |                                                                 |

                         |                            |                                                                 |
*/


namespace HAL
{
    void Init();
    uint CalculateCRC32();
}


namespace HAL_ADC3
{
    void Init();
    uint16 GetValue();
}


// ������������ ��� ���������� �������� �������
namespace HAL_DAC1
{
    void Init();

    // �������� value �� 0 �� 100
    void SetBrightness(int value);
};


// ������������ ��� ����� � FPGA � RAM
namespace HAL_FMC
{
    // ��� ������ �������
    uint8 Read(uint8* address);

    void Init();

    inline static uint16 Read(volatile const uint16 *const address) { return *address; };

    void Write(uint16 *const address, uint16 value);

    extern uint16 *const _ADDR_FPGA;                // ����� ������ � �������

    extern uint16 *const _ADDR_RAM_DISPLAY_FRONT;   // ������� ����������� �������. ��������� � RAM � �� ����� ������-
                                                    // �������� ��� ������ �����
    extern uint8 *const _ADDR_RAM_BEGIN;            // ������ ��������� ����� � RAM.
    extern uint8 *const _ADDR_RAM_END;              // ��������� �� ������ ������������ ����� RAM. ������������ �����
                                                    // ������ [ADDR_RAM_BEGIN...(ADDR_RAM_END - 1)]
};


namespace HAL_LTDC
{
    void Init(uint8 *frontBuffer, uint8 *backBuffer);

    void LoadPalette();

    void ToggleBuffers();
};


// ������ ��� �����
namespace HAL_TIM2
{
    void Init();
}


// ������ �����������
namespace HAL_TIM6
{
    void Init();
}
