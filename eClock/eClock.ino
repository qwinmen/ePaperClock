//Независимая от reset память
#include <EEPROM.h>

// e-Paper lib
// https://github.com/waveshare/e-Paper

#include <SPI.h>
#include "epd2in9_V2.h"
#include "epdpaint.h"

//Библиотека microDS18B20
//https://github.com/GyverLibs/microDS18B20/releases/tag/3.7
#include "microDS18B20.h"
MicroDS18B20<A0> ds;  //датчик DS18B20 подключен к пину (резистор на 4.7к обязателен)

//Библиотека для часов
//https://github.com/msparks/arduino-ds1302
#include <stdio.h>
#include "DS1302.h"

// Create a DS1302 object:
DS1302 rtc(/*Chip Enable (RST):*/2, /*Input/Output (DAT):*/3, /*Serial Clock (CLK):*/4);

Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);
unsigned long time_start_ms;
unsigned long time_now_s;

#define COLORED 0
#define UNCOLORED 1

void setup()
{
	Serial.begin(9600);
	if (epd.Init() != 0)
	{
		Serial.print("e-Paper init failed");
		return;
	}

	//достать флаг из памяти
	  bool dataReadFromMemory = false;
	  EEPROM.get(true, dataReadFromMemory);
	//если флаг не установлен
	if(dataReadFromMemory == false)
	{
		//инициировать часы
		rtc.writeProtect(false);
		rtc.halt(false);
		Time t(2022, 3, 13, 20, 36, 50, Time::kSunday);
		rtc.time(t);
		//запомнить флаг инициации в память
		bool dataF = true;
		EEPROM.put(true, dataF);
	}
}

void loop()
{
	static uint32_t timer = millis();
	if(millis() - timer >= 60000){	//каждую минуту
		timer = millis();	//обновляем таймер

		epd.ClearFrameMemory(0xFF); // bit set = white, bit reset = black
		epd.DisplayFrame();
		epd.ClearFrameMemory(0xFF); // bit set = white, bit reset = black
		epd.DisplayFrame();

		paint.SetRotate(ROTATE_90);
		Time t = rtc.time();

		HStart(NumericConverter(t.hr/10));
		HEnd(NumericConverter(t.hr%10));
		DotSplitter();
		MStart(NumericConverter(t.min/10));
		int minNumericEnd = t.min%10;
		MEnd(NumericConverter(minNumericEnd));

		//YYYY-month-day
		DateYear(t);

		//dayOf
		DayOf(dayAsString(t.day));

		if((minNumericEnd%2) == 0/*ds.readTemp()*/){
			Temperature(ds.getTemp());
		}
		
		ds.requestTemp();

		epd.DisplayFrame(); //экран
	}
}

///Конвертирует цифру в символ. Это костыль, из-за несоответствия в font72.cpp
const char* NumericConverter(int singleDigital){
	switch (singleDigital)
	{
	case 0: return "!";
	case 1: return "\"";
	case 2: return "#";
	case 3: return "$";
	case 4: return "%";
	case 5: return "&";
	case 6: return "'";
	case 7: return "(";
	case 8: return ")";
	case 9: return "*";
	default:
		return " ";
	}
}

///Первая цифра часы
void HStart(const char* hourStart)
{
		paint.SetWidth(Font72.Height);
		paint.SetHeight(Font72.Width);

		paint.Clear(UNCOLORED);															  //память
		paint.DrawStringAt(0, 0, hourStart, &Font72, COLORED);								  //память
		epd.SetFrameMemory(paint.GetImage(), 15, 10, paint.GetWidth(), paint.GetHeight()); //экран
}

///Вторая цифра часы
void HEnd(const char* hourEnd)
{
		paint.SetWidth(Font72.Height);
		paint.SetHeight(Font72.Width);

		paint.Clear(UNCOLORED);															  //память
		paint.DrawStringAt(0, 0, hourEnd, &Font72, COLORED);								  //память
		epd.SetFrameMemory(paint.GetImage(), 15, 70, paint.GetWidth(), paint.GetHeight()); //экран
}

///Первая цифра минуты
void MStart(const char* minStart)
{
		paint.SetWidth(Font72.Height);
		paint.SetHeight(Font72.Width);

		paint.Clear(UNCOLORED);															  //память
		paint.DrawStringAt(0, 0, minStart, &Font72, COLORED);								  //память
		epd.SetFrameMemory(paint.GetImage(), 15, 160, paint.GetWidth(), paint.GetHeight()); //экран
}

///Вторая цифра минуты
void MEnd(const char* minEnd)
{
		paint.SetWidth(Font72.Height); //Font26.Height
		paint.SetHeight(Font72.Width); //Font26.Width, 113 max

		paint.Clear(UNCOLORED);															  //память
		paint.DrawStringAt(0, 0, minEnd, &Font72, COLORED);								  //память
		epd.SetFrameMemory(paint.GetImage(), 15, 220, paint.GetWidth(), paint.GetHeight()); //экран
}

///Двоеточие, разделитель ЧАСЫ:МИНУТЫ
void DotSplitter()
{
		paint.SetWidth(Font72.Height);
		paint.SetHeight(Font72.Width);

		paint.Clear(UNCOLORED);															  //память
		paint.DrawStringAt(0, 0, "+", &Font72, COLORED);								  //память
		epd.SetFrameMemory(paint.GetImage(), 15, 120, paint.GetWidth(), paint.GetHeight()); //экран
}

///Год, месяц, число
void DateYear(Time t){
			paint.SetWidth(16);
			paint.SetHeight(80); //ширина прямоугольника
			
			const String month = monthAsString(t.mon);
			char buf[12];
			snprintf(buf, sizeof(buf), "%04d %s %02d", t.yr, month.c_str(), t.date);

			paint.Clear(UNCOLORED);
			paint.DrawStringAt(0, 0, buf, &Font12, COLORED);
			epd.SetFrameMemory(paint.GetImage(), 100, 5, paint.GetWidth(), paint.GetHeight());
		}

///День недели
void DayOf(String day){
			paint.SetWidth(16);
			paint.SetHeight(20); //ширина прямоугольника

			char buf[5];
			snprintf(buf, sizeof(buf), "%s", day.c_str());

			paint.Clear(UNCOLORED);
			paint.DrawStringAt(0, 0, buf, &Font12, COLORED);
			epd.SetFrameMemory(paint.GetImage(), 100, 100, paint.GetWidth(), paint.GetHeight());
		}

///Температура
void Temperature(int currentValue){
			paint.SetWidth(16);
			paint.SetHeight(50); //ширина прямоугольника
			char stringTemperature[20];
			sprintf(stringTemperature, "t:%d*C", currentValue);
			paint.Clear(UNCOLORED);
			paint.DrawStringAt(0, 0, stringTemperature, &Font12, COLORED);
			epd.SetFrameMemory(paint.GetImage(), 100, 230, paint.GetWidth(), paint.GetHeight());
		}

///День недели
String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "BC";
    case Time::kMonday: return "5H";//ПН
    case Time::kTuesday: return "BT";
    case Time::kWednesday: return "CP";
    case Time::kThursday: return "4T";//ЧТ
    case Time::kFriday: return "5T";//ПТ
    case Time::kSaturday: return "C6";//СБ
  }
  return "(unknown day)";
}

String monthAsString(uint8_t month) {
  switch (month) {
    case 1: return "jan";
    case 2: return "feb";
    case 3: return "mar";
    case 4: return "apr";
    case 5: return "may";
    case 6: return "jun";
    case 7: return "jul";
	case 8: return "aug";
	case 9: return "sep";
	case 10: return "oct";
	case 11: return "nov";
	case 12: return "dec";
  }
  return "(unknown month)";
}