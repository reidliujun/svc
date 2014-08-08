clear all;
close all;
%%%% frame/segment 100, file size for seg0: lay0-97KB, lay1-691KB, lay2-708KB
%%% fps is 25, 4 seconds per segment
l1=[1 1 1 1 1];
l2=[1 1 1 1 1];
threshold1=[208.604 208.604 208.604 208.604 208.604];
threshold2=[596.307 596.307 596.307 596.307 596.307];
time_slot = 0:4:16;

%% calculate the selected layer for speed 200KB/s
bd_200 = [200 204.499 349.135 106.823 267.960];%
layer_200=[1 1 2 1 2];%0:1 16:2 32:3
z_200=[l1' (layer_200-l1)']
%% calculate the selected layer for speed 300KB/s
bd_300=[300 301.092 153.704 332.857 250.057];
layer_300=[2 2 1 2 2];
z_300=[l1' (layer_300-l1)'];
%% calculate the selected layer for speed 400KB/s
bd_400=[400 318.211 255.856 314.513 410.008];
layer_400=[2 2 2 2 2];
z_400=[l1' (layer_400-l1)'];
%% calculate the selected layer for speed 600KB/s
bd_600=[600 594.819 588.823 537.340 604.023];
layer_600=[3 2 2 2 3];
z_600=[l1' l2' (layer_600-l1-l2)'];
%% calculate the selected layer for speed 700KB/s
bd_700=[700 716.516 547.513 678.9224 543.011];
layer_700=[3 3 2 3 2];  
z_700=[l1' l2' (layer_700-l1-l2)'];
%% calculate the selected layer for speed 800KB/s
bd_800=[800 821.573 739.973 603.314 792.06];
layer_800=[3 3 3 3 3];
z_800=[l1' l2' (layer_800-l1-l2)'];
%% plot figure1
figure(1);
subplot(1,3,1);
% set(subplot(1,3,1), 'Position', [0.08, 0.1, 0.17, 0.8]);
stackedbar = @(x, A) bar(x, A, 0.3,'stack');
prettyline = @(x, y) plot(x, y, 'k-o', 'LineWidth', 1);
[hAxes,hBar,hLine] = plotyy(time_slot,z_200,time_slot,bd_200,stackedbar,prettyline);
set(hAxes,'XTick',time_slot); % Change x-axis ticks
set(hAxes,'XTickLabel',time_slot); % Change x-axis ticks labels to desired values.
set(hAxes,'NextPlot','add');
plot(hAxes(2),time_slot,threshold1,'b-.o');
axis(hAxes(2),[-2 18 0 500]);
axis(hAxes(1),[-2 18 0 5]);
colormap summer;
title('Layer selection, network speed 200KB/s');
xlabel('Time (s)');
ylabel(hAxes(1),'Layer ID');
ylabel(hAxes(2), 'Bandwidth (KB/s)');
legend(hAxes(1),'layer1','layer2');
legend(hAxes(2),'Estimate Bandwidth','Switch threshold');
% leg1=legend(hAxes(1),'layer1','layer2');
% leg2=legend(hAxes(2),'Bandwidth');
% set(leg1,'FontSize',10);
% set(leg2,'FontSize',10);

subplot(1,3,2);
% set(subplot(1,3,2), 'Position', [0.42, 0.1, 0.17, 0.8]);
stackedbar = @(x, A) bar(x, A, 0.3,'stack');
prettyline = @(x, y) plot(x, y, 'k-o', 'LineWidth', 1);
[hAxes,hBar,hLine] = plotyy(time_slot,z_300,time_slot,bd_300,stackedbar,prettyline);
set(hAxes,'XTick',time_slot); % Change x-axis ticks
set(hAxes,'XTickLabel',time_slot); % Change x-axis ticks labels to desired values.
set(hAxes,'NextPlot','add');
plot(hAxes(2),time_slot,threshold1,'b-.o');
axis(hAxes(2),[-2 18 0 500]);
axis(hAxes(1),[-2 18 0 5]);
colormap summer;
title('Layer selection, network speed 300KB/s');
xlabel('Time (s)');
ylabel(hAxes(1),'Layer ID');
ylabel(hAxes(2), 'Bandwidth (KB/s)');
legend(hAxes(1),'layer1','layer2');
legend(hAxes(2),'Estimate Bandwidth','Switch threshold');
subplot(1,3,3);
% set(subplot(1,3,3), 'Position', [0.74, 0.1, 0.17, 0.8]);
stackedbar = @(x, A) bar(x, A, 0.3,'stack');
prettyline = @(x, y) plot(x, y, 'k-o', 'LineWidth', 1);
[hAxes,hBar,hLine] = plotyy(time_slot,z_400,time_slot,bd_400,stackedbar,prettyline);
set(hAxes,'XTick',time_slot); % Change x-axis ticks
set(hAxes,'XTickLabel',time_slot); % Change x-axis ticks labels to desired values.
set(hAxes,'NextPlot','add');
plot(hAxes(2),time_slot,threshold1,'b-.o');
axis(hAxes(2),[-2 18 0 500]);
axis(hAxes(1),[-2 18 0 5]);
colormap summer;
title('Layer selection, network speed 400KB/s');
xlabel('Time (s)');
ylabel(hAxes(1),'Layer ID');
ylabel(hAxes(2), 'Bandwidth (KB/s)');
legend(hAxes(1),'layer1','layer2');
legend(hAxes(2),'Estimate Bandwidth','Switch threshold');
% set(hLine,'LineWidth',2,'Color',[0,0.7,0.7],'Marker','o');
% bar(time_slot,z_200,0.2,'stack');
% axis
% hold on;
%bar([0 4 8 12 16],z_300,0.1,'stack');
%% plot figure2
figure(2);
subplot(1,3,1);
stackedbar = @(x, A) bar(x, A, 0.3,'stack');
prettyline = @(x, y) plot(x, y, 'k-o', 'LineWidth', 1);
[hAxes,hBar,hLine] = plotyy(time_slot,z_600,time_slot,bd_600,stackedbar,prettyline);
set(hAxes,'XTick',time_slot); % Change x-axis ticks
set(hAxes,'XTickLabel',time_slot); % Change x-axis ticks labels to desired values.
set(hAxes,'NextPlot','add');
plot(hAxes(2),time_slot,threshold2,'b-.o');
axis(hAxes(2),[-2 18 0 1100]);
axis(hAxes(1),[-2 18 0 6]);
colormap summer;
title('Layer selection, network speed 600KB/s');
xlabel('Time (s)');
ylabel(hAxes(1),'Layer ID');
ylabel(hAxes(2), 'Bandwidth (KB/s)');
legend(hAxes(1),'layer1','layer2','layer3');
legend(hAxes(2),'Estimate Bandwidth','Switch threshold');

subplot(1,3,2);
stackedbar = @(x, A) bar(x, A, 0.3,'stack');
prettyline = @(x, y) plot(x, y, 'k-o', 'LineWidth', 1);
[hAxes,hBar,hLine] = plotyy(time_slot,z_700,time_slot,bd_700,stackedbar,prettyline);
set(hAxes,'XTick',time_slot); % Change x-axis ticks
set(hAxes,'XTickLabel',time_slot); % Change x-axis ticks labels to desired values.
set(hAxes,'NextPlot','add');
plot(hAxes(2),time_slot,threshold2,'b-.o');
axis(hAxes(2),[-2 18 0 1100]);
axis(hAxes(1),[-2 18 0 6]);
colormap summer;
title('Layer selection, network speed 700KB/s');
xlabel('Time (s)');
ylabel(hAxes(1),'Layer ID');
ylabel(hAxes(2), 'Bandwidth (KB/s)');
legend(hAxes(1),'layer1','layer2','layer3');
legend(hAxes(2),'Estimate Bandwidth','Switch threshold');

subplot(1,3,3);
stackedbar = @(x, A) bar(x, A, 0.3,'stack');
prettyline = @(x, y) plot(x, y, 'k-o', 'LineWidth', 1);
[hAxes,hBar,hLine] = plotyy(time_slot,z_800,time_slot,bd_800,stackedbar,prettyline);
set(hAxes,'XTick',time_slot); % Change x-axis ticks
set(hAxes,'XTickLabel',time_slot); % Change x-axis ticks labels to desired values.
set(hAxes,'NextPlot','add');
plot(hAxes(2),time_slot,threshold2,'b-.o');
axis(hAxes(2),[-2 18 0 1100]);
axis(hAxes(1),[-2 18 0 6]);
colormap summer;
title('Layer selection, network speed 800KB/s');
xlabel('Time (s)');
ylabel(hAxes(1),'Layer ID');
ylabel(hAxes(2), 'Bandwidth (KB/s)');
legend(hAxes(1),'layer1','layer2','layer3');
legend(hAxes(2),'Estimate Bandwidth','Switch threshold');
%% plot figure3
%Calculate the happiness level of algorithm1
happy1 = happyDegree(layer_600)

figure(3);
subplot(1,2,1);
stackedbar = @(x, A) bar(x, A, 0.3,'stack');
prettyline = @(x, y) plot(x, y, 'k-o', 'LineWidth', 1);
[hAxes,hBar,hLine] = plotyy(time_slot,z_600,time_slot,bd_600,stackedbar,prettyline);
set(hAxes,'XTick',time_slot); % Change x-axis ticks
set(hAxes,'XTickLabel',time_slot); % Change x-axis ticks labels to desired values.
set(hAxes,'NextPlot','add');
plot(hAxes(2),time_slot,threshold2,'b-.o');
axis(hAxes(2),[-2 18 0 1100]);
axis(hAxes(1),[-2 18 0 6]);
colormap summer;
title('Layer selection, network speed 600KB/s, algorithm1');
xlabel('Time (s)');
ylabel(hAxes(1),'Layer ID');
ylabel(hAxes(2), 'Bandwidth (KB/s)');
legend(hAxes(1),'layer1','layer2','layer3');
legend(hAxes(2),'Estimate Bandwidth','Switch threshold');



% calculate the selected layer for speed 600KB/s
bd_600=[600 594.819 588.823 537.340 604.023];
layer_600=[3 3 3 2 2];
z_600=[l1' l2' (layer_600-l1-l2)'];


%Calculate the happiness level of algorithm2
happy2 = happyDegree(layer_600)


subplot(1,2,2);
stackedbar = @(x, A) bar(x, A, 0.3,'stack');
prettyline = @(x, y) plot(x, y, 'k-o', 'LineWidth', 1);
[hAxes,hBar,hLine] = plotyy(time_slot,z_600,time_slot,bd_600,stackedbar,prettyline);
set(hAxes,'XTick',time_slot); % Change x-axis ticks
set(hAxes,'XTickLabel',time_slot); % Change x-axis ticks labels to desired values.
set(hAxes,'NextPlot','add');
plot(hAxes(2),time_slot,threshold2,'b-.o');
axis(hAxes(2),[-2 18 0 1100]);
axis(hAxes(1),[-2 18 0 6]);
colormap summer;
title('Layer selection, network speed 600KB/s, algorithm2');
xlabel('Time (s)');
ylabel(hAxes(1),'Layer ID');
ylabel(hAxes(2), 'Bandwidth (KB/s)');
legend(hAxes(1),'layer1','layer2','layer3');
legend(hAxes(2),'Estimate Bandwidth','Switch threshold');