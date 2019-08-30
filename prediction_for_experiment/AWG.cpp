#include "stdafx.h"
#include "AWG.h"

namespace InternalProcess
{
	BOOL AWG::InitAWG()
	{
		ViStatus status;
		char command[256];

		status = viOpenDefaultRM(&(awgp->rm));
		if (status < VI_SUCCESS) return ErrorProcess(status);

		status = viOpen(awgp->rm, "USB0::0x0957::0x4B07::MY53400955::0::INSTR", VI_NULL, VI_NULL, &(awgp->vi));
		if (status < VI_SUCCESS) return ErrorProcess(status);

		if (awgp->Timeout == 0) awgp->Timeout = INFINITE;
		status = viSetAttribute(awgp->vi, VI_ATTR_TMO_VALUE, awgp->Timeout);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		return TRUE;
	}

	BOOL AWG::SetParam()
	{
		if (!MyViWrite("VOLT:OFFS 0")) return FALSE;
		//if (!MyViWrite("VOLT MAX")) return FALSE;
		if (!MyViWrite("SOURCE1:VOLT 1")) return FALSE;
		if (!MyViWrite("SOURCE2:VOLT 1")) return FALSE;

		return TRUE;
	}

	BOOL AWG::ChannelOutSwitch(int ch)
	{
		ViStatus status;
		char command[256];
		int outState;

		sprintf_s(command, 256, "OUTPut%d?\n", ch);
		status = viQueryf(awgp->vi, command, "%d", &outState);
		if (status < VI_SUCCESS || (!OPC())) return ErrorProcess(status);

		if (outState == 0)
		{
			sprintf_s(command, 256, "OUTPut%d ON\n", ch);
			if (!MyViWrite(command)) return FALSE;
		}
		else
		{
			sprintf_s(command, 256, "OUTPut%d OFF\n", ch);
			if (!MyViWrite(command)) return FALSE;
		}
		
		return TRUE;
	}

	BOOL AWG::ChgFunction(int ch, std::string function)
	{
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":FUNC " + function)) return FALSE;

		return TRUE;
	}

	BOOL AWG::GenTRG()
	{
		if (!MyViWrite("*TRG\n")) return FALSE;
		return TRUE;
	}

	BOOL AWG::TransportWFMdata(int ch, float *wfm, int num)
	{
		ViStatus status;
		ViPUInt32 retCnt(0);
		
		char *transform_data, char_tmp[1024];
		unsigned char *uchar_tmp = new unsigned char[4];
		int i;
		
		int char_num, byte_num, hedder_num;

		if (num * 4 < 10) byte_num = 1;
		else if (num * 4 >= 10 && num * 4 < 100) byte_num = 2;
		else if (num * 4 >= 100 && num * 4 < 1000) byte_num = 3;
		else if (num * 4 >= 1000 && num * 4 < 10000) byte_num = 4;
		else if (num * 4 >= 10000 && num * 4 < 100000) byte_num = 5;
		else if (num * 4 >= 100000 && num * 4 < 1000000) byte_num = 6;
		else if (num * 4 >= 1000000 && num * 4 < 10000000) byte_num = 7;
		else if (num * 4 >= 10000000 && num * 4 < 100000000) byte_num = 8;

		
		//sprintf_s(char_tmp, 1024, "DATA:ARB MYARB,#%d%d", byte_num, num * 4);
		sprintf_s(char_tmp, 1024, "SOURCE%d:DATA:ARB MYARB,#%d%d", ch, byte_num, num * 4);
		hedder_num = (int)strlen(char_tmp);
		char_num = hedder_num + num * 4;
		
		transform_data = new char[char_num];
		sprintf_s(transform_data, char_num, "%s", char_tmp);
		
		for (i = 0; i < num; i++) {
			uchar_tmp = reinterpret_cast<unsigned char(&)[sizeof(float)]>(wfm[i]);
			transform_data[hedder_num + i * 4 + 0] = uchar_tmp[3];
			transform_data[hedder_num + i * 4 + 1] = uchar_tmp[2];
			transform_data[hedder_num + i * 4 + 2] = uchar_tmp[1];
			transform_data[hedder_num + i * 4 + 3] = uchar_tmp[0];
		}

		status = viWrite(awgp->vi, (ViBuf)transform_data, char_num, retCnt);
		if (status < VI_SUCCESS || (!OPC())) return ErrorProcess(status);

		delete transform_data;
		
		/*
		sprintf_s(char_tmp, 1024, "DATA:ARB MYARB1,#320000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
		char_num = (int)strlen(char_tmp);
		
		uchar_tmp = reinterpret_cast<unsigned char(&)[sizeof(float)]>(wfm[0]);
		char_tmp[char_num + 0] = uchar_tmp[0];
		char_tmp[char_num + 1] = uchar_tmp[1];
		char_tmp[char_num + 2] = uchar_tmp[2];
		char_tmp[char_num + 3] = uchar_tmp[3];
		char_num = (int)strlen(char_tmp) + 1;
		
		status = viWrite(awgp->vi, (ViBuf)char_tmp, char_num, retCnt);
		if (status < VI_SUCCESS || (!OPC())) return ErrorProcess(status);
		
		Sleep(200);

		if (!MyViWrite("MMEM:LOAD:DATA \"INT:\\DC0V.arb\"")) return FALSE;
		
		Sleep(200);

		//if (!MyViWrite("DATA:SEQ #279\"seqExample\",\"MYARB\",0,once,maintain,10,\"INT:\\DC0V.arb\",0,repeatInf,maintain,10")) return FALSE;
		
		Sleep(200);
		*/

		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":FUNC ARB")) return FALSE;
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":FUNC:ARB:SRAT 100e6")) return FALSE;
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":FUNC:ARB:FILT OFF")) return FALSE;
		if (!MyViWrite("FORMat:BORDer NORM")) return FALSE;
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":FUNC:ARB MYARB")) return FALSE;

		return TRUE;
	}

	BOOL AWG::ClearMemory(int ch)
	{

		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":DATA:VOL:CLE")) return FALSE;

		return TRUE;
	}

	BOOL AWG::Burst(int ch, std::string command)
	{

		if (!MyViWrite("UNIT:ANGLE DEG ")) return FALSE;
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":BURSt:NCYCles 1")) return FALSE;
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":BURSt:PHASe 0")) return FALSE;
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":BURST:MODE TRIG")) return FALSE;
		if (!MyViWrite("TRIGger" + std::to_string(ch) + ":SOURCE BUS")) return FALSE;
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":BURSt:STATe " + command)) return FALSE;
	
		return TRUE;
	}

	BOOL AWG::SYNC(int ch)
	{
		if (!MyViWrite("SOURCE" + std::to_string(ch) + ":FUNC:ARB:SYNC")) return FALSE;

		return TRUE;
	}

	BOOL AWG::TermAWG()
	{
		ViStatus status;
		ViUInt32 retCnt;

		status = viWrite(awgp->vi, (ViBuf) "*CLS", 4, &retCnt);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		if (awgp->vi != VI_NULL) status = viClose(awgp->vi);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		if (awgp->rm != VI_NULL) status = viClose(awgp->rm);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		return TRUE;
	}

	BOOL AWG::MyViWrite(std::string command)
	{
		ViPUInt32 retCnt(0);
		ViStatus status(viWrite(awgp->vi, (ViBuf)command.c_str(), (ViUInt32)command.length(), retCnt));
		if (status < VI_SUCCESS || (!OPC())) return ErrorProcess(status);
		
		return TRUE;
	}

	BOOL AWG::ErrorProcess(ViStatus status)
	{
		viStatusDesc(awgp->vi, status, message);
		return FALSE;
	}

	BOOL AWG::OPC()
	{
		ViUInt32 retCnt;
		ViStatus status;
		char buf[100];

		status = viWrite(awgp->vi, (ViBuf)"*OPC?", 5, &retCnt);
		if (status < VI_SUCCESS) return ErrorProcess(status);
		
		status = viRead(awgp->vi, (ViBuf)buf, 100, &retCnt);
		if (status < VI_SUCCESS) return ErrorProcess(status);

		if (buf[0] != 0x31) return FALSE;
		
		return TRUE;
	}
}