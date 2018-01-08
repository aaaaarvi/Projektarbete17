
clear;

%% Load stuff

% Load event data
load('../../mat/dataPat.mat');

% Import tube position data
tubeData = csvread('../../mat/tubeData.csv');

%% Plot tube hits

% Load weights
load('../../mat/weights3.mat');

% Choose subset (train or test data)
idx = idx_test;

% Threshold for classifying hits
threshold = 0.99;

% Dropout parameter
pkeep = 1;

% Activation functions
sigma1  = @relu;
sigma2  = @relu;
sigma3  = @relu;
sigma4  = @relu;
sigmay  = @sigmoid;

% Transform data (not currently relevant)
T = Tstt;

% Event index
k = randsample(idx, 1);
while sum(A(k, :)) == 0
    k = randsample(idx, 1);
end
%k = 48975;
%k = 69018;
%k = 106120;
%k = 116098;
%k = 66321;
%k = 232050;
%k = 2808;
%k = 151663;
%k = 2359;
%k = 625;
%k = 4591;
%k = 1144;
%k = 7035;
%k = 4749;
%k = 5722;

%k = 5433;
%k = 1792;
%k = 4451;
%k = 8899;
%k = 9877;
%k = 5129;
%k = 7812;
%k = 6171;

% Feed forward
X = T(k, :)';
Z1tilde = (W1*X + B1)*pkeep;
Z1 = sigma1(Z1tilde);
Z2tilde = (W2*Z1 + B2)*pkeep;
Z2 = sigma2(Z2tilde);
Z3tilde = (W3*Z2 + B3)*pkeep;
Z3 = sigma3(Z3tilde);
Z4tilde = (W4*Z3 + B4)*pkeep;
Z4 = sigma4(Z4tilde);
Yp = Wy*Z4 + By;
Yh = sigmay(Yp);
Y = A(k, :)';

% Thresholded values
Yh_th = zeros(size(Yh));
Yh_th(Yh > threshold & X == 1) = 1;

% Compute prediction accuracy
pred_acc = 100*(sum(Yh_th == Y & X == 1)/sum(X));
efficiency = 100*(sum(Yh_th == Y & Y == 1)/sum(Y));
purity = 100*(sum(Yh_th == Y & Y == 1)/sum(Yh_th));

% Plot the input data
figure('pos', [10, 500, 1200, 480]);
subplot(1, 2, 1);
hold on;
viscircles([0, 0], 41.5, 'Color', 'k');
x = 15;
y1 = 8.75;
y2 = 17.5;
plot([0, x], [y2, y1], '-k', 'LineWidth', 2);
plot([x, x], [y1, -y1], '-k', 'LineWidth', 2);
plot([x, 0], [-y1, -y2], '-k', 'LineWidth', 2);
plot([0, -x], [-y2, -y1], '-k', 'LineWidth', 2);
plot([-x, -x], [-y1, y1], '-k', 'LineWidth', 2);
plot([-x, 0], [y1, y2], '-k', 'LineWidth', 2);
for i = 1:NtubesSTT
    
    % Outline
    x = tubeData(i, 2);
    y = tubeData(i, 3);
    r = 0.5;
    c1 = 0.5;
    c2 = 0.5;
    c3 = 0.5;
    %plot(x, y, 'LineWidth', 1, 'Marker', 'o', 'Color', [c1, c2, c3]);
    
    % Input data
    if X(i) == 1
        x = tubeData(i, 2);
        y = tubeData(i, 3);
        r = 0.5;
        c1 = 0;
        c2 = 0;
        c3 = 0;
        plot(x, y, 'LineWidth', 1, 'Marker', 'o', 'Color', [c1, c2, c3]);
    end
    
    % Correct proton track
    if Y(i) == 1
        x = tubeData(i, 2);
        y = tubeData(i, 3);
        r = 0.5;
        c1 = 1;
        c2 = 0;
        c3 = 0;
        plot(x, y, 'LineWidth', 1, 'Marker', 'o', 'Color', [c1, c2, c3]);
    end
end
xlim([-43, 43]);
ylim([-43, 43]);

% Plot the output from the NN
subplot(1, 2, 2);
hold on;
for i = 1:NtubesSTT
    if X(i) == 1
        x = tubeData(i, 2);
        y = tubeData(i, 3);
        r = 0.5;
        c1 = 1 - Yh_th(i);
        c2 = 1 - Yh_th(i);
        c3 = 1 - Yh_th(i);
        plot(x, y, 'LineWidth', 1, 'Marker', 'o', 'Color', [c1, c2, c3]);
    end
end
xlim([-43, 43]);
ylim([-43, 43]);


%% Plot momentum vector

% Load weights
load('../../mat/weights1c.mat', 'pkeep', ...
    'W1', 'W2', 'W3', 'W4', 'W5', 'W6', 'Wy', ...
    'B1', 'B2', 'B3', 'B4', 'B5', 'B6', 'By');

% Load event data
clear T Tstt A;
load('../../mat/dataPatMom.mat');

% Activation functions
sigma1  = @relu;
sigma2  = @relu;
sigma3  = @relu;
sigma4  = @relu;
sigma5  = @relu;
sigma6  = @relu;
sigmay  = @lin;

% Transform data (not currently relevant)
T = Tstt;

% Feed forward
%X = T(k, :)';
X = Yh_th;
Z1tilde = (W1*X + B1)*pkeep;
Z1 = sigma1(Z1tilde);
Z2tilde = (W2*Z1 + B2)*pkeep;
Z2 = sigma2(Z2tilde);
Z3tilde = (W3*Z2 + B3)*pkeep;
Z3 = sigma3(Z3tilde);
Z4tilde = (W4*Z3 + B4)*pkeep;
Z4 = sigma4(Z4tilde);
Z5tilde = (W5*Z4 + B5);
Z5 = sigma5(Z5tilde);
Z6tilde = (W6*Z5 + B6);
Z6 = sigma6(Z6tilde);
Yp = Wy*Z6 + By;
Yh = sigmay(Yp);
Y = A(k, :)';

% Compute prediction error
[theta1, rho1] = cart2pol(Y(1), Y(2));
[theta2, rho2] = cart2pol(Yh(1), Yh(2));
theta1 = theta1*180/pi;
theta2 = theta2*180/pi;
theta_diff = theta2 - theta1;
rho_error = 100*abs(rho2 - rho1)/rho1;
theta_error = min([abs(theta_diff), abs(theta_diff + 360), abs(theta_diff - 360)]);

% Plot
subplot(1, 2, 2);
hold on;
viscircles([0, 0], 41.5, 'Color', 'k');
x = 15;
y1 = 8.75;
y2 = 17.5;
plot([0, x], [y2, y1], '-k', 'LineWidth', 2);
plot([x, x], [y1, -y1], '-k', 'LineWidth', 2);
plot([x, 0], [-y1, -y2], '-k', 'LineWidth', 2);
plot([0, -x], [-y2, -y1], '-k', 'LineWidth', 2);
plot([-x, -x], [-y1, y1], '-k', 'LineWidth', 2);
plot([-x, 0], [y1, y2], '-k', 'LineWidth', 2);

scale = 50;
quiver(0, 0, scale*Y(1), scale*Y(2), ':b', 'LineWidth', 2, 'MaxHeadSize', 1);
quiver(0, 0, scale*Yh(1), scale*Yh(2), '-b', 'LineWidth', 2, 'MaxHeadSize', 1);
%legend(['acc = ' num2str(pred_acc) ' %'], ...
%    ['eff = ' num2str(efficiency) ' %'], ...
%    ['pur = ' num2str(purity) ' %'], ...
%    ['\delta\rho = ' num2str(rho_error) ' %'], ...
%    ['\delta\theta = ' num2str(theta_error) '\circ']);















