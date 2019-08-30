#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STARTDATA 0//204760//219037//203500//1020280
#define INTERVAL 10
#define NORMRIZE_TIME 1e6
#define LENGTH 500000

double* Get_wfm_from_wfm(int *rec_length, double *sampling_interval, double *horizontal_offset, char *filename);
int file_seek_abs(FILE *fp, int position);
int read_byte2char(char *out, int num, FILE *fp, int reverse_flag);

int main(int argc, char *argv[]) {
	char filename[1024], filename1[1050], *file, buf[1024];
	FILE *fout;
	int i, j, rec_length, trigger_point, num_ch;
	double sampling_interval, horizontal_offset, *wfm;
	
	while(fgets(buf, sizeof(buf), stdin) != NULL) {
		sscanf(buf, "%s", filename);
		
		// wfm = Get_wfm_from_wfm(&rec_length, &sampling_interval, &trigger_point, &num_ch, filename);
		wfm = Get_wfm_from_wfm(&rec_length, &sampling_interval, &horizontal_offset, filename);
		
		file = strtok(filename, ".");
		sprintf(filename1, "%s_%d_%d.txt\0", file, INTERVAL, STARTDATA, LENGTH);
		fout = fopen(filename1, "w");
		fprintf(fout, "Time [\\fm\\ns]\tIntensity [arb. units]\n");
		
		for(i = STARTDATA; i < rec_length; i++) {
			if(i > STARTDATA + LENGTH) break;
			else {
				if((i-STARTDATA) % INTERVAL == 0) fprintf(fout, "%e\t%e\n", (i * sampling_interval + horizontal_offset) * NORMRIZE_TIME, wfm[i]);
			}
		}
		
		fclose(fout);
	}

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
