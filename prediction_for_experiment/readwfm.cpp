#include "stdafx.h"
#include "readwfm.h"
#include <random>

namespace InternalProcess
{
	void readwfm::initreadwfm()
	{
		fname[0] = '\0';
		sampling_rate = 100e6;
		input_time_interval = 10e-6;
		node_num = 50;
		node_interval = sampling_rate * input_time_interval / node_num;
		predicted_twfm_length = 4000;

		return;
	}
	
	float* readwfm::out_wfm(char *filename)
	{
		float twfm_tmp[5000], tmp_float;
		int i, j, k;
		FILE *fp_in;
		errno_t err;

		sprintf_s(fname, "%s", filename);
		
		if ((err = fopen_s(&fp_in, fname, "r")) != 0)
		{
			printf("%d: Cannot open santafe twfm %s\n", err, fname);
			return 0;
		}

		i = 0;
		while (fscanf_s(fp_in, "%f", &tmp_float) != EOF && i < predicted_twfm_length)
		{
			twfm_tmp[i++] = tmp_float;
		}

		predicted_twfm_length = i;

		fclose(fp_in);

		int idx, wfm_buffer = 10;
		float *wfm = new float[node_num * predicted_twfm_length * node_interval + wfm_buffer];
		float tmp;

		wfm_num = node_num * predicted_twfm_length * node_interval;

		max_val = 0.0;
		for (k = 0; k < predicted_twfm_length; k++) {
			for (j = 0; j < node_num; j++)
			{
				idx = wfm_buffer / 2 + k * node_num * node_interval + j * node_interval;

				for (i = 0; i < node_interval; i++) wfm[idx + i] = mask[j] * twfm_tmp[k];
				
				if (abs(wfm[idx]) > max_val) max_val = abs(wfm[idx]);
			}
		}
		
		for (i = 0; i < wfm_num; i++) {
			wfm[wfm_buffer / 2 + i] /= 255;
		}

		for (i = 0; i < wfm_buffer / 2; i++)
		{
			wfm[i] = wfm[wfm_buffer / 2 + wfm_num + i] = 0.0;
		}
		wfm_num += wfm_buffer;
		
		return wfm;
	}

	void readwfm::makemask(int seed, char *maskfile)
	{
		int i, j, index, num = 86 * node_num, non_zero_num_half = num / 10 / 2;

		mask = new float[num];
		for (i = 0; i < num; i++) mask[i] = 0.0;

		if (seed == 0) {

			FILE *fp_in;
			errno_t err;
			float tmp_float;

			if ((err = fopen_s(&fp_in, maskfile, "r")) != 0)
			{
				printf("%d: Cannot open mask %s\n", err, maskfile);
				return;
			}

			i = 0;
			while (fscanf_s(fp_in, "%f", &tmp_float) != EOF || i < node_num)
			{
				mask[i++] = tmp_float;
			}

			fclose(fp_in);

		}

		else {

			std::mt19937 mt;
			mt.seed(seed);
			std::uniform_int_distribution<> rand100(0, num);

			for (i = 0; i < non_zero_num_half; i++) {
				index = (int)(rand100(mt)) % num;
				if (mask[index] == 0) mask[index] = 1;
				else i--;
			}

			for (i = 0; i < non_zero_num_half; i++) {
				index = (int)(rand100(mt)) % num;
				if (mask[index] == 0) mask[index] = -1;
				else i--;
			}

		}

		return;
	}

	float* readwfm::genTriggerSignal(int start_idx, int width, int buffer)
	{
		float *trigger_signal = new float[start_idx + width + buffer];
		
		for (int i = 0; i < start_idx; i++) trigger_signal[i] = 0;
		for (int i = start_idx; i < start_idx + width; i++) trigger_signal[i] = 0.5;
		for (int i = start_idx + width; i < start_idx + width + buffer; i++) trigger_signal[i] = 0;

		return trigger_signal;
	}
}