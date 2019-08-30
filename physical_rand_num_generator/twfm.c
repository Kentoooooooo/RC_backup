// 3.1 Lang-Kobayashiï˚íˆéÆÇÃéûä‘îgå`Åiê}3.6Åj

#include<stdio.h>
#include<math.h>

#define PI 3.141592653589793238
#define C  2.99792458e8

#define M 3 /* number of equations */

// variable parameter values
double r3     = 0.01;  // reflectivity of external mirror
double JRatio = 1.11;  // normalized injection current
double L      = 0.225; // external cavity length (one-way) [m]

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
double J, kap, tau;
double detun, omega0, phaseShift;

#define DELAY_MAX 100000 // maximum array size for delay
double eDelay[DELAY_MAX], phiDelay[DELAY_MAX];
int delayNum, delayIndex;

// calculation of parameter values
void calcParameter(double h)
{
  Nth = N0 + 1.0 / tauP / Gn; // carrier density at threshold
  Jth = Nth / tauS;           // injection current at threshold
  J   = JRatio * Jth;         // injection current

  kap = (1 - r2 * r2) * r3 / r2 / tauIn; // coupling strength
  tau = 2.0 * L / C; // external-cavity round-trip time for drive
  
  omega0 = 2.0 * PI * C / lambda;            // optical angular frequency
  phaseShift = fmod(omega0 * tau, 2.0 * PI); // initial phase shift
  
  delayNum = (int)(tau / h);
}

void initializeDelay(double a[])
{
  int i;
  delayIndex = 0;
  for(i = 0; i < DELAY_MAX; i++){
    eDelay[i]   = a[0];
    phiDelay[i] = a[1];
  }
}

void laser(double x[], double b[], double theta)
{
  b[0] = 1.0 / 2.0 * (Gn * (x[2] - N0) - 1.0 / tauP) * x[0]
         + kap * eDelay[delayIndex] * cos(theta);
  b[1] = alpha / 2.0 * (Gn * (x[2] - N0) - 1.0 / tauP)
         - kap * eDelay[delayIndex] / x[0] * sin(theta);
  b[2] = J - x[2] / tauS - Gn * (x[2] - N0) * x[0] * x[0];
}

void rungeKutta(double a[], double h, double t){
  int i, j;
  double x[M], b[4][M];
  
  double theta = fmod(phaseShift + a[1] - phiDelay[delayIndex], 2.0 * PI);
  
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
  eDelay[delayIndex]   = a[0];
  phiDelay[delayIndex] = a[1];
  
  delayIndex = (delayIndex + 1) % delayNum; 
}

int main()
{
  int i;
  double a[M], t;
  
  double h         = 5.0e-12;   // calculation step
  double transient = 5000.0e-9; // transient time
  double tMax      = 10.0e-9;   // plot time
  double sInt      = 800.0e-12;   // sampling interval
  double threshold = 2.0;
  
  int div = 1; // plot interval
  
  int trans = (int)(transient / h);
  int n    = (int)(tMax / h);
  int sample = (int)(sInt / h);
  
  // Initial conditions
  a[0] = 1.3e10;  // electric field amplitude
  a[1] = 0.0;     // electric field phase
  a[2] = 1.90e24; // carrier density
  initializeDelay(a);
  
  calcParameter(h);
  
  // calculation for transient
  for(i = 0; i < trans; i++){
    t = h * i;
    rungeKutta(a, h, t);
  }
  
  for(i = 0; i < n; i++){
    t = h * (trans + i);
    if(i % div == 0){
      printf("%e\t", h * i * 1e9);         // Time [ns]
      if( i % sample == 0) {
	      printf("%e\t%e\n", a[0] * a[0] * 1e-20, a[0] * a[0] * 1e-20);
      }else{
	      printf("%e\n", a[0] * a[0] * 1e-20);
      }
//      printf("%e\n", a[0] * a[0] * 1e-20); // Drive Intensity
    }
    rungeKutta(a, h, t);
  }
  return 0;
}
