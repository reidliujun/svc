close all;
clear all;
%% frame/segment 250, file size for seg0: lay0-287KB, lay1-2.1MB, lay2-2.2MB
%% calculate the start delay for speed 100KB/s
%%% lay0
t=[3.138 3.1417 3.1415];
t0_100=mean(t);
%z0=[t0 0 0];
%% calculate the start delay for speed 300KB/s, trickle -s -w 2000 -d 300
%%% lay0
t=[1.0197 1.0119 1.0101];
t0_300=mean(t);
%%% lay1
t=[9.073591 9.062651 9.072265];
t1_300=mean(t);
%z1=[t0 t1 0];
%% calculate the start delay for speed 700KB/s, trickle -s -w 2300 -d 700
%%% lay0
t=[0.458759 0.458852 0.458960];
t0_700=mean(t);
%%% lay1
t=[3.708420 3.698856 3.713338];
t1_700=mean(t);
%%% lay2
t=[7.016485 7.018241 7.015308];
t2_700=mean(t);
%z2=[t0 t1 t2];  
%% calculate the start delay for speed 1500KB/s
%%% lay0
t=[0.241548 0.240938 0.236776];
t0_1500 = mean(t);
%%% lay1
t=[1.706405 1.704672 1.697966];
t1_1500 = mean(t);
%%% lay2
t=[3.257236 3.258158 3.251598];
t2_1500 = mean(t);
%% calculate the start delay for speed 3000KB/s
%%% lay0
t=[0.141870 0.142484 0.144386];
t0_3000=mean(t);
%%% lay1
t=[0.877243 0.869578 0.875988];
t1_3000=mean(t);
%%% lay2
t=[1.650764 1.653890 1.661956];
t2_3000=mean(t);
%% plot
z0 = [t0_100 t0_300 t0_700 t0_1500 t0_3000];
z1 = [0 t1_300-t0_300 t1_700-t0_700 t1_1500-t0_1500 t1_3000-t0_3000];
z2 = [0 0 t2_700-t1_700 t2_1500-t1_1500 t2_3000-t1_3000];
x=[100 300 700 1500 3000];
%y=[z0;z1;z2];
y=[z0' z1' z2'];
figure(1);
subplot(1,3,1);
%x=[1 2 3;2 3 4];
bar(x,y,0.6,'stack');
axis([0 3500 0 10]);
title('Starting delay, 250 frames/segment');
legend({'lay0','lay1','lay2'});
xlabel('Bandwidth(KB/s)');
ylabel('Time(s)');
colormap summer;

%% frame/segment 100, file size for seg0: lay0-97KB, lay1-691KB, lay2-708KB

%% calculate the start delay for speed 100KB/s
%%% lay0
t=[1.027351 1.027423 1.027624 1.029192 1.028455];
t0_100=mean(t);
%z0=[t0 0 0];

%% calculate the start delay for speed 300KB/s
%%% lay0
t=[0.356436 0.358263 0.354682 0.355473 0.356191];
t0_300=mean(t);
%%% lay1
t=[3.601180 3.597161 3.606440 3.599316 3.603924];
t1_300=mean(t);
%z1=[t0 t1 0];

%% calculate the start delay for speed 700KB/s
%%% lay0
t=[0.171562 0.175532 0.168378 0.172313 0.172195];
t0_700=mean(t);
%%% lay1
t=[1.211874 1.214337 1.211287 1.212231 1.206961];
t1_700=mean(t);
%%% lay2
t=[2.902550 2.920098 2.925581 2.909894 2.922418];
t2_700=mean(t);
%z2=[t0 t1 t2];     

%% calculate the start delay for speed 1500KB/s
%%% lay0
t=[0.098094 0.098990 0.095001];
t0_1500 = mean(t);
%%% lay1
t=[0.611423 0.612068 0.613164];
t1_1500 = mean(t);
%%% lay2
t=[1.077358 1.080346 1.076284];
t2_1500 = mean(t);
%% calculate the start delay for speed 3000KB/s
%%% lay0
t=[0.063500 0.061264 0.064805];
t0_3000=mean(t);
%%% lay1
t=[0.337197 0.333398 0.338034];
t1_3000=mean(t);
%%% lay2
t=[0.626239 0.571697 0.571509];
t2_3000=mean(t);

z0 = [t0_100 t0_300 t0_700 t0_1500 t0_3000];
z1 = [0 t1_300-t0_300 t1_700-t0_700 t1_1500-t0_1500 t1_3000-t0_3000];
z2 = [0 0 t2_700-t1_700 t2_1500-t1_1500 t2_3000-t1_3000];
x=[100 300 700 1500 3000];
%y=[z0;z1;z2];
y=[z0' z1' z2'];
subplot(1,3,2);
%x=[1 2 3;2 3 4];
bar(x,y,0.6,'stack');
axis([0 3500 0 4]);
title('Starting delay, 100 frames/segment');
legend({'lay0','lay1','lay2'});
xlabel('Bandwidth(KB/s)');
ylabel('Time(s)');
colormap summer;

%% frame/segment 50, file size for seg0: lay0-24KB, lay1-46KB, lay2-112KB

%% calculate the start delay for speed 100KB/s
%%% lay0
t=[0.297192 0.300102 0.296746];
t0_100=mean(t);
%z0=[t0 0 0];

%% calculate the start delay for speed 300KB/s
%%% lay0
t=[0.109485 0.108814 0.108241];
t0_300=mean(t);
%%% lay1
t=[0.308224 0.307669 0.303772];
t1_300=mean(t);
%z1=[t0 t1 0];

%% calculate the start delay for speed 700KB/s
%%% lay0
t=[0.062 0.0335 0.042];
t0_700=mean(t);
%%% lay1
t=[0.151 0.165 0.145];
t1_700=mean(t);
%%% lay2
t=[0.290083 0.298036 0.277902];
t2_700=mean(t);
%z2=[t0 t1 t2];     

z0 = [t0_100 t0_300 t0_700];
z1 = [0 t1_300-t0_300 t1_700-t0_700];
z2 = [0 0 t2_700-t1_700];
x=[100 300 700];
%y=[z0;z1;z2];
y=[z0' z1' z2'];
subplot(1,3,3);
%x=[1 2 3;2 3 4];
bar(x,y,0.13,'stack');
axis([0 800 0 0.6]);
title('Starting delay, 50 frames/segment');
legend({'lay0','lay1','lay2'});
xlabel('Bandwidth(KB/s)');
ylabel('Time(s)');
colormap summer;







