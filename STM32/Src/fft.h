#ifndef FFT_h
#define FFT_h

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <math.h> 
#include "arm_math.h"
#include "wm8731.h"

#define FFT_SIZE 512 //указываем размер расчитываемого FFT
#define FFT_DOUBLE_SIZE_BUFFER (FFT_SIZE*2) //Размер буффера для расчёта FFT
#define FFT_PRINT_SIZE 320 //указываем размер выводимого FFT
#define FFT_MAX_HEIGHT 40 //указываем максимальную высоту FFT
#define FFT_WTF_HEIGHT 70 //указываем максимальную высоту водопада
#define FFT_BOTTOM_OFFSET 170 //начало FFT и водопада
#define FFT_MIN 20.0f //MIN порог сигнала FFT
#define FFT_STEP_COEFF 10.0f //коэффициент шага автокалибровки сигнала FFT (больше - медленней)
#define FFT_STEP_FIX 10.0f //шаг снижения коэффициента FFT
#define FFT_STEP_PRECISION 1.0f //шаг снижения коэффициента FFT (для слабых сигналов)
#define FFT_MAX_IN_RED_ZONE 15 //максимум красных пиков на водопаде (для автоподстройки)
#define FFT_MIN_IN_RED_ZONE 1 //минимум красных пиков на водопаде (для автоподстройки)
#define FFT_HZ_IN_PIXEL (TRX_SAMPLERATE / FFT_PRINT_SIZE) // герц в одном пикселе
#define FFT_CWDECODER_OFFSET 17 //место под текст CW декодера

extern void FFT_doFFT(void);
extern void FFT_printFFT(void);
extern void FFT_moveWaterfall(int32_t _freq_diff);
extern void FFT_printWaterfallDMA(void);

volatile extern uint32_t FFT_buff_index;
extern bool NeedFFTInputBuffer;
extern bool FFT_need_fft;
extern bool FFT_buffer_ready;
extern float32_t FFTInput_I[FFT_SIZE];
extern float32_t FFTInput_Q[FFT_SIZE];
extern void FFT_Init(void);
extern void FFT_Reset(void);

extern DMA_HandleTypeDef hdma_memtomem_dma2_stream6;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream7;

#endif
