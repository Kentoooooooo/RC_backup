#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STARTDATA 0
#define INTERVAL 1//[“_]
#define NORMRIZE_TIME 1e6
#define LENGTH 4000000
#define EPS 1e-8

struct Histdata {
	double min, max, div, minData, maxData, median, average, sq_sum, var;
	int bin, *data, n;
};

void tranceport(char *a, char *b);
int ReadCSV(double *wfm, char *filename);
void SetHistgramParameter(struct Histdata *histgram, double min, double max, int bin);
void InputHistgram(struct Histdata *histgram, double inputData);
void OutputHistgram(struct Histdata *histgram, char* filename);

int main(int argc, char *argv[]) {
	int i, num;
	double *wfm;
	char buf[1024], filename[1024], out_filename[1024], *file;
	struct Histdata histgram;
	
	wfm = (double*)malloc(sizeof(double) * LENGTH);
	
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		sscanf(buf, "%s", filename);
		
		num = ReadCSV(wfm, filename);
		
		SetHistgramParameter(&histgram, -0.05, 0.05, 50);
		
		for(i = 0; i < num; i++) InputHistgram(&histgram, wfm[i]);
		
		file = strtok(filename, ".");
		sprintf(out_filename, "%s_histgram.txt\0", file);
		
		OutputHistgram(&histgram, out_filename);
	}
	
	free(wfm);
	
	return 0;
}

int ReadCSV(double *wfm, char *filename) {
	FILE *fin;
	char buf[1024], substr[1024], str[1024];
	int i, point_num;
	double data_time, data_intensity;
	
	fin = fopen(filename, "r");
	
	point_num = 0;
	
	while(fgets(substr, sizeof(substr), fin) != NULL && point_num < LENGTH) {
		if(point_num >= STARTDATA) {
			if(point_num % INTERVAL == 0) {
				tranceport(substr, str);
				
				strcpy(buf, strtok(str, ","));
				
				for(i = 1; i < 3; i++) {
					strcpy(buf, strtok(NULL, ","));
				}
				data_time = atof(strtok(NULL, ","));
				data_intensity = atof(strtok(NULL, ",\n"));
				wfm[(point_num - STARTDATA) / INTERVAL] = data_intensity;
			}
		}
		point_num++;
	}
	
	/*
	while(fscanf(fin, "%lf", &data_intensity) != EOF && point_num < LENGTH) {
		// fscanf(fin, "%lf", &data_intensity);
		if(point_num >= STARTDATA) {
			if(point_num % INTERVAL == 0) {
				wfm[(point_num - STARTDATA) / INTERVAL] = data_intensity;
			}
		}
		point_num++;
	}
	*/
	return (point_num - STARTDATA) / INTERVAL;
}

void tranceport(char *a, char *b) {
	//while(*b++ = *a) if(*a++ == ',') *b++ = ' ';
	if(*a == ',') *b++ = 'b';
	while(*b++ = *a) {
		if(*a == ',' && *(a + 1) == ',') *b++ = 'a';
		a++;
	}
	return;
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
	histgram->average = histgram->sq_sum = 0.0;
	histgram->n = 0;
	
	return;
}

void InputHistgram(struct Histdata *histgram, double inputData) {
	
	if(inputData >= histgram->min && inputData <= histgram->max) histgram->data[(int)((inputData - histgram->min) / histgram->div)]++;
	if(inputData < histgram->minData) histgram->minData = inputData;
	if(inputData > histgram->maxData) histgram->maxData = inputData;
	histgram->average += inputData;
	histgram->sq_sum += inputData * inputData;
	histgram->n++;
	
	return;
}

void OutputHistgram(struct Histdata *histgram, char* filename) {
	int i, maxDatanum;
	FILE *outputfp;
	
	histgram->average /= histgram->n;
	histgram->var = histgram->sq_sum / histgram->n - histgram->average * histgram->average;
	for(i = 0, maxDatanum = 0; i < histgram->bin; i++) {
		if(histgram->data[i] > maxDatanum) {
			maxDatanum = histgram->data[i];
			histgram->median = (i + 0.5) * histgram->div + histgram->min;
		}
	}
	outputfp = fopen(filename, "w");
	fprintf(outputfp, "Maximum data : %e\nMinimum data : %e\nMedian : %e\nAverage : %e\nVariance : %e\n", histgram->maxData, histgram->minData, histgram->median, histgram->average, histgram->var);
	fprintf(outputfp, "Data\tProbability\tThe number of points\n");
	for(i = 0; i < histgram->bin; i++) fprintf(outputfp, "%e\t%e\t%d\n", (i + 0.5) * histgram->div + histgram->min, (double)histgram->data[i] / (double)(histgram->n), histgram->data[i]);
	free(histgram->data);
	fclose(outputfp);
	
	return;
}

