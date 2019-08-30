#include "stdafx.h"
#include "WaveCalculator.h"

namespace InternalProcess
{
	WaveCalculator::WaveCalculator(OSCParam *gbs, char waveform[])
	{
		oscp = gbs;
		for (int i = 0; i < 2; i++)
		{
			if (oscp->DebugFlag)
			{
				wave[i] = waveform + oscp->RecordDataLength * i;
			}
			else
			{
				wave[i] = waveform + oscp->RecordHeaderLength + (oscp->RecordHeaderLength + oscp->RecordDataLength) * i;
			}
		}
	}

	inline double WaveCalculator::GetWaveValue(int ch, int idx) const
	{
		return oscp->YZero[ch] + oscp->YMult[ch] * ((double)wave[ch][idx + 6] - oscp->YOff[ch]);
	}

	BOOL WaveCalculator::MakeBitfromWFM(char bit[], int data_num, int decimation_point)
	{
		int count = 0;
		double sum(0.0);

		for (int i = 0; i < data_num; i++)
		{
			sum += GetWaveValue(RES1, i);
		}

		double ave(sum / data_num);

		for (int i = 0; i < data_num; i += decimation_point)
		{
			if (GetWaveValue(RES1, i) > ave) bit[count] = '1';
			else bit[count] = '0';
			count++;
		}

		return TRUE;
	}

	BOOL WaveCalculator::AcqWaveform(double wfm[])
	{
		for (int i = 0; i < 2500; i++)
		{
			wfm[i] = GetWaveValue(RES1, i);
		}

		return TRUE;
	}
}