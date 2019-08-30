ensyuu11

P = 4; %1‚©‚ç10‚Ü‚Å
A =  1.0 : -0.1 : 1.0 - 0.1.*P;
%x(n) = 1.0 .*sin(pi/6 .* n) + 0.5 .* sin(pi/2 .*n) + 0.5 .* sin(pi/4 .*n);
x(n) = 1.0 .*sin(pi/6 .* n);
for i=2:P
    x(n) =  x(n) + A(i) .*sin(pi / i .* n);
end
y = filter(x, 1, h_c);

M_x = length(x); M_y = length(y);
T_x = fft(x, 2*M_x);
A_x = abs(T_x);
P_x = unwrap(atan2(imag(T_x) , real(T_x)));
T_y = fft(y, 2*M_y);
A_y = abs(T_y);
P_y = unwrap(atan2(imag(T_y) , real(T_y)));

yf = ifft(fft(x, 2*M_x) .* fft(h_c, 2*M_x));

yf  = real(yf);

figure; axis on;
subplot(2,1,1); plot(A_x); axis([0 2*N 0 100]); xlabel('Normalized Frequency(xƒÎ rad/sample)'); ylabel('Magnitude(dB)');title('Amplitude(INPUT)');
subplot(2,1,2); plot(A_y); axis([0 2*N 0 100]); xlabel('Normalized Frequency(xƒÎ rad/sample)'); ylabel('Magnitude(dB)');title('Amplitude(OUTPUT)');
figure; axis on;
subplot(2,1,1); plot(x); axis([0 2*N -2 2]); xlabel('Time(s)'); ylabel('Magnitude(dB)');title('Time-Domain Wave(INPUT)');
subplot(2,1,2); plot(y); axis([0 2*N -2 2]); xlabel('Time(s)'); ylabel('Magnitude(dB)');title('Time-Domain Wave(OUTPUT)');
%figure; axis on; 
%subplot(2,1,1); plot(x); axis([0 2*N -2 2]); title('Time-Domain Wave(INPUT)');
%subplot(2,1,2); plot(yf); axis([0 2*N -2 2]); title('Time-Domain Wave(OUTPUT)');
