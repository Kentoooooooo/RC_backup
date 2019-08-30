// 4.3 一方向結合されたLang-Kobayashi方程式のカオス同期のパラメータ変化に対する相互相関値（図4.15）

// このプログラムは計算に時間がかかるかもしれない。
// 計算時間が長すぎる場合には、tMax や paraNum を小さい値に設定すると、計算時間を短縮できる。

#include<stdio.h>
#include<math.h>

#define PI 3.141592653589793238
#define C  2.99792458e8

#define M 6 /* number of equations */

// variable parameter values
double r3Dri  = 0.008;  // reflectivity of external mirror of drive
double JRatio = 1.3;    // normalized injection current
double LDri   = 0.6;    // external cavity length (one-way) for drive [m]
double LInj   = 1.2;    // distance between drive and response lasers [m]
double detun  = -4.0e9;	// optical frequency detuning [Hz]

double kapInjRatio = 12.5; // injection strength ratio to drive feedback strength

// fixed parameter values
double r2     = 0.556;	   // reflectivity of internal mirror
double tauIn  = 8.0e-12;   // internal cavity round-trip time [s]
double lambda = 1.537e-6;  // optical wavelength [m]
double Gn     = 8.4e-13;   // gain coefficient
double N0     = 1.4e24;	   // carrier density at transparency
double tauP   = 1.927e-12; // photon lifetime [s]
double tauS   = 2.04e-9;   // carrier lifetime [s]
double alpha  = 3.0;	   // alpha parameter

double Nth, Jth;
double J, kapDri, kapInj, tauDri, tauInj;
double omega0, phaseShiftDri, phaseShiftInj;

#define DELAY_MAX 100000 // maximum array size for delay
double eDelayDri[DELAY_MAX], phiDelayDri[DELAY_MAX];
double eDelayInj[DELAY_MAX], phiDelayInj[DELAY_MAX];
int delayDriNum, delayInjNum;
int delayDriIndex, delayInjIndex;

#define WAVE_MAX 200000 // maximum array size to save waveforms
double wave[3][WAVE_MAX];

// calculation of parameter values
void calcParameter(double h)
{
  Nth = N0 + 1.0 / tauP / Gn; // carrier density at threshold
  Jth = Nth / tauS;           // injection current at threshold
  J   = JRatio * Jth;         // injection current
  
  kapDri = (1 - r2 * r2) * r3Dri / r2 / tauIn; // feedback strength of drive
  kapInj = kapInjRatio * kapDri; // injection strength from drive to response
  
  tauDri = 2.0 * LDri / C; // external-cavity round-trip time for drive
  tauInj =       LInj / C; // coupling time from drive to response
  
  omega0 = 2.0 * PI * C / lambda;                  // optical angular frequency
  phaseShiftDri = fmod(omega0 * tauDri, 2.0 * PI); // initial phase shift for drive
  phaseShiftInj = fmod(omega0 * tauInj, 2.0 * PI); // initial phase shift for coupling
  
  delayDriNum = (int)(tauDri / h);
  delayInjNum = (int)(tauInj / h);
}

void initializeDelay(double a[])
{
  int i;
  delayDriIndex = delayInjIndex = 0;
  
  for(i = 0; i < DELAY_MAX; i++){
    eDelayDri[i]   = a[0];
    phiDelayDri[i] = a[1];
  }
  for(i = 0; i < DELAY_MAX; i++){
    eDelayInj[i]   = 0.0;
    phiDelayInj[i] = 0.0;
  }
}

void laser(double x[], double b[], double theta[])
{
  b[0] = 1.0 / 2.0 * (Gn * (x[2] - N0) - 1.0 / tauP) * x[0]
    + kapDri * eDelayDri[delayDriIndex] * cos(theta[0]);
  b[1] = alpha / 2.0 * (Gn * (x[2] - N0) - 1.0 / tauP)
    - kapDri * eDelayDri[delayDriIndex] / x[0] * sin(theta[0]);
  b[2] = J - x[2] / tauS - Gn * (x[2] - N0) * x[0] * x[0];

  b[3] = 1.0 / 2.0 * (Gn * (x[5] - N0) - 1.0 / tauP) * x[3]
    + kapInj * eDelayInj[delayInjIndex] * cos(theta[1]);
  b[4] = alpha / 2.0 * (Gn * (x[5] - N0) - 1.0 / tauP)
    - kapInj * eDelayInj[delayInjIndex] / x[3] * sin(theta[1]);
  b[5] = J - x[5] / tauS - Gn * (x[5] - N0) * x[3] * x[3];
}

void rungeKutta(double a[], double h, double t){
  int i, j;
  double x[M], b[4][M];

  double theta[2];
  theta[0] = fmod(phaseShiftDri + a[1] - phiDelayDri[delayDriIndex], 2.0 * PI);
  theta[1] = fmod(phaseShiftInj + a[4] - phiDelayInj[delayInjIndex] - 2.0 * PI * detun * t, 2.0 * PI);

  for(i = 0; i < 4; i++){
    for(j = 0; j < M; j++){
      if(i == 0) x[j] = a[j];
      if(i == 1) x[j] = a[j] + h * b[0][j] / 2.0;
      if(i == 2) x[j] = a[j] + h * b[1][j] / 2.0;
      if(i == 3) x[j] = a[j] + h * b[2][j];
    }
    laser(x, b[i], theta);
  }

  for(i = 0; i < M; i++){
    a[i] += h * (b[0][i] + 2.0 * b[1][i] + 2.0 * b[2][i] + b[3][i]) / 6.0;
  }

  // Renew arrays for delay
  eDelayDri[delayDriIndex]   = eDelayInj[delayInjIndex]   = a[0];
  phiDelayDri[delayDriIndex] = phiDelayInj[delayInjIndex] = a[1];

  delayDriIndex = (delayDriIndex + 1) % delayDriNum;
  delayInjIndex = (delayInjIndex + 1) % delayInjNum;
}

double crossCorrelation(double wave1[], double wave2[], int n)
{  
  int i;
  double sum1, sum2;
  double ave1, ave2;
  double dev1, dev2;
  double var1, var2;
  double cov;

  sum1 = sum2 = 0.0;
  for(i = 0; i < n; i++){
    sum1 += wave1[i];
    sum2 += wave2[i];
  }
  ave1 = sum1 / n;
  ave2 = sum2 / n;
  
  var1 = var2 = cov = 0.0;
  for(i = 0; i < n; i++){
    dev1 = wave1[i] - ave1;
    dev2 = wave2[i] - ave2;
    
    var1 += dev1 * dev1;
    var2 += dev2 * dev2;
    cov  += dev1 * dev2;
  }
  var1 /= n;
  var2 /= n;
  cov  /= n;
  if(var1 == 0.0 || var2 == 0.0) return 0.0;
  return cov / (sqrt(var1) * sqrt(var2));
}

int main()
{
  int i, p;
  double a[M], t;
  
  double h         = 5.0e-12;   // calculation step
  double transient = 5000.0e-9; // transient time
  double tMax      = 1000.0e-9; // plot time
  
  int trans = (int)(transient / h);
  int n    = (int)(tMax / h);
  
  // range for parameter change
  double paraMin = 0.0;
  double paraMax = 12.0;
  int paraNum = 100;
  double paraDiv = (paraMax - paraMin) / paraNum;
  
  // Initial conditions
  a[0] = 1.3e10;  // electric field amplitude for drive
  a[1] = 0.0;     // electric field phase for drive
  a[2] = 1.90e24; // carrier density for drive
  a[3] = 1.4e10;  // electric field amplitude for response
  a[4] = 0.0;     // electric field phase for response
  a[5] = 1.85e24; // carrier density for response
  initializeDelay(a);
  
  for(p = 0; p < paraNum; p++){
    kapInjRatio = paraMin + paraDiv * p; // parameter change
    calcParameter(h);
    
    // calculation for transient
    for(i = 0; i < trans; i++){
      t = h * i;
      rungeKutta(a, h, t);
    }
    
    for(i = 0; i < n; i++){
      t = h * (trans + i);
      wave[0][i] = a[0] * a[0]; // Drive intensity
      wave[1][i] = a[3] * a[3]; // Response intensity
      wave[2][i] = eDelayDri[delayDriIndex] * eDelayDri[delayDriIndex]; // Delayed drive intensity
      rungeKutta(a, h, t);
    }
    
    printf("%e\t", kapInj * 1e-9);
    printf("%e\t", crossCorrelation(wave[0], wave[1], n)); // Identical synchronization
    printf("%e\n", crossCorrelation(wave[2], wave[1], n)); // Generalized synchronization
  }
  return 0;
}
