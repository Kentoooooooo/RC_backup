#include <stdio.h>
#define OFFSET 127
int main(int argc, char **argv) {
	FILE *fp,*output;
	int hist[256] = { 0 };
	int datanum=0;
	char filename[1024];
	char c;
	int i;
	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("Error:input file error\n");
		return 1;
	}
	sprintf(filename, "%s_histgram.log", argv[1]);
	output = fopen(filename, "r");
	if (output != NULL) {
		for (i = 0; i < 256; i++) {
			fscanf(output, "%*d\t%d\t%*lf\n", hist[i]);
			datanum += hist[i];
		}
		fclose(output);
	}
	while (c = fgetc(fp)) {
		if (c == '\n') {
			break;
		}
	}
	while (fscanf(fp,"%*lf\t%d\n",&i) != EOF) {
		hist[i + OFFSET]++;
		datanum++;
	}
	fclose(fp);
	output = fopen("./wfm/histgram.log", "w");
	for (i = 0; i < 256; i++)
		fprintf(output, "%d\t%d\t%f\n", i - OFFSET, hist[i], (double)hist[i] / datanum);
	fclose(output);
	return 0;
}