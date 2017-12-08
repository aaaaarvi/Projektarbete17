
% Trains a neural network in the task of classifying which tube hits of the
% stt belong to the final state proton.

clear;

%% INITIALIZATION

% Load data
load('../../mat/dataTSPat.mat');

% Number of training and testing points (images)
Ntrain = 10000000;
Ntest = 100000;

% Load and save flags
load_flag = 0;
save_flag = 1;

% Learning rate
gamma_min = 0.001;
gamma_max = 0.001;

% Dropout parameter
pkeep = 1;

% Threshold for classifying hits
threshold = 0.99;

% Standard deviation for the initial random weights
st_dev = 0.12;

% Epoch size
epochSize = 1000;
Nep = Ntrain/epochSize; % Nr of epochs

% Number of neurons
n = 2*NtubesSTT; % Number of input neurons
s1 = 200;        % 1:st hidden layer
s2 = 200;        % 2:nd hidden layer
s3 = 200;        % 3:rd hidden layer
s4 = 200;        % 4:th hidden layer
m = NtubesSTT;   % Number of output neurons

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

% Loss function
loss  = @crossEntropyLoss2;
lossg = @crossEntropyLoss2_grad;

% Transform data (not currently relevant)
T = Tstt;

% Divide into training and testing indices
Ntest = min(Npoints/2, Ntest);
%idx_test = randsample(Npoints, Ntest)';
%idx_train = setdiff(1:Npoints, idx_test);
% OR
idx_keep = find(sum(T, 2) ~= 0)';
Npoints = length(idx_keep);
idx_test = 1:Ntest;%randsample(idx_keep, Ntest);
idx_train = setdiff(idx_keep, idx_test);

% Initial weights and biases
W1 = st_dev*randn(s1, n);    % Weights to 1:st hidden layer
W2 = st_dev*randn(s2, s1);   % Weights to 2:nd hidden layer
W3 = st_dev*randn(s3, s2);   % Weights to 3:rd hidden layer
W4 = st_dev*randn(s4, s3);   % Weights to 4:th hidden layer
Wy = st_dev*randn(m, s4);    % Weights to output layer
B1 = st_dev*ones(s1, 1);     % Biases to 1:st hidden layer
B2 = st_dev*ones(s2, 1);     % Biases to 2:nd hidden layer
B3 = st_dev*ones(s3, 1);     % Biases to 3:rd hidden layer
B4 = st_dev*ones(s4, 1);     % Biases to 4:th hidden layer
By = st_dev*ones(m, 1);      % Biases to output layer

% Parameters for the Adam Optimizer
beta1 = 0.9;
beta2 = 0.999;
epsilon = 1e-8;
mW1 = zeros(s1, n);   vW1 = zeros(s1, n);
mW2 = zeros(s2, s1);  vW2 = zeros(s2, s1);
mW3 = zeros(s3, s2);  vW3 = zeros(s3, s2);
mW4 = zeros(s4, s3);  vW4 = zeros(s4, s3);
mWy = zeros(m, s4);   vWy = zeros(m, s4);
mB1 = zeros(s1, 1);   vB1 = zeros(s1, 1);
mB2 = zeros(s2, 1);   vB2 = zeros(s2, 1);
mB3 = zeros(s3, 1);   vB3 = zeros(s3, 1);
mB4 = zeros(s4, 1);   vB4 = zeros(s4, 1);
mBy = zeros(m, 1);    vBy = zeros(m, 1);


%% TRAINING

% Train the network
C_train = zeros(Nep, 1);
C_test = zeros(Nep, 1);
predAcc_test = zeros(Nep, 1);
predAcc_train = zeros(Nep, 1);
predAccMax = 0;
jaccard_train = zeros(Nep, 1);
jaccard_test = zeros(Nep, 1);
if load_flag == 1
    load('../../mat/weights4.mat');
end

% Loop through each epoch
figure;
h = waitbar(0, 'Training the neurual network...');
for ep = 1:Nep
    
    % Initialize the weight and bias changes
    dW1 = zeros(s1, n);
    dW2 = zeros(s2, s1);
    dW3 = zeros(s3, s2);
    dW4 = zeros(s4, s3);
    dWy = zeros(m, s4);
    dB1 = zeros(s1, 1);
    dB2 = zeros(s2, 1);
    dB3 = zeros(s3, 1);
    dB4 = zeros(s4, 1);
    dBy = zeros(m, 1);
    
    % Loop through each image in the epoch
    im_train = randsample(idx_train, epochSize);
    for ex = 1:epochSize
        im = im_train(ex);
        
        % Dropout vectors
        doZ1 = 1*(rand(s1, 1) < pkeep);
        doZ2 = 1*(rand(s2, 1) < pkeep);
        doZ3 = 1*(rand(s3, 1) < pkeep);
        doZ4 = 1*(rand(s4, 1) < pkeep);
        
        % Forward propagation (with dropout)
        X = T(im, :)';
        Z1tilde = (W1*X + B1).*doZ1;
        Z1 = sigma1(Z1tilde).*doZ1;
        Z2tilde = (W2*Z1 + B2).*doZ2;
        Z2 = sigma2(Z2tilde).*doZ2;
        Z3tilde = (W3*Z2 + B3).*doZ3;
        Z3 = sigma3(Z3tilde).*doZ3;
        Z4tilde = (W4*Z3 + B4).*doZ4;
        Z4 = sigma4(Z4tilde).*doZ4;
        Yp = Wy*Z4 + By;
        Yh = sigmay(Yp);
        
        % Compute the training loss
        Y = A(im, :)';
        C_train(ep) = C_train(ep) + loss(Yh, Y)/epochSize;
        
        % Compute the training prediction accuracy
        if sum(X(1:NtubesSTT)) ~= 0
            predAcc_train(ep) = predAcc_train(ep) + 100*(sum((Yh > threshold) == Y & X(1:NtubesSTT) == 1)/sum(X(1:NtubesSTT)))/epochSize;
        end
        
        % Compute the training Jaccard index
        jaccard = 1 - pdist([((Yh > threshold).*X(1:NtubesSTT))'; Y'], 'jaccard');
        if ~isnan(jaccard)
            jaccard_train(ep) = jaccard_train(ep) + jaccard/epochSize;
        end
        
        % Backpropagate
        delta_y = sigmayg(Yp).*lossg(Yh, Y);
        delta_4 = sigma4g(Z4tilde)*(Wy'*delta_y);
        delta_3 = sigma3g(Z3tilde)*(W4'*delta_4);
        delta_2 = sigma2g(Z2tilde)*(W3'*delta_3);
        delta_1 = sigma1g(Z1tilde)*(W2'*delta_2);
        dW1 = dW1 + delta_1*X';
        dW2 = dW2 + delta_2*Z1';
        dW3 = dW3 + delta_3*Z2';
        dW4 = dW4 + delta_4*Z3';
        dWy = dWy + delta_y*Z4';
        dB1 = dB1 + delta_1;
        dB2 = dB2 + delta_2;
        dB3 = dB3 + delta_3;
        dB4 = dB4 + delta_4;
        dBy = dBy + delta_y;
    end
    
    % Step size
    gamma = gamma_max*((gamma_min/gamma_max)^(ep/Nep));
    
    % Partial derivatives
    dW1 = dW1/epochSize;
    dW2 = dW2/epochSize;
    dW3 = dW3/epochSize;
    dW4 = dW4/epochSize;
    dWy = dWy/epochSize;
    dB1 = dB1/epochSize;
    dB2 = dB2/epochSize;
    dB3 = dB3/epochSize;
    dB4 = dB4/epochSize;
    dBy = dBy/epochSize;
    
    % Adam Optimizer
    mW1 = (beta1*mW1 + (1 - beta1)*dW1);%/(1 - beta1^ep);
    mW2 = (beta1*mW2 + (1 - beta1)*dW2);%/(1 - beta1^ep);
    mW3 = (beta1*mW3 + (1 - beta1)*dW3);%/(1 - beta1^ep);
    mW4 = (beta1*mW4 + (1 - beta1)*dW4);%/(1 - beta1^ep);
    mWy = (beta1*mWy + (1 - beta1)*dWy);%/(1 - beta1^ep);
    mB1 = (beta1*mB1 + (1 - beta1)*dB1);%/(1 - beta1^ep);
    mB2 = (beta1*mB2 + (1 - beta1)*dB2);%/(1 - beta1^ep);
    mB3 = (beta1*mB3 + (1 - beta1)*dB3);%/(1 - beta1^ep);
    mB4 = (beta1*mB4 + (1 - beta1)*dB4);%/(1 - beta1^ep);
    mBy = (beta1*mBy + (1 - beta1)*dBy);%/(1 - beta1^ep);
    vW1 = (beta2*vW1 + (1 - beta2)*dW1.*dW1);%/(1 - beta2^ep);
    vW2 = (beta2*vW2 + (1 - beta2)*dW2.*dW2);%/(1 - beta2^ep);
    vW3 = (beta2*vW3 + (1 - beta2)*dW3.*dW3);%/(1 - beta2^ep);
    vW4 = (beta2*vW4 + (1 - beta2)*dW4.*dW4);%/(1 - beta2^ep);
    vWy = (beta2*vWy + (1 - beta2)*dWy.*dWy);%/(1 - beta2^ep);
    vB1 = (beta2*vB1 + (1 - beta2)*dB1.*dB1);%/(1 - beta2^ep);
    vB2 = (beta2*vB2 + (1 - beta2)*dB2.*dB2);%/(1 - beta2^ep);
    vB3 = (beta2*vB3 + (1 - beta2)*dB3.*dB3);%/(1 - beta2^ep);
    vB4 = (beta2*vB4 + (1 - beta2)*dB4.*dB4);%/(1 - beta2^ep);
    vBy = (beta2*vBy + (1 - beta2)*dBy.*dBy);%/(1 - beta2^ep);
    dW1 = mW1./(sqrt(vW1) + epsilon);
    dW2 = mW2./(sqrt(vW2) + epsilon);
    dW3 = mW3./(sqrt(vW3) + epsilon);
    dW4 = mW4./(sqrt(vW4) + epsilon);
    dWy = mWy./(sqrt(vWy) + epsilon);
    dB1 = mB1./(sqrt(vB1) + epsilon);
    dB2 = mB2./(sqrt(vB2) + epsilon);
    dB3 = mB3./(sqrt(vB3) + epsilon);
    dB4 = mB4./(sqrt(vB4) + epsilon);
    dBy = mBy./(sqrt(vBy) + epsilon);
    
    % Update the weights
    W1 = W1 - gamma*dW1;%.*(rand(size(W1)) > dropout);
    W2 = W2 - gamma*dW2;%.*(rand(size(W2)) > dropout);
    W3 = W3 - gamma*dW3;%.*(rand(size(W3)) > dropout);
    W4 = W4 - gamma*dW4;%.*(rand(size(W4)) > dropout);
    Wy = Wy - gamma*dWy;%.*(rand(size(Wy)) > dropout);
    B1 = B1 - gamma*dB1;%.*(rand(size(B1)) > dropout);
    B2 = B2 - gamma*dB2;%.*(rand(size(B2)) > dropout);
    B3 = B3 - gamma*dB3;%.*(rand(size(B3)) > dropout);
    B4 = B4 - gamma*dB4;%.*(rand(size(B4)) > dropout);
    By = By - gamma*dBy;%.*(rand(size(By)) > dropout);
    
    % Compute the test loss, prediction accuracy and Jaccard index
    C_temp = 0;
    im_test = randsample(idx_test, epochSize);
    for k = im_test
        X = T(k, :)';
        Z1tilde = (W1*X + B1)*pkeep;
        Z1 = sigma1(Z1tilde)*pkeep;
        Z2tilde = (W2*Z1 + B2)*pkeep;
        Z2 = sigma2(Z2tilde)*pkeep;
        Z3tilde = (W3*Z2 + B3)*pkeep;
        Z3 = sigma3(Z3tilde)*pkeep;
        Z4tilde = (W4*Z3 + B4)*pkeep;
        Z4 = sigma4(Z4tilde)*pkeep;
        Yp = Wy*Z4 + By;
        Yh = sigmay(Yp);
        %Yh = Yh.*X;
        Y = A(k, :)';
        C_test(ep) = C_test(ep) + loss(Yh, Y)/epochSize;
        if sum(X(1:NtubesSTT)) ~= 0
            predAcc_test(ep) = predAcc_test(ep) + 100*(sum((Yh > threshold) == Y & X(1:NtubesSTT) == 1)/sum(X(1:NtubesSTT)))/epochSize;
        end
        jaccard = 1 - pdist([((Yh > threshold).*X(1:NtubesSTT))'; Y'], 'jaccard');
        if ~isnan(jaccard)
            jaccard_test(ep) = jaccard_test(ep) + jaccard/epochSize;
        end
    end
    
    % Update predAccMax and save the weights
    if predAcc_test(ep) > predAccMax
        predAccMax = predAcc_test(ep);
    end
    if save_flag == 1
        save('../../mat/weights4.mat', ...
            'W1', 'W2', 'W3', 'W4', 'Wy', ...
            'B1', 'B2', 'B3', 'B4', 'By', ...
            'mW1', 'mW2', 'mW3', 'mW4', 'mWy', ...
            'mB1', 'mB2', 'mB3', 'mB4', 'mBy', ...
            'vW1', 'vW2', 'vW3', 'vW4', 'vWy', ...
            'vB1', 'vB2', 'vB3', 'vB4', 'vBy', ...
            'predAccMax', 'idx_train', 'idx_test', 'pkeep');
    end
    
    % Compute the largest partial derivative
    maxWeight = max(abs(min(min(dW1))), max(max(dW1))) + ...
        max(abs(min(min(dW2))), max(max(dW2))) + ...
        max(abs(min(min(dW3))), max(max(dW3))) + ...
        max(abs(min(min(dW4))), max(max(dW4))) + ...
        max(abs(min(min(dWy))), max(max(dWy))) + ...
        max(abs(min(min(dB1))), max(max(dB1))) + ...
        max(abs(min(min(dB2))), max(max(dB2))) + ...
        max(abs(min(min(dB3))), max(max(dB3))) + ...
        max(abs(min(min(dB4))), max(max(dB4))) + ...
        max(abs(min(min(dBy))), max(max(dBy)));
    
    % Display information
    fprintf('Epoch %d: C = %.3f \t acc = %.2f %%\t max(dW) = %.2e \t sum(Yh) = %.4f (%.4f) \n', ...
        ep, C_train(ep), predAcc_test(ep), maxWeight, sum(full(Yh)), sum(full(Y)));
    
    % Plot the error and prediction accuracy
    subplot(1, 3, 1);
    plot(0:(ep-1), C_train(1:ep), '-b', 1:ep, C_test(1:ep), '-r');
    title('Loss');
    xlabel('Epoch number');
    if strcmp(func2str(loss), 'crossEntropyLoss')
        ylabel('Cross-entropy loss');
    elseif strcmp(func2str(loss), 'crossEntropyLoss2')
        ylabel('Cross-entropy loss (alternate)');
    elseif strcmp(func2str(loss), 'quadraticLoss')
        ylabel('Quadratic loss');
    else
        ylabel('Loss');
    end
    legend('training loss', 'test loss', 'Location', 'northwest');
    grid on;
    subplot(1, 3, 2);
    plot(0:(ep-1), predAcc_train(1:ep), '-b', 1:ep, predAcc_test(1:ep), '-r');
    title('Prediction accuracy');
    xlabel('Epoch number');
    ylabel('Accuracy in %');
    legend('training accuracy', 'test accuracy', 'Location', 'northwest');
    grid on;
    subplot(1, 3, 3);
    plot(0:(ep-1), jaccard_train(1:ep), '-b', 1:ep, jaccard_test(1:ep), '-r');
    title('Jaccard index');
    xlabel('Epoch number');
    ylabel('Jaccard value');
    legend('training Jaccard index', 'test Jaccard index', 'Location', 'northwest');
    grid on;
    
    % Display progress
    waitbar(ep/Nep, h);
end
close(h);


%% TESTING

% Plot the error and prediction accuracy
figure;
subplot(1, 3, 1);
plot(0:(ep-1), C_train(1:ep), '-b', 1:ep, C_test(1:ep), '-r');
title('Loss');
xlabel('Epoch number');
if strcmp(func2str(loss), 'crossEntropyLoss')
    ylabel('Cross-entropy loss');
elseif strcmp(func2str(loss), 'crossEntropyLoss2')
    ylabel('Cross-entropy loss (alternate)');
elseif strcmp(func2str(loss), 'quadraticLoss')
    ylabel('Quadratic loss');
else
    ylabel('Loss');
end
legend('training loss', 'test loss', 'Location', 'northwest');
grid on;
subplot(1, 3, 2);
plot(0:(ep-1), predAcc_train(1:ep), '-b', 1:ep, predAcc_test(1:ep), '-r');
title('Prediction accuracy');
xlabel('Epoch number');
ylabel('Accuracy in %');
legend('training accuracy', 'test accuracy', 'Location', 'northwest');
grid on;
subplot(1, 3, 3);
plot(0:(ep-1), jaccard_train(1:ep), '-b', 1:ep, jaccard_test(1:ep), '-r');
title('Jaccard index');
xlabel('Epoch number');
ylabel('Jaccard value');
legend('training Jaccard index', 'test Jaccard index', 'Location', 'northwest');
grid on;

% Display the best prediction accuracy
disp(' ');
disp(['Highest accuracy: ' num2str(predAccMax) ' %']);


