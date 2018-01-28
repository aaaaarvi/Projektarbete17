
clear;

%% Load stuff

% Load weights
load('../../mat/weights3.mat');

% Load event data
load('../../mat/dataPat.mat');

%% Plot stuff

% Choose subset (train or test data)
idx = idx_test;

% Activation functions
sigma1  = @relu;
sigma1g = @relu_grad;
sigma2  = @relu;
sigma2g = @relu_grad;
sigma3  = @relu;
sigma3g = @relu_grad;
sigma4  = @relu;
sigma4g = @relu_grad;
sigmay  = @sigmoid;
sigmayg = @sigmoid_grad2;

% Transform data (not currently relevant)
T = Tstt;

%
% Loop over threshold values
thresholds = 0.99;
tic
for i=1:10
for k = idx
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
    Yh_th = zeros(size(Yh));
    Yh_th(Yh > thresholds & X == 1) = 1;
end
end
toc
% Adjust for data points that are missing X, Y or Yh vector








