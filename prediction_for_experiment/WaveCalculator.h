#pragma once
#include "StructCPP.h"

namespace InternalProcess
{
	class WaveCalculator
	{
	public:
		WaveCalculator(OSCParam *c, char waveform[]);
		BOOL MakeBitfromWFM(char bit[], int data_num, int decimation_point);
		BOOL AcqWaveform(double wfm[]);

	private:
		const OSCParam *oscp;
		const char *wave[2];

		double GetWaveValue(int ch, int idx) const;

		void printResult(int ReadLength, double sampleFreq, double corre1_2, double corre3_4, double corre1_3, double corre2_4){
			std::ofstream ofs(oscp->Filename, std::ios::app);
			ofs << ReadLength << '\t' << sampleFreq << '\t' << corre1_2 << '\t' << corre3_4 << '\t' << corre1_3 << '\t' << corre2_4 << std::endl;
 		}
	};
}