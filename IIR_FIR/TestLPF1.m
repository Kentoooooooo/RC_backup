clear all; close all;

N = 100;
omega_c = pi/4;

n = 1 : 2*N+1;

h_c(n) = (1 ./ (pi .* (n-(N+1)))) .* sin(omega_c .* (n-(N+1)));

h_c(N+1) = omega_c / pi;
T_c = fft(h_c);
A_c = abs(T_c);
P_c = unwrap(atan2(imag(T_c) , real(T_c)));

figure; axis on;
subplot(2,1,1)

plot(A_c);
axis([0 N 0 1.5]);
title('Amplitude');

subplot(2,1,2); plot(P_c); axis([0 N -80 2]); title('Phase');

w(n) = 0.54 + 0.46 .* cos((n-(N+1)).*pi./N);
h_cw = w.*h_c;
T_cw = fft(h_cw);
A_cw = abs(T_cw);
P_cw = unwrap(atan2(imag(T_cw) , real(T_cw)));

figure; axis on; 
subplot(2,1,1); plot(A_cw); axis([0 N 0 1.5]); title('Amplitude');
subplot(2,1,2); plot(P_cw); axis([0 N -90 2]); title('Phase');
