#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STARTDATA 0
#define INTERVAL 1//[“_]
#define NORMRIZE_TIME 1e9
#define LENGTH 4000000
#define EPS 1e-8

struct Histdata {
	double min, max, div, minData, maxData, median, average;
	int bin, *data, n;
};

double* Get_wfm_from_wfm(int *rec_length, double *sampling_interval, double *horizontal_offset, char *filename);
int file_seek_abs(FILE *fp, int position);
int read_byte2char(char *out, int num, FILE *fp, int reverse_flag);
void SetHistgramParameter(struct Histdata *histgram, double min, double max, int bin);
void InputHistgram(struct Histdata *histgram, double inputData);
void OutputHistgram(struct Histdata *histgram, FILE* outputfp);

int main(int argc, char *argv[]) {
	int i, rec_length;
	double *wfm, sampling_interval, horizontal_offset;
	char buf[1024], filename[1024], out_filename[1024], *file;
	struct Histdata histgram;
	FILE *outputfp;
	
	outputfp = fopen("bifurcation.txt", "w");
	
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		sscanf(buf, "%s", filename);
		
		wfm = Get_wfm_from_wfm(&rec_length, &sampling_interval, &horizontal_offset, filename);
		
		SetHistgramParameter(&histgram, -0.05, 0.05, 50);
		
		for(i = 0; i < rec_length; i++) InputHistgram(&histgram, wfm[i]);
		
		OutputHistgram(&histgram, outputfp);
		
		free(wfm);
	}
	
	fclose(outputfp);
	
	return 0;
}

double* Get_wfm_from_wfm(int *rec_length, double *sampling_interval, double *horizontal_offset, char *filename) {
	double scale_volt, offset_volt, *wfm;
	int i, tmp_int;
	unsigned long tmp_ulong;
	short tmp_short;//16G oscilloscope
	char tmp_char, read_char[128];//23G oscilloscope
	FILE *fp;
	
	double *double_ptr;
	int *int_ptr;
	unsigned long *ulong_ptr;
	short int *short_ptr;
	int reverse_flag = 1;
	
	if((fp = fopen(filename, "rb")) == NULL) {
		fprintf(stderr, "inputfile open error\n");
		return NULL;
	}
	
	if (file_seek_abs(fp, 2) != 0) return NULL;
	if(fread(&read_char, sizeof(char), 8, fp) < 1) { fprintf(stderr, "File read error\n"); return NULL; }
	read_char[8] = '\0';
	// printf("FILE Version : %s\n", read_char);
	
	/********** Y scale **********/
	if (file_seek_abs(fp, 168 - 2) != 0) return NULL;
	read_byte2char(read_char, 8, fp, reverse_flag);
	double_ptr = (double *)read_char;
	scale_volt = *double_ptr;
	/********** Y scale **********/
	
	/********** Y offset  **********/
	if (file_seek_abs(fp, 176 - 2) != 0) return NULL;
	read_byte2char(read_char, 8, fp, reverse_flag);
	double_ptr = (double *)read_char;
	offset_volt = *double_ptr;
	/********** Y offset  **********/
	
	/********** Format **********/
	if (file_seek_abs(fp, 240 - 2) != 0) return NULL;
	read_byte2char(read_char, 4, fp, reverse_flag);
	int_ptr = (int *)read_char;
	// printf("Format : %d\n", (int)(*int_ptr));
	/********** Format  **********/
	
	/********** Sampling interval (Horizontal scale) **********/
	if (file_seek_abs(fp, 488 - 10) != 0) return NULL;
	read_byte2char(read_char, 8, fp, reverse_flag);
	double_ptr = (double *)read_char;
	*sampling_interval = *double_ptr;
	/********** Sampling interval (Horizontal scale) **********/
	
	/********** Horizontal offset **********/
	if (file_seek_abs(fp, 496 - 10) != 0) return NULL;
	read_byte2char(read_char, 8, fp, reverse_flag);
	double_ptr = (double *)read_char;
	*horizontal_offset = *double_ptr;
	/********** Horizontal offset **********/
	
	/*** Record length (including pre and post charge data --- 32) ***/
	if (file_seek_abs(fp, 504 - 10) != 0) return NULL;
	read_byte2char(read_char, 4, fp, reverse_flag);
	ulong_ptr = (unsigned long *)read_char;
	*rec_length = *ulong_ptr - 32;
	/*** Record length (including pre and post charge data --- 32 points) ***/
	
	wfm = (double*)malloc(sizeof(double) * *rec_length);
	int j;
	if (file_seek_abs(fp, 870 - 18) != 0) return NULL;
	for(i = 0; i < *rec_length; i++) {
		/***** 16 G oscilloscope *****/
		// if(fread(&tmp_short, sizeof(short), 1, fp) < 1) { fprintf(stderr, "File read error\n"); return NULL; }
		// wfm[i] = (double)tmp_short * scale_volt + offset_volt;
		/***** 16 G oscilloscope *****/
		/***** 23 G oscilloscope *****/
		// if(fread(&tmp_char, sizeof(char), 1, fp) < 1) { fprintf(stderr, "File read error\n"); return NULL; }
		// wfm[i] = (double)tmp_char * scale_volt + offset_volt;
		/***** 23 G oscilloscope *****/
		read_byte2char(read_char, 2, fp, reverse_flag);
		short_ptr = (short int*)read_char;
		wfm[i] = (double)(*short_ptr * scale_volt + offset_volt);
	}
	
	fclose(fp);
	
	return wfm;
}

int file_seek_abs(FILE *fp, int position) {
	if (fseek(fp, position, SEEK_SET) != 0) {
		fprintf(stderr, "Seek error.\n");
		return -1;
	}
	return 0;
}

int read_byte2char(char *out, int num, FILE *fp, int reverse_flag) {
	int idx;
	char tmp;
	
	if(reverse_flag == 0) {
		idx = 0;
		while(fread(&tmp, 1, 1, fp) >= 1 && idx < num) {
			out[idx++] = tmp;
			if (idx == num) break;
		}
		out[idx] = '\0';
	}
	else {
		idx = num - 1;
		while(fread(&tmp, 1, 1, fp) >= 1 && idx < num) {
			out[idx--] = tmp;
			if (idx < 0) break;
		}
		out[num] = '\0';
	}
	
	return 0;
}

void SetHistgramParameter(struct Histdata *histgram, double min, double max, int bin) {
	int i;
	
	histgram->data = (int*)malloc(sizeof(int) * bin);
	if(histgram->data == NULL) fprintf(stderr, "error : histgram\n");
	histgram->min = min;
	histgram->max = max;
	histgram->bin = bin;
	histgram->div = (max - min) / bin;
	for(i = 0; i < bin; i++) histgram->data[i] = 0;
	histgram->minData = histgram->max;
	histgram->maxData = histgram->min;
	histgram->average = 0.0;
	histgram->n = 0;
	
	return;
}

void InputHistgram(struct Histdata *histgram, double inputData) {
	
	if(inputData >= histgram->min && inputData <= histgram->max) histgram->data[(int)((inputData - histgram->min) / histgram->div)]++;
	if(inputData < histgram->minData) histgram->minData = inputData;
	if(inputData > histgram->maxData) histgram->maxData = inputData;
	histgram->average += inputData;
	histgram->n++;
	
	return;
}

void OutputHistgram(struct Histdata *histgram, FILE* outputfp) {
	int i, maxDatanum;
	// FILE *outputfp;
	
	for(i = 0; i < histgram->bin; i++) fprintf(outputfp, "%e\t", (double)histgram->data[i] / (double)(histgram->n));
	
	fseek(outputfp, -1, SEEK_CUR);
	fprintf(outputfp, "\n");
	
	free(histgram->data);
	
	// histgram->average /= histgram->n;
	// for(i = 0, maxDatanum = 0; i < histgram->bin; i++) {
		// if(histgram->data[i] > maxDatanum) {
			// maxDatanum = histgram->data[i];
			// histgram->median = (i + 0.5) * histgram->div + histgram->min;
		// }
	// }
	// outputfp = fopen(filename, "w");
	// fprintf(outputfp, "Maximum data : %e\nMinimum data : %e\nMedian : %e\nAverage : %e\n", histgram->maxData, histgram->minData, histgram->median, histgram->average);
	// fprintf(outputfp, "Data\tProbability\tThe number of points\n");
	// for(i = 0; i < histgram->bin; i++) fprintf(outputfp, "%e\t%e\t%d\n", (i + 0.5) * histgram->div + histgram->min, (double)histgram->data[i] / (double)(histgram->n), histgram->data[i]);
	// free(histgram->data);
	// fclose(outputfp);
	
	return;
}

