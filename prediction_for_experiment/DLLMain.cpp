#include "stdafx.h"
#include "DLLMain.h"

namespace InternalProcess
{
#ifdef _MANAGED
#pragma managed(push, off)
#endif
	BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
	{
		return TRUE;
	}

	DllExport BOOL APIENTRY DllInitOscillo(OSCParam *oscp, LPSTR message)
	{
		Oscilloscope os(oscp, message, NULL);
		return os.InitOscillo();
	}

	DllExport BOOL APIENTRY DllTermOscillo(OSCParam *oscp, LPSTR waveform, LPSTR message)
	{
		Oscilloscope os(oscp, message, waveform);
		return os.TermOscillo();
	}

	DllExport BOOL APIENTRY DllMakeBit(OSCParam *oscp, LPSTR waveform, LPSTR bit)
	{
		WaveCalculator wc(oscp, waveform);
		return wc.MakeBitfromWFM(bit, oscp->RecordDataLength, oscp->SetNum);
	}

	DllExport BOOL APIENTRY DllAcqWaveform(OSCParam *oscp, LPSTR waveform, double wfm[])
	{
		WaveCalculator wc(oscp, waveform);
		return wc.AcqWaveform(wfm);
	}

	DllExport BOOL APIENTRY DllGetWave(OSCParam *oscp, LPSTR waveform, LPSTR message)
	{
		Oscilloscope os(oscp, message, waveform);
		return os.GetWave();
	}

	DllExport BOOL APIENTRY DllGetWaveDebug(OSCParam *oscp, LPSTR waveform, INT n)
	{
		Oscilloscope os(oscp, NULL, waveform);
		return os.GetWaveDebug(n);
	}
#ifdef _MANAGED
#pragma managed(pop)
#endif
}