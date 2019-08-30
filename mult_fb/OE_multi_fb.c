#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define PI 3.1415926535897932384626433832795
#define C 2.99792458e8
#define h 1e-10
#define dh 1e-4
#define EPS 1e-8

struct parameter {
	double tau_L, tau_H, delay[10], beta, phi, epsilon, weight_fb[10];
};

struct LaserEOfeedback {
	struct parameter p;
	double x, y, *xdelay;
	int descretized_num[10], fbNum, fbIndex[10], long_fb;
};

int set_param(struct LaserEOfeedback *laser, int out, int para_flag, double param_max);
void CalcLaserEOfeedback(struct LaserEOfeedback *laser);
void AutoCorrelation(double *Data, int N, double timeStep, char *filename);

int main(int argc, char *argv[]) {
	struct LaserEOfeedback laser;
	double param, param_min, param_max, param_interval, t_transient, t_plot;
	double buff[8], *wfmData[8];
	int i, j, k, n, p, s, t, transient, plot, divi, out, para_flag, iteration, plotnum, feedback_memory;
	char buffer_ch[128];
	clock_t start, end;
	FILE *inputfp, *outputfp[4];
	
	//	ファイルからのデータの読み込み
	if (argc == 2) {
		inputfp = fopen(argv[1], "r");
		if(inputfp == NULL) {
			fprintf(stderr, "%sを開くことができません", argv[1]);
			exit(2);
		}
		fscanf(inputfp, "%lf", &(laser.p.tau_L));
		fscanf(inputfp, "%lf", &(laser.p.tau_H));
		fscanf(inputfp, "%d", &(laser.fbNum));
		for(i = 0; i < laser.fbNum; i++) fscanf(inputfp, "%lf", &(laser.p.delay[i]));
		fscanf(inputfp, "%lf", &(laser.p.beta));
		fscanf(inputfp, "%lf", &(laser.p.phi));
		fscanf(inputfp, "%lf %lf %lf", &param_min, &param_max, &param_interval);
		fscanf(inputfp, "%lf %lf", &t_transient, &t_plot);
		fscanf(inputfp, "%d", &divi);
		
		fclose(inputfp);
	} else {
		fprintf(stderr, "引数のエラーです\n");
		exit(1);
	}
	
	const int amp_max_int = (param_max * param_interval >= 0.0) ? (int)(param_max / param_interval + EPS) : (int)(param_max / param_interval - EPS);
	const int amp_min_int = (param_min * param_interval >= 0.0) ? (int)(param_min / param_interval + EPS) : (int)(param_min / param_interval - EPS);
	
	t_transient *= 1.0e-6;
	t_plot *= 1.0e-6;
	transient = (int)(t_transient / h + EPS);
	plot = (int)(t_plot / h + EPS);
	plotnum = plot / divi;
	printf("Number of calculation : %d \t Number of plot : %d\n", transient + plot, plotnum);
	
	//	出力モード選択部分
	printf("Select a calculation-mode (1 or 2)\n");
	printf("\t 1 --> Temporal waveforms\n");
	printf("\t 2 --> Bifurcation diagram\n");
	printf("inputfp --> ");
	scanf("%d", &out);
	if(out == 1) {
		printf("Select a output-mode (1 or 2 or 3)\n");
		printf("\t 1 --> Temporal waveforms of x(t) and y(t)\n");
		printf("\t 2 --> Auto-correlation function\n");
		printf("inputfp --> ");
		scanf("%d", &para_flag);
		if(para_flag == 1) {
			outputfp[0] = fopen("twfm.txt", "w");
			fprintf(outputfp[0], "Time [\\fm\\ns]\tx(t)\ty(t)\n");
		} else if(para_flag == 2) {
		} else {
			fprintf(stderr, "error : para_flag = %d\n", para_flag);
			exit(1);
		}
	} else if(out == 2) {
		printf("Select a parameter which you change (1 or 2 or 3)\n");
		printf("\t 1 --> Feedback strength beta\n");
		printf("\t 2 --> Feedback delay time 1\n");
		printf("\t 3 --> Bias point for feedback 1\n");
		printf("inputfp --> ");
		scanf("%d", &para_flag);
		if(para_flag == 1) sprintf(buffer_ch, "Feedback strength \\fb\\n");
		if(para_flag == 2) sprintf(buffer_ch, "Feedback delay time 1 \\ft\\n\\d1\\n [ms]");
		if(para_flag == 3) sprintf(buffer_ch, "Bias point");
		outputfp[0] = fopen("bifurcation.txt", "w");
		fprintf(outputfp[0], "%s\tx(t)\n", buffer_ch);
	} else {
		fprintf(stderr, "error : out = %d\n", out);
		exit(1);
	}
	
	set_param(&laser, out, para_flag, param_max);
	
	for(i = 0; i < 1; i++) {
		wfmData[i] = (double*)malloc(sizeof(double) * plotnum);
		if(wfmData[i] == NULL) {
			fprintf(stderr, "error : malloc\n");
			exit(1);
		}
	}
	
	/* ここからレーザデータの初期化 */
	laser.x = 0.05;
	laser.y = 0.0;
	for(i = 0; i < laser.descretized_num[laser.long_fb]; i++) laser.xdelay[i] = 0.05;
	for(i = 0; i < laser.fbNum; i++) laser.fbIndex[i] = laser.descretized_num[laser.long_fb] - laser.descretized_num[i];
	/* ここまでレーザデータの初期化 */
	
	//	メイン計算
	for(s = amp_min_int; s <= amp_max_int; s++) {
		param = param_interval * (double)s;
		
		start = clock();
		
		if(out == 2) {
			if(para_flag == 1) {
				laser.p.beta = param;
				printf("beta = %e\n", laser.p.beta);
			}
			if(para_flag == 2) {
				laser.p.delay[0] = param;
				laser.descretized_num[0] = (int)(laser.p.delay[0] / h + EPS);
				for(i = 0, laser.long_fb = 0; i < laser.fbNum; i++) if(laser.p.delay[i] > laser.p.delay[laser.long_fb]) laser.long_fb = i;
				printf("\nDelay time 1 = %e (Descretized num = %d)\n", laser.p.delay[0], laser.descretized_num[0]);
				param *= 1.0e3;
			}
			if(para_flag == 3) {
				laser.p.phi = param * M_PI;
				printf("\nBias point = %e * PI\n", param);
			}
		}
		
		laser.x = 0.05;
		laser.y = 0.1;
		for(i = 0; i < laser.descretized_num[laser.long_fb]; i++) laser.xdelay[i] = 0.1;
		for(i = 0; i < laser.fbNum; i++) laser.fbIndex[i] = laser.descretized_num[laser.long_fb] - laser.descretized_num[i];
		
		iteration = -10;
		for(i = 0; i < 8; i++) buff[i] = 0.0;
		
		for(i = 0; i < transient; i++) CalcLaserEOfeedback(&laser);
		
		for(i = 1; i <= plot; i++) {
			CalcLaserEOfeedback(&laser);
			
			if(out == 1) {
				if(para_flag == 1) {
					if(i % divi == 0) fprintf(outputfp[0], "%e\t%e\t%e\n", i * dh, laser.x, laser.xdelay[laser.fbIndex[laser.long_fb]]);
				} else if(para_flag == 2) {
					if(i % divi == 0) wfmData[0][i / divi - 1] = laser.x;
				}
			} else if(out == 2) {
				if(iteration < 50 && buff[4] >= buff[3] && buff[4] >= laser.x) {
					if(iteration >= 0) fprintf(outputfp[0], "%e\t%e\n", param, buff[4]);
					iteration++;
				}
				buff[3] = buff[4];
				buff[4] = laser.x;
				if(iteration == 50) break;
			}
		}
		
		end = clock();
		printf("%f\n", (double)(end - start) / CLOCKS_PER_SEC);
		
		if(out == 1) {
			if(para_flag == 2) AutoCorrelation(wfmData[0], plotnum, dh * divi, "auto-correlation.txt");
			break;
		}
	}
	
	for(i = 0; i < 1; i++) free(wfmData[i]);
	free(laser.xdelay);
	
	fclose(outputfp[0]);
	
	return 0;
}

int set_param(struct LaserEOfeedback *laser, int out, int para_flag, double param_max) {
	int i, j, fb_memory;
	
	for(i = 0; i < laser->fbNum; i++) laser->p.weight_fb[i] = 1.0 / (double)(laser->fbNum);
	laser->p.phi *= PI;
	laser->p.tau_L = 1.0 / (2.0 * PI * laser->p.tau_L);
	laser->p.tau_H = 1.0 / (2.0 * PI * laser->p.tau_H);
	laser->p.epsilon = laser->p.tau_L / laser->p.tau_H;
	for(i = 0, laser->long_fb = 0; i < laser->fbNum; i++) {
		laser->descretized_num[i] = (int)(laser->p.delay[i] / h + EPS);
		if(laser->p.delay[i] > laser->p.delay[laser->long_fb]) laser->long_fb = i;
		printf("Delay time = %e (Descretized num = %d)\n", laser->p.delay[i], laser->descretized_num[i]);
	}
	printf("epsilon = %e, beta = %e\n", laser->p.epsilon, laser->p.beta);
	
	if(out == 2 && para_flag == 2 && param_max > laser->p.delay[laser->long_fb]) fb_memory = (int)(param_max / h + EPS);
	else fb_memory = (int)(laser->p.delay[laser->long_fb] / h + EPS);
	
	laser->xdelay = (double*)malloc(sizeof(double) * fb_memory);
	
	return fb_memory;
}

void CalcLaserEOfeedback(struct LaserEOfeedback *laser) {
	int i, j, k;
	double a[2], ini[2], b[2][4], cosTheta, feedback, interval_L = h / laser->p.tau_L, interval_H = h / laser->p.tau_H, buffer;
	
	ini[0] = laser->x;
	ini[1] = laser->y;
	for(i = 0, buffer = 0.0; i < laser->fbNum; i++) buffer += laser->xdelay[laser->fbIndex[i]] * laser->p.weight_fb[i];
	cosTheta = cos(buffer + laser->p.phi);
	feedback = laser->p.beta * cosTheta * cosTheta;
	
	for(i = 0; i < 4; i++) {
		if(i == 0) for(k = 0; k < 2; k++) a[k] = ini[k];
		else if(i == 1) for(k = 0; k < 2; k++) a[k] = ini[k] + b[k][0] / 2.0;
		else if(i == 2) for(k = 0; k < 2; k++) a[k] = ini[k] + b[k][1] / 2.0;
		else if(i == 3) for(k = 0; k < 2; k++) a[k] = ini[k] + b[k][2];
		
		b[0][i] = interval_L * ( - (1.0 + laser->p.epsilon) * a[0] - a[1] + feedback);
		b[1][i] = interval_H * a[0];
	}
	
	for(i = 0; i < 2; i++) a[i] = ini[i] + (b[i][0] + 2.0 * b[i][1] + 2.0 * b[i][2] + b[i][3]) / 6.0;
	
	laser->xdelay[laser->fbIndex[laser->long_fb]] = laser->x;
	for(i = 0; i < laser->fbNum; i++) laser->fbIndex[i] = (laser->fbIndex[i] + 1) % laser->descretized_num[laser->long_fb];
	laser->x = a[0];
	laser->y = a[1];
	
	return;
}

void AutoCorrelation(double *Data, int N, double timeStep, char *filename) {
	double Data1Ave, Data2Ave;
	double Data1Sum, Data2Sum;
	double Data1SquSum, Data2SquSum;
	double ProductSum;
	double Data1Deviation, Data2Deviation;
	double *correlationData;
	int i, j, k, plotLength, wfmLength, averagingPoint;
	FILE *outputfp;
	
	for(i = 0, Data1Sum = Data1SquSum = 0.0; i < N; i++) {
		Data1Sum += Data[i];
		Data1SquSum += Data[i] * Data[i];
	}
	Data1Ave = Data1Sum / N;
	Data1Deviation = sqrt((Data1SquSum - Data1Sum / N * Data1Sum) / N);
	
	if(Data1Deviation < 1e-4 || isnan(Data1Deviation)) {
		fprintf(stderr, "small deviation: %e\n", Data1Deviation);
		return;
	}
	
	wfmLength = (int)(30.0 / timeStep + EPS);
	plotLength = (int)(100.0 / timeStep + EPS);
	correlationData = (double*)malloc(sizeof(double) * plotLength);
	averagingPoint = 100;
	if(N < averagingPoint * wfmLength + plotLength) averagingPoint = (N - plotLength) / wfmLength;
	
	for(i = 0; i < plotLength; i++) correlationData[i] = 0.0;
	for(k = 0; k < averagingPoint; k++) {
		for(i =  k * wfmLength, Data1Sum = Data1SquSum = 0.0; i < (k + 1) * wfmLength; i++) {
			Data1Sum += Data[i];
			Data1SquSum += Data[i] * Data[i];
		}
		Data1Ave = Data1Sum / wfmLength;
		
		for(j = 0; j < plotLength; j++) {
			for(i = k * wfmLength, Data2Sum = 0.0, Data2SquSum = 0.0, ProductSum = 0.0; i < (k + 1) * wfmLength; i++) {
				Data2Sum += Data[i+j];
				Data2SquSum += Data[i+j] * Data[i+j];
				ProductSum += Data[i] * Data[i+j];
			}
			Data2Ave = Data2Sum / wfmLength;
			
			Data1Deviation = sqrt(Data1SquSum - Data1Ave * Data1Sum);
			Data2Deviation = sqrt(Data2SquSum - Data2Ave * Data2Sum);
			correlationData[j] += (ProductSum - Data1Ave * Data2Sum - Data2Ave * Data1Sum + wfmLength * Data1Ave * Data2Ave) / (Data1Deviation * Data2Deviation);
		}
	}
	
	outputfp = fopen(filename, "w");
	fprintf(outputfp, "Time shift [ms]\tAuto-correlation\n");
	for(i = 0; i < plotLength; i++) {
		correlationData[i] /= (double)averagingPoint;
		fprintf(outputfp, "%e\t%e\n", (double)i * timeStep, correlationData[i]);
	}
	fclose(outputfp);
	free(correlationData);
	
	return;
}
