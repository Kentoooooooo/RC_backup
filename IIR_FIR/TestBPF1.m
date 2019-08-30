% TestLPF1;

omega_0 = pi / 2;

h_lp = h_cw;

h_bp(n) = real(h_lp(n) .* exp(j .*omega_0 .* (n-1)));

h_bp = 2 .* h_bp;

T_bp = fft(h_bp);
A_bp = abs(T_bp);
P_bp = unwrap(atan2(imag(T_bp) , real(T_bp)));

figure; axis on;
subplot(2,1,1); plot(A_bp); axis([0 N 0 1.5]); title('Amplitude');
subplot(2,1,2); plot(P_bp); axis([0 N -170 20]); title('Phase');
