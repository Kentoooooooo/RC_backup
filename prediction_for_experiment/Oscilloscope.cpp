#include "stdafx.h"
#include "Oscilloscope.h"

namespace InternalProcess
{
	BOOL Oscilloscope::InitOscillo()
	{
		ViStatus status;
		char buf[128];
		long maxRecLength = 100;
		
		status = viOpenDefaultRM(&(oscp->rm));
		if (status < VI_SUCCESS) return ErrorProcess(status);

		status = viOpen(oscp->rm, "TCPIP0::169.254.247.226::inst0::INSTR", VI_NULL, VI_NULL, &(oscp->vi));
		if (status < VI_SUCCESS) return ErrorProcess(status);

		status = viSetAttribute(oscp->vi, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);
		if (!OPC()) return ErrorProcess(status);
		status = viSetAttribute(oscp->vi, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS);
		if (!OPC()) return ErrorProcess(status);

		if (oscp->Timeout == 0) oscp->Timeout = INFINITE;
		status = viSetAttribute(oscp->vi, VI_ATTR_TMO_VALUE, oscp->Timeout);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		if (!MyViWrite("DATA:ENCDG SRIBINARY")) return FALSE;
		if (!MyViWrite("WFMInpre:BYT_Nr 1")) return FALSE;

		if (!MyViWrite("DATa:SOU CH1")) return FALSE;

		status = viPrintf(oscp->vi, "data:start %d;data:stop %d\n", 1, oscp->RecordDataLength);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		status = viQueryf(oscp->vi, "WFMInpre:YOFf?\n", "%lf", &(oscp->YOff[0]));
		if (status < VI_SUCCESS) return ErrorProcess(status);
		status = viQueryf(oscp->vi, "WFMInpre:YMULt?\n", "%lf", &(oscp->YMult[0]));
		if (status < VI_SUCCESS) return ErrorProcess(status);
		status = viQueryf(oscp->vi, "WFMInpre:YZERo?\n", "%lf", &(oscp->YZero[0]));
		if (status < VI_SUCCESS) return ErrorProcess(status);

		status = viQueryf(oscp->vi, "HORizontal:MAIn:SCAle?", "%lf", &(oscp->HorizontalScale));
		if (status < VI_SUCCESS) return ErrorProcess(status);
		status = viQueryf(oscp->vi, "WFMInpre:XINcr?", "%lf", &(oscp->SamplingInterval));
		if (status < VI_SUCCESS) return ErrorProcess(status);

		

		return TRUE;
	}

	BOOL Oscilloscope::GetWave()
	{
		ViStatus status;
		ViUInt32 retCnt;
		
		if (!MyViWrite("ACQ:STATE 1")) return FALSE;

		status = viWrite(oscp->vi, (ViBuf) "CURVE?", 6, &retCnt);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		status = viRead(oscp->vi, (ViBuf)waveform, oscp->RecordBufferLength, &retCnt);
		if (status < VI_SUCCESS) return ErrorProcess(status);
		
		return TRUE;
	}
	
	BOOL Oscilloscope::Acq_Mode_Chg(int out_mode_flag, std::string runmode)
	{
		char command[256];

		if(out_mode_flag == 1) sprintf_s(command, 256, "ACQuire:STOPAfter RUNSTop\n");
		if(out_mode_flag == 0) sprintf_s(command, 256, "ACQuire:STOPAfter SEQuence\n");

		if (!MyViWrite(command)) return FALSE;
		if (!MyViWrite("ACQ:STATE " + runmode)) return FALSE;

		return TRUE;
	}

	BOOL Oscilloscope::SetSaveFileFormat(std::string format)
	{
		char command[256];

		sprintf_s(command, 256, "SAVe:WAVEform:FILEFormat %s\n", format);
		if (!MyViWrite(command)) return FALSE;

		return TRUE;
	}

	BOOL Oscilloscope::SaveWfm(int ch, char *dest_name)
	{

		char command[256];

		sprintf_s(command, 256, "SAVe:WAVEform CH%d,\"%s\"\n", ch, dest_name);
		printf("%s\n", command);
		if (!MyViWrite(command)) return FALSE;

		return TRUE;
	}

	BOOL Oscilloscope::TermOscillo()
	{
		ViStatus status;
		ViUInt32 retCnt;
		
		status = viWrite(oscp->vi, (ViBuf) "*CLS", 4, &retCnt);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		if (oscp->vi != VI_NULL) status = viClose(oscp->vi);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		if (oscp->rm != VI_NULL) status = viClose(oscp->rm);
		if (status < VI_SUCCESS) return ErrorProcess(status);
		
		return TRUE;
	}

	BOOL Oscilloscope::MyViWrite(std::string command)
	{
		
		ViPUInt32 retCnt(0);
		ViStatus status(viWrite(oscp->vi, (ViBuf)command.c_str(), (ViUInt32)command.length(), retCnt));
		if (status < VI_SUCCESS || (!OPC())) return ErrorProcess(status);
		
		return TRUE;
	}

	BOOL Oscilloscope::ErrorProcess(ViStatus status)
	{
		viStatusDesc(oscp->vi, status, message);
		return FALSE;
	}

	BOOL Oscilloscope::OPC()
	{
		ViUInt32 retCnt;
		ViStatus status;
		char buf[100];
		
		status = viWrite(oscp->vi, (ViBuf)"*OPC?", 5, &retCnt);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		status = viRead(oscp->vi, (ViBuf)buf, 100, &retCnt);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		if (buf[0] != 0x31) return FALSE;
		
		return TRUE;
	}

	BOOL Oscilloscope::GetWaveDebug(int n)
	{
		using namespace std;

		if (n >= 80) return FALSE;

		string filename("C:\\MySpace\\WorkSpace\\Program\\C#\\test\\20140711\\1278288084");
		string headerName(string(filename + "_header.txt"));
		ifstream header(headerName.c_str());

		if (!header.is_open()) return FALSE;

		for (int i = 0; i < 2; i++)
		{
			header >> oscp->YZero[i] >> oscp->YMult[i] >> oscp->YOff[i];

			string ch("ch1");
			ch[2] += i;
			string waveName(filename + ".bin");
			ifstream wave(waveName.c_str(), ios::binary);
			if (!wave.is_open()) return FALSE;
			wave.read(waveform + i * oscp->RecordDataLength, oscp->RecordDataLength);
		}

		return TRUE;
	}
}