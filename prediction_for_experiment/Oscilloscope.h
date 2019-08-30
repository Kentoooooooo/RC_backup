#pragma once
#include "StructCPP.h"

namespace InternalProcess
{
	class Oscilloscope
	{
	public:
		Oscilloscope(OSCParam *c, char m[], char w[])
			:oscp(c), message(m), waveform(w){}
		BOOL InitOscillo();
		BOOL GetWave();
		BOOL Acq_Mode_Chg(int out_mode_flag, std::string runmode);
		BOOL SetSaveFileFormat(std::string command);
		BOOL SaveWfm(int ch, char *dest_name);
		BOOL TermOscillo();
		BOOL GetWaveDebug(int n);

	private:
		OSCParam *oscp;
		char *message;
		char *waveform;

		BOOL MyViWrite(std::string command);
		BOOL ErrorProcess(ViStatus status);
		BOOL OPC();
	};
}