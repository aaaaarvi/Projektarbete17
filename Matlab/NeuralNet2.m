clear;

%% INITIALIZATION

% Load data
load('../../mat/dataTS.mat');

% Number of training and testing points (images)
Ntrain = 1000000;
Ntest = 2000;

% Choose number of PCA components
NcompSTT = NtubesSTT;%1000;
NcompFTS = NtubesFTS;%2000;

% Load and save flags
load_flag = 0;
save_flag = 1;

% Learning rate
gamma_min = 0.001;
gamma_max = 0.001;

% Minimum momentum difference
minDiff1 = 100; % percent
minDiff2 = 1e-2; % absolute

% Dropout parameter
pkeep = 1;

% Epoch size
epochSize = 100;
Nep = Ntrain/epochSize; % Nr of epochs

% Number of neurons
n1 = NcompSTT;   % Number of input neurons (STT data)
n2 = NcompFTS;   % Number of input neurons (FTS data)
s1_1 = 200;      % 1:st hidden layer (STT data)
s1_2 = 200;      % 1:st hidden layer (FTS data)
s2_1 = 100;      % 2:nd hidden layer (STT data)
s2_2 = 100;      % 2:nd hidden layer (FTS data)
s3 = 100;        % 3:rd hidden layer
s4 = 60;         % 4:th hidden layer
s5 = 30;         % 5:th hidden layer
m = 8;           % Number of output neurons

% Activation functions
sigma1_1  = @relu;
sigma1_1g = @relu_grad;
sigma1_2  = @relu;
sigma1_2g = @relu_grad;
sigma2_1  = @relu;
sigma2_1g = @relu_grad;
sigma2_2  = @relu;
sigma2_2g = @relu_grad;
sigma3  = @relu;
sigma3g = @relu_grad;
sigma4  = @relu;
sigma4g = @relu_grad;
sigma5  = @relu;
sigma5g = @relu_grad;
sigmay  = @lin;
sigmayg = @lin_grad;

% Loss function
loss  = @quadraticLoss;
lossg = @quadraticLoss_grad;

% Standard deviation for the initial random weights
st_dev = 0.055;

% Transform data
%Tstt = Tstt*coeffSTT(:, 1:NcompSTT);
%Tfts = Tfts*coeffFTS(:, 1:NcompFTS);
T = [Tstt, Tfts];

% Divide into training and testing indices
Ntest = min(Npoints/2, Ntest);
%idx_test = randsample(Npoints, Ntest)';
%idx_train = setdiff(1:Npoints, idx_test);
% OR
idx_keep = find(sum(T, 2) ~= 0)';
Npoints = length(idx_keep);
idx_test = randsample(idx_keep, Ntest);
idx_train = setdiff(idx_keep, idx_test);

% Initial weights and biases
W1_1 = st_dev*randn(s1_1, n1);   % Weights to 1:st hidden layer (STT data)
W1_2 = st_dev*randn(s1_2, n2);   % Weights to 1:st hidden layer (FTS data)
W2_1 = st_dev*randn(s2_1, s1_1); % Weights to 2:nd hidden layer (STT data)
W2_2 = st_dev*randn(s2_2, s1_2); % Weights to 2:nd hidden layer (FTS data)
W3_1 = st_dev*randn(s3, s2_1);   % Weights to 3:rd hidden layer (STT data)
W3_2 = st_dev*randn(s3, s2_2);   % Weights to 3:rd hidden layer (FTS data)
W4 = st_dev*randn(s4, s3);       % Weights to 4:th hidden layer
W5 = st_dev*randn(s5, s4);       % Weights to 4:th hidden layer
Wy = st_dev*randn(m, s5);        % Weights to output layer
B1_1 = st_dev*ones(s1_1, 1);     % Biases to 1:st hidden layer (STT data)
B1_2 = st_dev*ones(s1_2, 1);     % Biases to 1:st hidden layer (FTS data)
B2_1 = st_dev*ones(s2_1, 1);     % Biases to 2:nd hidden layer (STT data)
B2_2 = st_dev*ones(s2_2, 1);     % Biases to 2:nd hidden layer (FTS data)
B3 = st_dev*ones(s3, 1);         % Biases to 3:rd hidden layer
B4 = st_dev*ones(s4, 1);         % Biases to 4:th hidden layer
B5 = st_dev*ones(s5, 1);         % Biases to 4:th hidden layer
By = st_dev*ones(m, 1);          % Biases to output layer

% Parameters for the Adam Optimizer
beta1 = 0.9;
beta2 = 0.999;
epsilon = 1e-8;
mW1_1 = zeros(s1_1, n1);    vW1_1 = zeros(s1_1, n1);
mW1_2 = zeros(s1_2, n2);    vW1_2 = zeros(s1_2, n2);
mW2_1 = zeros(s2_1, s1_1);  vW2_1 = zeros(s2_1, s1_1);
mW2_2 = zeros(s2_2, s1_2);  vW2_2 = zeros(s2_2, s1_2);
mW3_1 = zeros(s3, s2_1);    vW3_1 = zeros(s3, s2_1);
mW3_2 = zeros(s3, s2_2);    vW3_2 = zeros(s3, s2_2);
mW4 = zeros(s4, s3);        vW4 = zeros(s4, s3);
mW5 = zeros(s5, s4);        vW5 = zeros(s5, s4);
mWy = zeros(m, s5);         vWy = zeros(m, s5);
mB1_1 = zeros(s1_1, 1);     vB1_1 = zeros(s1_1, 1);
mB1_2 = zeros(s1_2, 1);     vB1_2 = zeros(s1_2, 1);
mB2_1 = zeros(s2_1, 1);     vB2_1 = zeros(s2_1, 1);
mB2_2 = zeros(s2_2, 1);     vB2_2 = zeros(s2_2, 1);
mB3 = zeros(s3, 1);         vB3 = zeros(s3, 1);
mB4 = zeros(s4, 1);         vB4 = zeros(s4, 1);
mB5 = zeros(s5, 1);         vB5 = zeros(s5, 1);
mBy = zeros(m, 1);          vBy = zeros(m, 1);


%% TRAINING

% Train the network
C_train = zeros(Nep, 1);
C_test = zeros(Nep, 1);
predAcc_test = zeros(Nep, 1);
predAcc_train = zeros(Nep, 1);
predAccMax = 0;
if load_flag == 1
    load('../../mat/weights.mat', ...
        'W1_1', 'W1_2', 'W2_1', 'W2_2', 'W3_1', 'W3_2', 'W4', 'W5', 'Wy', ...
        'B1_1', 'B1_2', 'B2_1', 'B2_2', 'B3', 'B4', 'B5', 'By', ...
        'predAccMax', 'idx_train', 'idx_test');
end

% Loop through each epoch
figure;
h = waitbar(0, 'Training the neurual network...');
for ep = 1:Nep
    
    % Initialize the weight and bias changes
    dW1_1 = zeros(s1_1, n1);
    dW1_2 = zeros(s1_2, n2);
    dW2_1 = zeros(s2_1, s1_1);
    dW2_2 = zeros(s2_2, s1_2);
    dW3_1 = zeros(s3, s2_1);
    dW3_2 = zeros(s3, s2_2);
    dW4 = zeros(s4, s3);
    dW5 = zeros(s5, s4);
    dWy = zeros(m, s5);
    dB1_1 = zeros(s1_1, 1);
    dB1_2 = zeros(s1_2, 1);
    dB2_1 = zeros(s2_1, 1);
    dB2_2 = zeros(s2_2, 1);
    dB3 = zeros(s3, 1);
    dB4 = zeros(s4, 1);
    dB5 = zeros(s5, 1);
    dBy = zeros(m, 1);
    
    % Loop through each image in the epoch
    im_train = randsample(idx_train, epochSize);
    for ex = 1:epochSize
        im = im_train(ex);
        
        % Dropout vectors
        doZ1_1 = 1*(rand(s1_1, 1) < pkeep);
        doZ1_2 = 1*(rand(s1_2, 1) < pkeep);
        doZ2_1 = 1*(rand(s2_1, 1) < pkeep);
        doZ2_2 = 1*(rand(s2_2, 1) < pkeep);
        doZ3 = 1*(rand(s3, 1) < pkeep);
        doZ4 = 1*(rand(s4, 1) < pkeep);
        doZ5 = 1*(rand(s5, 1) < pkeep);
        
        % Forward propagation (with dropout)
        X_1 = Tstt(im, :)';
        X_2 = Tfts(im, :)';
        Z1tilde_1 = (W1_1*X_1 + B1_1).*doZ1_1;
        Z1_1 = sigma1_1(Z1tilde_1).*doZ1_1;
        Z1tilde_2 = (W1_2*X_2 + B1_2).*doZ1_2;
        Z1_2 = sigma1_2(Z1tilde_2).*doZ1_2;
        Z2tilde_1 = (W2_1*Z1_1 + B2_1).*doZ2_1;
        Z2_1 = sigma2_1(Z2tilde_1).*doZ2_1;
        Z2tilde_2 = (W2_2*Z1_2 + B2_2).*doZ2_2;
        Z2_2 = sigma2_2(Z2tilde_2).*doZ2_2;
        Z3tilde = (W3_1*Z2_1 + W3_2*Z2_2 + B3).*doZ3;
        Z3 = sigma3(Z3tilde).*doZ3;
        Z4tilde = (W4*Z3 + B4).*doZ4;
        Z4 = sigma4(Z4tilde).*doZ4;
        Z5tilde = (W5*Z4 + B5).*doZ5;
        Z5 = sigma5(Z5tilde).*doZ5;
        Yp = Wy*Z5 + By;
        Yh = sigmay(Yp);
        
        % Compute the training loss
        Y = A(im, :)';
        C_train(ep) = C_train(ep) + loss(Yh, Y)/epochSize;
        
        % Compute the training prediction accuracy
        if sum(100*abs(Yh - Y)./abs(Y) > minDiff1) == 0 || ...
                sum(abs(Yh - Y) > minDiff2) == 0
            predAcc_train(ep) = predAcc_train(ep) + 100/epochSize;
        end
        
        % Backpropagate
        delta_y = sigmayg(Yp)*lossg(Yh, Y);
        delta_5 = sigma5g(Z5tilde)*(Wy'*delta_y);
        delta_4 = sigma4g(Z4tilde)*(W5'*delta_5);
        delta_3 = sigma3g(Z3tilde)*(W4'*delta_4);
        delta_2_1 = sigma2_1g(Z2tilde_1)*(W3_1'*delta_3);
        delta_2_2 = sigma2_2g(Z2tilde_2)*(W3_2'*delta_3);
        delta_1_1 = sigma1_1g(Z1tilde_1)*(W2_1'*delta_2_1);
        delta_1_2 = sigma1_2g(Z1tilde_2)*(W2_2'*delta_2_2);
        dW1_1 = dW1_1 + delta_1_1*X_1';
        dW1_2 = dW1_2 + delta_1_2*X_2';
        dW2_1 = dW2_1 + delta_2_1*Z1_1';
        dW2_2 = dW2_2 + delta_2_2*Z1_2';
        dW3_1 = dW3_1 + delta_3*Z2_1';
        dW3_2 = dW3_2 + delta_3*Z2_2';
        dW4 = dW4 + delta_4*Z3';
        dW5 = dW5 + delta_5*Z4';
        dWy = dWy + delta_y*Z5';
        dB1_1 = dB1_1 + delta_1_1;
        dB1_2 = dB1_2 + delta_1_2;
        dB2_1 = dB2_1 + delta_2_1;
        dB2_2 = dB2_2 + delta_2_2;
        dB3 = dB3 + delta_3;
        dB4 = dB4 + delta_4;
        dB5 = dB5 + delta_5;
        dBy = dBy + delta_y;
    end
    
    % Step size
    gamma = gamma_max*((gamma_min/gamma_max)^(ep/Nep));
    
    % Partial derivatives
    dW1_1 = dW1_1/epochSize;
    dW1_2 = dW1_2/epochSize;
    dW2_1 = dW2_1/epochSize;
    dW2_2 = dW2_2/epochSize;
    dW3_1 = dW3_1/epochSize;
    dW3_2 = dW3_2/epochSize;
    dW4 = dW4/epochSize;
    dW5 = dW5/epochSize;
    dWy = dWy/epochSize;
    dB1_1 = dB1_1/epochSize;
    dB1_2 = dB1_2/epochSize;
    dB2_1 = dB2_1/epochSize;
    dB2_2 = dB2_2/epochSize;
    dB3 = dB3/epochSize;
    dB4 = dB4/epochSize;
    dB5 = dB5/epochSize;
    dBy = dBy/epochSize;
    
    % Adam Optimizer
    mW1_1 = (beta1*mW1_1 + (1 - beta1)*dW1_1);%/(1 - beta1^ep);
    mW1_2 = (beta1*mW1_2 + (1 - beta1)*dW1_2);%/(1 - beta1^ep);
    mW2_1 = (beta1*mW2_1 + (1 - beta1)*dW2_1);%/(1 - beta1^ep);
    mW2_2 = (beta1*mW2_2 + (1 - beta1)*dW2_2);%/(1 - beta1^ep);
    mW3_1 = (beta1*mW3_1 + (1 - beta1)*dW3_1);%/(1 - beta1^ep);
    mW3_2 = (beta1*mW3_2 + (1 - beta1)*dW3_2);%/(1 - beta1^ep);
    mW4 = (beta1*mW4 + (1 - beta1)*dW4);%/(1 - beta1^ep);
    mW5 = (beta1*mW5 + (1 - beta1)*dW5);%/(1 - beta1^ep);
    mWy = (beta1*mWy + (1 - beta1)*dWy);%/(1 - beta1^ep);
    mB1_1 = (beta1*mB1_1 + (1 - beta1)*dB1_1);%/(1 - beta1^ep);
    mB1_2 = (beta1*mB1_2 + (1 - beta1)*dB1_2);%/(1 - beta1^ep);
    mB2_1 = (beta1*mB2_1 + (1 - beta1)*dB2_1);%/(1 - beta1^ep);
    mB2_2 = (beta1*mB2_2 + (1 - beta1)*dB2_2);%/(1 - beta1^ep);
    mB3 = (beta1*mB3 + (1 - beta1)*dB3);%/(1 - beta1^ep);
    mB4 = (beta1*mB4 + (1 - beta1)*dB4);%/(1 - beta1^ep);
    mB5 = (beta1*mB5 + (1 - beta1)*dB5);%/(1 - beta1^ep);
    mBy = (beta1*mBy + (1 - beta1)*dBy);%/(1 - beta1^ep);
    vW1_1 = (beta2*vW1_1 + (1 - beta2)*dW1_1.*dW1_1);%/(1 - beta2^ep);
    vW1_2 = (beta2*vW1_2 + (1 - beta2)*dW1_2.*dW1_2);%/(1 - beta2^ep);
    vW2_1 = (beta2*vW2_1 + (1 - beta2)*dW2_1.*dW2_1);%/(1 - beta2^ep);
    vW2_2 = (beta2*vW2_2 + (1 - beta2)*dW2_2.*dW2_2);%/(1 - beta2^ep);
    vW3_1 = (beta2*vW3_1 + (1 - beta2)*dW3_1.*dW3_1);%/(1 - beta2^ep);
    vW3_2 = (beta2*vW3_2 + (1 - beta2)*dW3_2.*dW3_2);%/(1 - beta2^ep);
    vW4 = (beta2*vW4 + (1 - beta2)*dW4.*dW4);%/(1 - beta2^ep);
    vW5 = (beta2*vW5 + (1 - beta2)*dW5.*dW5);%/(1 - beta2^ep);
    vWy = (beta2*vWy + (1 - beta2)*dWy.*dWy);%/(1 - beta2^ep);
    vB1_1 = (beta2*vB1_1 + (1 - beta2)*dB1_1.*dB1_1);%/(1 - beta2^ep);
    vB1_2 = (beta2*vB1_2 + (1 - beta2)*dB1_2.*dB1_2);%/(1 - beta2^ep);
    vB2_1 = (beta2*vB2_1 + (1 - beta2)*dB2_1.*dB2_1);%/(1 - beta2^ep);
    vB2_2 = (beta2*vB2_2 + (1 - beta2)*dB2_2.*dB2_2);%/(1 - beta2^ep);
    vB3 = (beta2*vB3 + (1 - beta2)*dB3.*dB3);%/(1 - beta2^ep);
    vB4 = (beta2*vB4 + (1 - beta2)*dB4.*dB4);%/(1 - beta2^ep);
    vB5 = (beta2*vB5 + (1 - beta2)*dB5.*dB5);%/(1 - beta2^ep);
    vBy = (beta2*vBy + (1 - beta2)*dBy.*dBy);%/(1 - beta2^ep);
    dW1_1 = mW1_1./(sqrt(vW1_1) + epsilon);
    dW1_2 = mW1_2./(sqrt(vW1_2) + epsilon);
    dW2_1 = mW2_1./(sqrt(vW2_1) + epsilon);
    dW2_2 = mW2_2./(sqrt(vW2_2) + epsilon);
    dW3_1 = mW3_1./(sqrt(vW3_1) + epsilon);
    dW3_2 = mW3_2./(sqrt(vW3_2) + epsilon);
    dW4 = mW4./(sqrt(vW4) + epsilon);
    dW5 = mW5./(sqrt(vW5) + epsilon);
    dWy = mWy./(sqrt(vWy) + epsilon);
    dB1_1 = mB1_1./(sqrt(vB1_1) + epsilon);
    dB1_2 = mB1_2./(sqrt(vB1_2) + epsilon);
    dB2_1 = mB2_1./(sqrt(vB2_1) + epsilon);
    dB2_2 = mB2_2./(sqrt(vB2_2) + epsilon);
    dB3 = mB3./(sqrt(vB3) + epsilon);
    dB4 = mB4./(sqrt(vB4) + epsilon);
    dB5 = mB5./(sqrt(vB5) + epsilon);
    dBy = mBy./(sqrt(vBy) + epsilon);
    
    % Update the weights
    W1_1 = W1_1 - gamma*dW1_1;%.*(rand(size(W1)) > dropout);
    W1_2 = W1_2 - gamma*dW1_2;%.*(rand(size(W1)) > dropout);
    W2_1 = W2_1 - gamma*dW2_1;%.*(rand(size(W2)) > dropout);
    W2_2 = W2_2 - gamma*dW2_2;%.*(rand(size(W2)) > dropout);
    W3_1 = W3_1 - gamma*dW3_1;%.*(rand(size(W2)) > dropout);
    W3_2 = W3_2 - gamma*dW3_2;%.*(rand(size(W2)) > dropout);
    W4 = W4 - gamma*dW4;%.*(rand(size(W4)) > dropout);
    W5 = W5 - gamma*dW5;%.*(rand(size(W4)) > dropout);
    Wy = Wy - gamma*dWy;%.*(rand(size(Wy)) > dropout);
    B1_1 = B1_1 - gamma*dB1_1;%.*(rand(size(B1)) > dropout);
    B1_2 = B1_2 - gamma*dB1_2;%.*(rand(size(B1)) > dropout);
    B2_1 = B2_1 - gamma*dB2_1;%.*(rand(size(B1)) > dropout);
    B2_2 = B2_2 - gamma*dB2_2;%.*(rand(size(B1)) > dropout);
    B3 = B3 - gamma*dB3;%.*(rand(size(B3)) > dropout);
    B4 = B4 - gamma*dB4;%.*(rand(size(B4)) > dropout);
    B5 = B5 - gamma*dB5;%.*(rand(size(B4)) > dropout);
    By = By - gamma*dBy;%.*(rand(size(By)) > dropout);
    
    % Compute the test loss and prediction accuracy
    C_temp = 0;
    for k = idx_test
        X_1 = Tstt(k, :)';
        X_2 = Tfts(k, :)';
        Z1tilde_1 = (W1_1*X_1 + B1_1)*pkeep;
        Z1_1 = sigma1_1(Z1tilde_1)*pkeep;
        Z1tilde_2 = (W1_2*X_2 + B1_2)*pkeep;
        Z1_2 = sigma1_2(Z1tilde_2)*pkeep;
        Z2tilde_1 = (W2_1*Z1_1 + B2_1)*pkeep;
        Z2_1 = sigma2_1(Z2tilde_1)*pkeep;
        Z2tilde_2 = (W2_2*Z1_2 + B2_2)*pkeep;
        Z2_2 = sigma2_2(Z2tilde_2)*pkeep;
        Z3tilde = (W3_1*Z2_1 + W3_2*Z2_2 + B3)*pkeep;
        Z3 = sigma3(Z3tilde)*pkeep;
        Z4tilde = (W4*Z3 + B4)*pkeep;
        Z4 = sigma4(Z4tilde)*pkeep;
        Z5tilde = (W5*Z4 + B5)*pkeep;
        Z5 = sigma5(Z5tilde)*pkeep;
        Yp = Wy*Z5 + By;
        Yh = sigmay(Yp);
        Y = A(k, :)';
        C_test(ep) = C_test(ep) + loss(Yh, Y)/Ntest;
        if sum(100*abs(Yh - Y)./abs(Y) > minDiff1) == 0 || ...
                sum(abs(Yh - Y) > minDiff2) == 0
            predAcc_test(ep) = predAcc_test(ep) + 100/Ntest;
        end
    end
    
    % Update predAccMax and save the weights
    if predAcc_test(ep) > predAccMax
        predAccMax = predAcc_test(ep);
        if save_flag == 1
            save('../../mat/weights.mat', ...
                'W1_1', 'W1_2', 'W2_1', 'W2_2', 'W3_1', 'W3_2', 'W4', 'W5', 'Wy', ...
                'B1_1', 'B1_2', 'B2_1', 'B2_2', 'B3', 'B4', 'B5', 'By', ...
                'predAccMax', 'idx_train', 'idx_test');
        end
    end
    
    % Compute the largest partial derivative
    maxWeight = max(abs(min(min(dW1_1))), max(max(dW1_1))) + ...
        max(abs(min(min(dW1_2))), max(max(dW1_2))) + ...
        max(abs(min(min(dW2_1))), max(max(dW2_1))) + ...
        max(abs(min(min(dW2_2))), max(max(dW2_2))) + ...
        max(abs(min(min(dW3_1))), max(max(dW3_1))) + ...
        max(abs(min(min(dW3_2))), max(max(dW3_2))) + ...
        max(abs(min(min(dW4))), max(max(dW4))) + ...
        max(abs(min(min(dW5))), max(max(dW5))) + ...
        max(abs(min(min(dWy))), max(max(dWy))) + ...
        max(abs(min(min(dB1_1))), max(max(dB1_1))) + ...
        max(abs(min(min(dB1_2))), max(max(dB1_2))) + ...
        max(abs(min(min(dB2_1))), max(max(dB2_1))) + ...
        max(abs(min(min(dB2_2))), max(max(dB2_2))) + ...
        max(abs(min(min(dB3))), max(max(dB3))) + ...
        max(abs(min(min(dB4))), max(max(dB4))) + ...
        max(abs(min(min(dB5))), max(max(dB5))) + ...
        max(abs(min(min(dBy))), max(max(dBy)));
    
    % Display information
    fprintf('Epoch %d: C = %.3f \t acc = %.2f %%\t max(dW) = %.2e \n', ...
        ep, C_train(ep), predAcc_test(ep), maxWeight);
    
    % Plot the error and prediction accuracy
    subplot(1, 2, 1);
    plot(1:ep, C_train(1:ep), '-b', 1:ep, C_test(1:ep), '-r');
    title('Loss');
    xlabel('Epoch number');
    if strcmp(func2str(loss), 'crossEntropyLoss')
        ylabel('Cross-entropy loss');
    elseif strcmp(func2str(loss), 'quadraticLoss')
        ylabel('Quadratic loss');
    else
        ylabel('Loss');
    end
    legend('training loss', 'test loss');
    grid on;
    subplot(1, 2, 2);
    plot(1:ep, predAcc_train(1:ep), '-b', 1:ep, predAcc_test(1:ep), '-r');
    title('Prediction accuracy');
    xlabel('Epoch number');
    ylabel('Accuracy in %');
    legend('training accuracy', 'test accuracy', 'Location', 'southeast');
    grid on;
    
    % Display progress
    waitbar(ep/Nep, h);
end
close(h);


%% TESTING

% Test for an image
testim = randsample(idx_train, 1);
X_1 = Tstt(testim, :)';
X_2 = Tfts(testim, :)';
Z1tilde_1 = (W1_1*X_1 + B1_1)*pkeep;
Z1_1 = sigma1_1(Z1tilde_1)*pkeep;
Z1tilde_2 = (W1_2*X_2 + B1_2)*pkeep;
Z1_2 = sigma1_2(Z1tilde_2)*pkeep;
Z2tilde_1 = (W2_1*Z1_1 + B2_1)*pkeep;
Z2_1 = sigma2_1(Z2tilde_1)*pkeep;
Z2tilde_2 = (W2_2*Z1_2 + B2_2)*pkeep;
Z2_2 = sigma2_2(Z2tilde_2)*pkeep;
Z3tilde = (W3_1*Z2_1 + W3_2*Z2_2 + B3)*pkeep;
Z3 = sigma3(Z3tilde)*pkeep;
Z4tilde = (W4*Z3 + B4)*pkeep;
Z4 = sigma4(Z4tilde)*pkeep;
Z5tilde = (W5*Z4 + B5)*pkeep;
Z5 = sigma5(Z5tilde)*pkeep;
Yp = Wy*Z5 + By;
Yh = sigmay(Yp)
Y = A(k, :)'

% Plot the error and prediction accuracy
figure;
subplot(1, 2, 1);
plot(1:ep, C_train(1:ep), '-b', 1:ep, C_test(1:ep), '-r');
title('Loss');
xlabel('Epoch number');
if strcmp(func2str(loss), 'crossEntropyLoss')
    ylabel('Cross-entropy loss');
elseif strcmp(func2str(loss), 'quadraticLoss')
    ylabel('Quadratic loss');
else
    ylabel('Loss');
end
legend('training loss', 'test loss');
grid on;
subplot(1, 2, 2);
plot(1:ep, predAcc_train(1:ep), '-b', 1:ep, predAcc_test(1:ep), '-r');
title('Prediction accuracy');
xlabel('Epoch number');
ylabel('Accuracy in %');
legend('training accuracy', 'test accuracy');
grid on;

% Display the best prediction accuracy
disp(' ');
disp(['Highest accuracy: ' num2str(predAccMax) ' %']);


