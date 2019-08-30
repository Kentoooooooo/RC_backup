#pragma once
#include "StructCPP.h"

namespace InternalProcess
{
	class AWG
	{
	public:
		AWG(AWGParam *p, char m[])
			:awgp(p), message(m) {}
		BOOL InitAWG();
		BOOL TermAWG();
		BOOL SetParam();
		BOOL ChannelOutSwitch(int ch);
		BOOL GenTRG();
		BOOL ClearMemory(int ch);
		BOOL ChgFunction(int ch, std::string function);
		BOOL TransportWFMdata(int ch, float *wfm, int num);
		BOOL Burst(int ch, std::string command);
		BOOL SYNC(int ch);

	private:
		AWGParam *awgp;
		char *message;

		BOOL MyViWrite(std::string command);
		BOOL ErrorProcess(ViStatus status);
		BOOL OPC();
	};
}
