#pragma once
#include "StructCPP.h"
#include "Oscilloscope.h"
#include "AWG.h"
#include "readwfm.h"
#include "WaveCalculator.h"
#define DllExport extern "C" __declspec(dllexport)

namespace InternalProcess
{
	DllExport BOOL APIENTRY DllInitOscillo(OSCParam *oscp, LPSTR message);
	DllExport BOOL APIENTRY DllTermOscillo(OSCParam *oscp, LPSTR waveform, LPSTR message);
	DllExport BOOL APIENTRY DllMakeBit(OSCParam *oscp, LPSTR waveform, LPSTR bit);
	DllExport BOOL APIENTRY DllAcqWaveform(OSCParam *oscp, LPSTR waveform, double wfm[]);
	DllExport BOOL APIENTRY DllGetWave(OSCParam *oscp, LPSTR waveform, LPSTR message);
	DllExport BOOL APIENTRY DllGetWaveDebug(OSCParam *oscp, LPSTR waveform, INT n);
}