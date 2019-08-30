#pragma once
#include <Windows.h>
#include <cmath>
#include <fstream>
#include <string>
#include "visa.h"

namespace InternalProcess
{
	enum CH {RES1, RES2, RES3, RES4};

	typedef struct
	{
		const char* Filename;
		
		int DebugFlag;
		int Timeout;
		int RecordDataLength;
		int RecordHeaderLength;
		int RecordBufferLength;
		int SetNum;

		double YZero[4], YMult[4], YOff[4];
		double SamplingInterval;
		double HorizontalScale;
		
		ViSession rm;
		ViSession vi;
	} OSCParam;

	typedef struct
	{
		int Timeout;

		double amp[2];

		ViSession rm;
		ViSession vi;
	} AWGParam;
}