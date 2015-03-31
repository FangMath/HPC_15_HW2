close all; clear all;
x = 2.^[0:11]*1000;
y = [0.012, 0.016, 0.008, 0.011, 0.022, 0.042, 0.059, 0.011, 0.018, 0.382, 0.76, 1.54];

bar(log(x/1000)/log(2),y);
xlabel('N = 2^k*1000');
ylabel('timings (secends)');
title('timings with different N');
