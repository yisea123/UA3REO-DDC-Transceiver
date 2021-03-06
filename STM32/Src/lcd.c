#include "main.h"
#include "lcd.h"
#include "functions.h"
#include "arm_math.h"
#include "agc.h"
#include "settings.h"
#include "system_menu.h"
#include "wm8731.h"
#include "audio_filters.h"
#include "fonts.h"
#include "wm8731.h"
#include "usbd_ua3reo.h"
#include "noise_reduction.h"
#include "cw_decoder.h"
#include "peripheral.h"

volatile bool LCD_busy = false;
volatile DEF_LCD_UpdateQuery LCD_UpdateQuery = { false };
volatile uint8_t TimeMenuSelection = 0;
volatile bool LCD_timeMenuOpened = false;
volatile bool LCD_systemMenuOpened = false;

static char LCD_freq_string_hz[6];
static char LCD_freq_string_khz[6];
static char LCD_freq_string_mhz[6];
static uint32_t LCD_last_showed_freq = 0;
static uint16_t LCD_last_showed_freq_mhz = 9999;
static uint16_t LCD_last_showed_freq_khz = 9999;
static uint16_t LCD_last_showed_freq_hz = 9999;
static int LCD_last_s_meter = 1;
static bool LCD_pressed = false;
static uint32_t Time;
static uint8_t Hours;
static uint8_t Last_showed_Hours = 255;
static uint8_t Minutes;
static uint8_t Last_showed_Minutes = 255;
static uint8_t Seconds;
static uint8_t Last_showed_Seconds = 255;

static void printInfo(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char* text, uint16_t back_color, uint16_t text_color, uint16_t in_active_color, bool active);
static void LCD_displayFreqInfo(void);
static void LCD_displayTopButtons(bool redraw);
static void LCD_displayStatusInfoBar(void);
static void LCD_displayStatusInfoGUI(void);
static void LCD_displayTextBar(void);

void LCD_Init(void)
{
	LCDDriver_setBrightness(TRX.LCD_Brightness);
	LCDDriver_Init();
#if SCREEN_ROTATE
	LCDDriver_setRotation(2);
#else
	LCDDriver_setRotation(4);
#endif
	LCDDriver_Fill(COLOR_WHITE);
}

static void LCD_displayTopButtons(bool redraw) { //вывод верхних кнопок
	if (LCD_systemMenuOpened) return;
	if (LCD_busy)
	{
		LCD_UpdateQuery.TopButtons = true;
		return;
	}
	LCD_busy = true;
	if (redraw) LCDDriver_Fill_RectWH(0, 0, 319, 65, COLOR_BLACK);

	//вывод инфо о работе трансивера
	printInfo(0, 0, 50, 22, (!TRX.current_vfo) ? "VFOA" : "VFOB", COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, true);
	printInfo(50, 0, 60, 22, (char *)MODE_DESCR[TRX_getMode(CurrentVFO())], COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, true);
	printInfo(110, 0, 50, 22, "PRE", COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, TRX.Preamp);
	printInfo(160, 0, 50, 22, "ATT", COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, TRX.ATT);
	printInfo(210, 0, 50, 22, "CLAR", COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, TRX.CLAR);
	
	printInfo(0, 22, 50, 22, "FAST", COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, (TRX.Fast == true));
	printInfo(50, 22, 50, 22, "AGC", COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, TRX.AGC);
	printInfo(100, 22, 50, 22, "DNR", COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, TRX.DNR);
	printInfo(160, 22, 50, 22, "LOCK", COLOR_BLACK, COLOR_BUTTON_TEXT, COLOR_BUTTON_INACTIVE_TEXT, TRX.Locked);

	LCD_busy = false;
	LCD_UpdateQuery.TopButtons = false;
}

static void LCD_displayFreqInfo() { //вывод частоты на экран
	if (LCD_systemMenuOpened) return;
	if (LCD_last_showed_freq == TRX_getFrequency(CurrentVFO())) return;
	if (LCD_busy)
	{
		LCD_UpdateQuery.FreqInfo = true;
		return;
	}
	LCD_busy = true;
	uint16_t mhz_x_offset = 0;
	LCD_last_showed_freq = TRX_getFrequency(CurrentVFO());
	if (TRX_getFrequency(CurrentVFO()) >= 100000000)
		mhz_x_offset = 0;
	else if (TRX_getFrequency(CurrentVFO()) >= 10000000)
		mhz_x_offset = 26;
	else
		mhz_x_offset = 52;
	LCDDriver_Fill_RectWH(0, 57, mhz_x_offset, 35, COLOR_BLACK);

	//добавляем пробелов для вывода частоты
	uint16_t hz = ((uint32_t)TRX_getFrequency(CurrentVFO()) % 1000);
	uint16_t khz = ((uint32_t)(TRX_getFrequency(CurrentVFO()) / 1000) % 1000);
	uint16_t mhz = ((uint32_t)(TRX_getFrequency(CurrentVFO()) / 1000000) % 1000000);
	sprintf(LCD_freq_string_hz, "%d", hz);
	sprintf(LCD_freq_string_khz, "%d", khz);
	sprintf(LCD_freq_string_mhz, "%d", mhz);

	if (LCD_last_showed_freq_mhz != mhz)
	{
		LCDDriver_printTextFont(LCD_freq_string_mhz, mhz_x_offset, 90, COLOR_WHITE, COLOR_BLACK, FreeSans24pt7b);
		LCD_last_showed_freq_mhz = mhz;
	}

	char buff[50] = "";
	if (LCD_last_showed_freq_khz != khz)
	{
		addSymbols(buff, LCD_freq_string_khz, 3, "0", false);
		LCDDriver_printTextFont(buff, 91, 90, COLOR_WHITE, COLOR_BLACK, FreeSans24pt7b);
		LCD_last_showed_freq_khz = khz;
	}
	if (LCD_last_showed_freq_hz != hz)
	{
		addSymbols(buff, LCD_freq_string_hz, 3, "0", false);
		LCDDriver_printTextFont(buff, 182, 90, COLOR_WHITE, COLOR_BLACK, FreeSans24pt7b);
		LCD_last_showed_freq_hz = hz;
	}
	NeedSaveSettings = true;
	LCD_UpdateQuery.FreqInfo = false;
	LCD_busy = false;
}

static void LCD_displayStatusInfoGUI(void) { //вывод RX/TX и с-метра
	if (LCD_systemMenuOpened) return;
	if (LCD_busy)
	{
		LCD_UpdateQuery.StatusInfoGUI = true;
		return;
	}
	LCD_busy = true;
	
	LCDDriver_Fill_RectWH(0, 100, LCD_WIDTH, 30, COLOR_BLACK);
	
	if (TRX_on_TX())
	{
		if(TRX_Tune)
			LCDDriver_printTextFont("TU", 10, 114, COLOR_RED, COLOR_BLACK, FreeSans9pt7b);
		else
			LCDDriver_printTextFont("TX", 10, 114, COLOR_RED, COLOR_BLACK, FreeSans9pt7b);
		
		LCDDriver_drawRectXY(40, 100, 40 + PMETER_WIDTH, 115, COLOR_RED); //рамка SWR-метра
		LCDDriver_printText("SWR:", 45, 120, COLOR_GREEN, COLOR_BLACK, 1);
		LCDDriver_printText("FWD:", 45+55, 120, COLOR_GREEN, COLOR_BLACK, 1);
		LCDDriver_printText("REF:", 45+55*2, 120, COLOR_GREEN, COLOR_BLACK, 1);
		
		LCDDriver_drawRectXY(40 + PMETER_WIDTH + 10, 100, 40 + PMETER_WIDTH + 10 + AMETER_WIDTH, 115, COLOR_RED); //рамка ALC-метра
		LCDDriver_printText("ALC:", 40 + PMETER_WIDTH + 15, 120, COLOR_GREEN, COLOR_BLACK, 1);
	}
	else
	{
		LCDDriver_printTextFont("RX", 10, 114, COLOR_GREEN, COLOR_BLACK, FreeSans9pt7b);
		
		LCDDriver_drawRectXY(40, 100, 40 + SMETER_WIDTH, 115, COLOR_RED); //рамка S-метра

		LCDDriver_printTextFont(".", 78, 90, COLOR_WHITE, COLOR_BLACK, FreeSans24pt7b); //разделители частоты
		LCDDriver_printTextFont(".", 169, 90, COLOR_WHITE, COLOR_BLACK, FreeSans24pt7b);

		LCDDriver_printText("dBm", SMETER_WIDTH + 69, 105, COLOR_GREEN, COLOR_BLACK, 1);

		uint16_t step = SMETER_WIDTH / 8;
		LCDDriver_printText("1", 50 + step * 0, 120, COLOR_RED, COLOR_BLACK, 1);
		LCDDriver_printText("3", 50 + step * 1, 120, COLOR_RED, COLOR_BLACK, 1);
		LCDDriver_printText("5", 50 + step * 2, 120, COLOR_RED, COLOR_BLACK, 1);
		LCDDriver_printText("7", 50 + step * 3, 120, COLOR_RED, COLOR_BLACK, 1);
		LCDDriver_printText("9", 50 + step * 4, 120, COLOR_RED, COLOR_BLACK, 1);
		LCDDriver_printText("+20", 50 + step * 5, 120, COLOR_RED, COLOR_BLACK, 1);
		LCDDriver_printText("+40", 50 + step * 6, 120, COLOR_RED, COLOR_BLACK, 1);
		LCDDriver_printText("+60", 50 + step * 7, 120, COLOR_RED, COLOR_BLACK, 1);

		if (TRX.NotchFilter)
		{
			char buff[10] = "";
			sprintf(buff, "%dhz", TRX.NotchFC);
			addSymbols(buff, buff, 7, " ", false);
			LCDDriver_printText(buff, SMETER_WIDTH + 46, 120, COLOR_BLUE, COLOR_BLACK, 1);
		}
		else
		{
			LCDDriver_Fill_RectWH(SMETER_WIDTH + 46, 120, 44, 8, COLOR_BLACK);
			if (TRX.FFT_Zoom == 1) LCDDriver_printText("48kHz", SMETER_WIDTH + 46, 120, COLOR_WHITE, COLOR_BLACK, 1);
			if (TRX.FFT_Zoom == 2) LCDDriver_printText("24kHz", SMETER_WIDTH + 46, 120, COLOR_WHITE, COLOR_BLACK, 1);
			if (TRX.FFT_Zoom == 4) LCDDriver_printText("12kHz", SMETER_WIDTH + 46, 120, COLOR_WHITE, COLOR_BLACK, 1);
			if (TRX.FFT_Zoom == 8) LCDDriver_printText(" 6kHz", SMETER_WIDTH + 46, 120, COLOR_WHITE, COLOR_BLACK, 1);
			if (TRX.FFT_Zoom == 16) LCDDriver_printText(" 3kHz", SMETER_WIDTH + 46, 120, COLOR_WHITE, COLOR_BLACK, 1);
		}
	}
	
	//Redraw CW decoder
	if (TRX.CWDecoder && (TRX_getMode(CurrentVFO()) == TRX_MODE_CW_L || TRX_getMode(CurrentVFO()) == TRX_MODE_CW_U))
	{
		LCDDriver_Fill_RectWH(0, LCD_HEIGHT - FFT_CWDECODER_OFFSET, FFT_PRINT_SIZE, FFT_CWDECODER_OFFSET, COLOR_BLACK);
		LCD_UpdateQuery.TextBar = true;
	}

	LCD_UpdateQuery.StatusInfoGUI = false;
	LCD_busy = false;
}

static void LCD_displayStatusInfoBar(void) { //S-метра и прочей информации
	if (LCD_systemMenuOpened) return;
	if (LCD_busy)
	{
		LCD_UpdateQuery.StatusInfoBar = true;
		return;
	}
	LCD_busy = true;
	char ctmp[50];
	const int width = SMETER_WIDTH - 2;

	if (!TRX_on_TX())
	{
		float32_t TRX_s_meter = (127.0f + TRX_RX_dBm) / 6; //127dbm - S0, 6dBm - 1S div
		if (TRX_getFrequency(CurrentVFO()) >= 144000000)
			TRX_s_meter = (127.0f + TRX_RX_dBm) / 6; //147dbm - S0 для частот выше 144мгц
		if (TRX_s_meter <= 9.0f)
			TRX_s_meter = TRX_s_meter * ((width / 9.0f)*5.0f / 9.0f); //первые 9 баллов по 6 дб, первые 5 из 8 рисок (9 участков)
		else
			TRX_s_meter = ((width / 9.0f)*5.0f) + (TRX_s_meter - 9.0f)*((width / 9.0f)*3.0f / 10.0f); //остальные 3 балла по 10 дб
		if (TRX_s_meter > width) TRX_s_meter = width;
		if (TRX_s_meter < 0.0f) TRX_s_meter = 0.0f;

		int s_width = TRX_s_meter;
		if (LCD_last_s_meter > s_width) s_width = LCD_last_s_meter - ((LCD_last_s_meter - s_width) / 4); //сглаживаем движение с-метра
		else if (LCD_last_s_meter < s_width) s_width = s_width - ((s_width - LCD_last_s_meter) / 2);
		if (LCD_last_s_meter != s_width)
		{
			if(!TRX.S_METER_Style) //полоса
			{
				LCDDriver_Fill_RectWH(41 + s_width, 101, LCD_last_s_meter - s_width, 13, COLOR_BLACK);
				LCDDriver_Fill_RectWH(41, 101, s_width, 13, COLOR_WHITE);
			}
			else //линия
			{
				LCDDriver_Fill_RectWH(41, 101, width, 13, COLOR_BLACK);
				LCDDriver_Fill_RectWH(41 + s_width, 101, 1, 13, COLOR_WHITE);
			}
			
			LCD_last_s_meter = s_width;
		}

		sprintf(ctmp, "%d", TRX_RX_dBm);
		LCDDriver_Fill_RectWH(41 + width + 5, 105, 23, 8, COLOR_BLACK);
		LCDDriver_printText(ctmp, 41 + width + 5, 105, COLOR_GREEN, COLOR_BLACK, 1);
	}
	else
	{
		//SWR
		LCDDriver_Fill_RectWH(45+23, 120, 25, 8, COLOR_BLACK);
		sprintf(ctmp, "%.1f", TRX_SWR);
		LCDDriver_printText(ctmp, 45+23, 120, COLOR_RED, COLOR_BLACK, 1);
		
		//FWD
		LCDDriver_Fill_RectWH(45+55+23, 120, 25, 8, COLOR_BLACK);
		float32_t fwd_power = (TRX_SWR_forward * TRX_SWR_forward) / 50.0f;
		if(fwd_power<0.0f) fwd_power=0.0f;
		sprintf(ctmp, "%.1fW", fwd_power);
		LCDDriver_printText(ctmp, 45+55+23, 120, COLOR_RED, COLOR_BLACK, 1);
		
		//REF
		LCDDriver_Fill_RectWH(45+55*2+23, 120, 25, 8, COLOR_BLACK);
		float32_t ref_power = (TRX_SWR_backward * TRX_SWR_backward) / 50.0f;
		if(ref_power<0.0f) ref_power=0.0f;
		sprintf(ctmp, "%.1fW", ref_power);
		LCDDriver_printText(ctmp, 45+55*2+23, 120, COLOR_RED, COLOR_BLACK, 1);
		
		//SWR Meter
		if(fwd_power>MAX_RF_POWER) fwd_power=MAX_RF_POWER;
		uint16_t ref_width=ref_power * (PMETER_WIDTH - 2) / MAX_RF_POWER;
		uint16_t fwd_width=fwd_power * (PMETER_WIDTH - 2) / MAX_RF_POWER;
		uint16_t est_width=(MAX_RF_POWER - fwd_power) * (PMETER_WIDTH - 2) / MAX_RF_POWER;
		if(ref_width>fwd_width)
			ref_width=fwd_width;
		fwd_width -= ref_width;
		LCDDriver_Fill_RectWH(40 + 1, 101, fwd_width, 13, COLOR_GREEN);
		LCDDriver_Fill_RectWH(40 + 1 + fwd_width, 101, ref_width, 13, COLOR_RED);
		LCDDriver_Fill_RectWH(40 + 1 + fwd_width + ref_width, 101, est_width, 13, COLOR_BLACK);
		
		//ALC
		LCDDriver_Fill_RectWH(40 + PMETER_WIDTH + 40, 120, 25, 8, COLOR_BLACK);
		uint8_t alc_level = (uint8_t)(TRX_GetALC()*100);
		sprintf(ctmp, "%d%%", alc_level);
		LCDDriver_printText(ctmp, 40 + PMETER_WIDTH + 40, 120, COLOR_RED, COLOR_BLACK, 1);
		uint8_t alc_level_width = (AMETER_WIDTH - 2) * alc_level / 100;
		if(alc_level_width > (AMETER_WIDTH - 2))
			alc_level_width = AMETER_WIDTH - 2;
		LCDDriver_Fill_RectWH(40 + PMETER_WIDTH + 11, 101, alc_level_width, 13, COLOR_GREEN);
		if(alc_level<100)
			LCDDriver_Fill_RectWH(40 + PMETER_WIDTH + 11 + alc_level_width, 101, AMETER_WIDTH - 2 - alc_level_width, 13, COLOR_BLUE);
	}
	
	LCDDriver_Fill_RectWH(270, 20, 50, 8, COLOR_BLACK);
	if (TRX_ADC_OTR || TRX_DAC_OTR || TRX_ADC_MAXAMPLITUDE > (powf(2,ADC_BITS)*0.49) || TRX_ADC_MINAMPLITUDE < -(powf(2,ADC_BITS)*0.49))
		LCDDriver_printText("OVR", 270, 20, COLOR_RED, COLOR_BLACK, 1);
	if (WM8731_Buffer_underrun && !TRX_on_TX())
		LCDDriver_printText("WBF", 297, 20, COLOR_RED, COLOR_BLACK, 1);
	if (FPGA_Buffer_underrun && TRX_on_TX())
		LCDDriver_printText("FBF", 297, 20, COLOR_RED, COLOR_BLACK, 1);
	if (false & RX_USB_AUDIO_underrun)
		LCDDriver_printText("UBF", 297, 20, COLOR_RED, COLOR_BLACK, 1);

	Time = RTC->TR;
	Hours = ((Time >> 20) & 0x03) * 10 + ((Time >> 16) & 0x0f);
	Minutes = ((Time >> 12) & 0x07) * 10 + ((Time >> 8) & 0x0f);
	Seconds = ((Time >> 4) & 0x07) * 10 + ((Time >> 0) & 0x0f);

	const uint8_t time_y = 5;
	const uint16_t time_x = 268;
	if (Hours != Last_showed_Hours)
	{
		sprintf(ctmp, "%d", Hours);
		addSymbols(ctmp, ctmp, 2, "0", false);
		LCDDriver_printText(ctmp, time_x, time_y, COLOR_WHITE, COLOR_BLACK, 1);
		LCDDriver_printText(":", time_x + 12, time_y, COLOR_WHITE, COLOR_BLACK, 1);
		Last_showed_Hours = Hours;
	}
	if (Minutes != Last_showed_Minutes)
	{
		sprintf(ctmp, "%d", Minutes);
		addSymbols(ctmp, ctmp, 2, "0", false);
		LCDDriver_printText(ctmp, time_x + 18, time_y, COLOR_WHITE, COLOR_BLACK, 1);
		LCDDriver_printText(":", time_x + 30, time_y, COLOR_WHITE, COLOR_BLACK, 1);
		Last_showed_Minutes = Minutes;
	}
	if (Seconds != Last_showed_Seconds)
	{
		sprintf(ctmp, "%d", Seconds);
		addSymbols(ctmp, ctmp, 2, "0", false);
		LCDDriver_printText(ctmp, time_x + 36, time_y, COLOR_WHITE, COLOR_BLACK, 1);
		Last_showed_Seconds = Seconds;
	}
	LCD_UpdateQuery.StatusInfoBar = false;
	LCD_busy = false;
}

static void LCD_displayTextBar(void) { //вывод текста под водопадом
	if (LCD_systemMenuOpened) return;
	if (LCD_busy)
	{
		LCD_UpdateQuery.TextBar = true;
		return;
	}
	LCD_busy = true;

	if (TRX.CWDecoder && (TRX_getMode(CurrentVFO()) == TRX_MODE_CW_L || TRX_getMode(CurrentVFO()) == TRX_MODE_CW_U))
	{
		char ctmp[50];
		sprintf(ctmp, "WPM:%d", CW_Decoder_WPM);
		LCDDriver_printText(ctmp, 0, LCD_HEIGHT - FFT_CWDECODER_OFFSET + 1, COLOR_WHITE, COLOR_BLACK, 2);
		LCDDriver_printText((char *)&CW_Decoder_Text, 75, LCD_HEIGHT - FFT_CWDECODER_OFFSET + 1, COLOR_WHITE, COLOR_BLACK, 2);
	}

	LCD_UpdateQuery.TextBar = false;
	LCD_busy = false;
}

void LCD_redraw(void) {
	LCD_UpdateQuery.Background = true;
	LCD_UpdateQuery.FreqInfo = true;
	LCD_UpdateQuery.StatusInfoBar = true;
	LCD_UpdateQuery.StatusInfoGUI = true;
	LCD_UpdateQuery.TopButtons = true;
	LCD_UpdateQuery.SystemMenu = true;
	LCD_UpdateQuery.TextBar = true;
	LCD_last_s_meter = 0;
	LCD_last_showed_freq = 0;
	Last_showed_Hours = 255;
	Last_showed_Minutes = 255;
	Last_showed_Seconds = 255;
	LCD_last_showed_freq_mhz = 9999;
	LCD_last_showed_freq_khz = 9999;
	LCD_last_showed_freq_hz = 9999;
	LCD_doEvents();
}

void LCD_doEvents(void)
{
	if (LCD_busy) return;
	if (TRX_Time_InActive > TRX.Standby_Time && TRX.Standby_Time > 0)
		LCDDriver_setBrightness(5);
	else
		LCDDriver_setBrightness(TRX.LCD_Brightness);

	if (LCD_UpdateQuery.Background)
	{
		LCD_busy = true;
		LCDDriver_Fill(COLOR_BLACK);
		LCD_UpdateQuery.Background = false;
		LCD_busy = false;
	}
	if (LCD_UpdateQuery.TopButtons) LCD_displayTopButtons(false);
	if (LCD_UpdateQuery.FreqInfo) LCD_displayFreqInfo();
	if (LCD_UpdateQuery.StatusInfoGUI) LCD_displayStatusInfoGUI();
	LCD_displayStatusInfoBar();
	if (LCD_UpdateQuery.SystemMenu) drawSystemMenu(false);
	if (LCD_UpdateQuery.TextBar) LCD_displayTextBar();
}

static void printInfo(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char* text, uint16_t back_color, uint16_t text_color, uint16_t inactive_color, bool active)
{
	LCDDriver_Fill_RectWH(x, y, width, height, back_color);
	uint16_t x1, y1, w, h;
	LCDDriver_getTextBounds(text, x, y, &x1, &y1, &w, &h, FreeSans9pt7b);
	LCDDriver_printTextFont(text, x + (width - w) / 2, y + (height / 2) + h / 2, active ? text_color : inactive_color, back_color, FreeSans9pt7b);
}

void LCD_showError(char text[], bool redraw)
{
	LCD_busy = true;
	LCDDriver_Fill(COLOR_RED);
	LCDDriver_printTextFont(text, 5, 110, COLOR_WHITE, COLOR_RED, FreeSans12pt7b);
	HAL_IWDG_Refresh(&hiwdg);
	if (redraw)
		HAL_Delay(3000);
	HAL_IWDG_Refresh(&hiwdg);
	LCD_busy = false;
	if (redraw)
		LCD_redraw();
}
