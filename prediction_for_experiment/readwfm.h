#pragma once
#include "StructCPP.h"

namespace InternalProcess
{
	class readwfm
	{
	public:
		float *mask;
		int predicted_twfm_length;
		int node_num;
		int node_interval;
		int wfm_num;
		double sampling_rate, input_time_interval;

		void initreadwfm();
		float* out_wfm(char *filename);
		void makemask(int seed, char *maskfile);
	private:
		char fname[128];
		int channel;
		float max_val;

		float *genTriggerSignal(int start_idx, int width, int buffer);
	};
}