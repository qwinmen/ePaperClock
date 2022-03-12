// lib
// https://github.com/waveshare/e-Paper

#include <SPI.h>
#include "epd2in9_V2.h"
//#include "imagedata.h"
#include "epdpaint.h"
//#include <stdio.h>
Epd epd;
unsigned char image[1024];
Paint paint(image, 0, 0);
unsigned long time_start_ms;
unsigned long time_now_s;
#define COLORED 0
#define UNCOLORED 1

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(9600);
	if (epd.Init() != 0)
	{
		Serial.print("e-Paper init failed");
		return;
	}

	Serial.println(epd.Init());

	// epd.ClearFrameMemory(0xFF); // bit set = white, bit reset = black
	// epd.DisplayFrame();
	// epd.ClearFrameMemory(0xFF); // bit set = white, bit reset = black
	// epd.DisplayFrame();

	// paint.SetRotate(ROTATE_0);
	// paint.SetWidth(200);
	// paint.SetHeight(24);

	// paint.Clear(UNCOLORED);
	// paint.DrawStringAt(10, 4, "EEEEEEEEEE", &Font24, COLORED);
	// epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());

	// epd.DisplayFrame();
	// delay(3000);
	//  epd.Sleep();
}

bool firstLoop = true;
void loop()
{
	if (firstLoop)
	{
		epd.ClearFrameMemory(0xFF); // bit set = white, bit reset = black
		epd.DisplayFrame();
		epd.ClearFrameMemory(0xFF); // bit set = white, bit reset = black
		epd.DisplayFrame();

		paint.SetRotate(ROTATE_90);
		
		HStart(NumericConverter(0));
		HEnd(NumericConverter(9));
		DotSplitter();
		MStart(NumericConverter(5));
		MEnd(NumericConverter(8));

		//YYYY-month-day
		DateYear();
		//dayOf
		DayOf();
		//t
		Temperature();

		epd.DisplayFrame(); //экран

		firstLoop = false;
	}

	delay(3000);
	Serial.println("e-Paper loop");
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
void DateYear(){
			paint.SetWidth(16);
			paint.SetHeight(80); //ширина прямоугольника

			paint.Clear(UNCOLORED);
			paint.DrawStringAt(0, 0, "2022 jan 27", &Font12, COLORED);
			epd.SetFrameMemory(paint.GetImage(), 100, 5, paint.GetWidth(), paint.GetHeight());
		}

///День недели
void DayOf(){
			paint.SetWidth(16);
			paint.SetHeight(20); //ширина прямоугольника

			paint.Clear(UNCOLORED);
			paint.DrawStringAt(0, 0, "BT", &Font12, COLORED);
			epd.SetFrameMemory(paint.GetImage(), 100, 100, paint.GetWidth(), paint.GetHeight());
		}

///Температура
void Temperature(){
			paint.SetWidth(16);
			paint.SetHeight(50); //ширина прямоугольника

			paint.Clear(UNCOLORED);
			paint.DrawStringAt(0, 0, "t:-10*C", &Font12, COLORED);
			epd.SetFrameMemory(paint.GetImage(), 100, 230, paint.GetWidth(), paint.GetHeight());
		}