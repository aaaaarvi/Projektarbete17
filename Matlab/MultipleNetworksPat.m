
% Combines several similar networks.
% Note: Must have the same test set!

clear;

% Number of networks
Nnets = 1;

% Load test indices
load('../../mat/NN3/weights3_run1.mat', 'idx_test');
idx = idx_test;
Ntest = length(idx);

% Load weight sizes and pkeep
load('../../mat/NN3/weights3_run1.mat', 'pkeep', ...
    'n', 's1', 's2', 's3', 's4', 'm');

% Load event data
load('../../mat/dataPat.mat');
T = Tstt;

% Threshold value
threshold = 0.99;

% Activation functions
sigma1  = @relu;
sigma2  = @relu;
sigma3  = @relu;
sigma4  = @relu;
sigmay  = @sigmoid;

% Initialize weight and bias tensors
W1_all = zeros(s1, n, Nnets);
W2_all = zeros(s2, s1, Nnets);
W3_all = zeros(s3, s2, Nnets);
W4_all = zeros(s4, s3, Nnets);
Wy_all = zeros(m, s4, Nnets);
B1_all = zeros(s1, Nnets);
B2_all = zeros(s2, Nnets);
B3_all = zeros(s3, Nnets);
B4_all = zeros(s4, Nnets);
By_all = zeros(m, Nnets);

% Load weights and biases and store them
for i = 1:Nnets
    load(['../../mat/NN3/weights3_run' num2str(i) '.mat'], ...
        'W1', 'W2', 'W3', 'W4', 'Wy', ...
        'B1', 'B2', 'B3', 'B4', 'By');
    W1_all(:, :, i) = W1;
    W2_all(:, :, i) = W2;
    W3_all(:, :, i) = W3;
    W4_all(:, :, i) = W4;
    Wy_all(:, :, i) = Wy;
    B1_all(:, i) = B1;
    B2_all(:, i) = B2;
    B3_all(:, i) = B3;
    B4_all(:, i) = B4;
    By_all(:, i) = By;
end

% Compute the average and the combined accuracy
acc = zeros(Nnets, 1);
acc_combined = zeros(Nnets, 1);
for i = idx
    Yh_combined = zeros(m, 1);
    for j = 1:Nnets
        
        % Feed forward
        X = T(i, :)';
        Z1tilde = (W1_all(:,:,j)*X + B1_all(:,j))*pkeep;
        Z1 = sigma1(Z1tilde);
        Z2tilde = (W2_all(:,:,j)*Z1 + B2_all(:,j))*pkeep;
        Z2 = sigma2(Z2tilde);
        Z3tilde = (W3_all(:,:,j)*Z2 + B3_all(:,j))*pkeep;
        Z3 = sigma3(Z3tilde);
        Z4tilde = (W4_all(:,:,j)*Z3 + B4_all(:,j))*pkeep;
        Z4 = sigma4(Z4tilde);
        Yp = Wy_all(:,:,j)*Z4 + By_all(:,j);
        Yh = sigmay(Yp);
        Yh_th = zeros(size(Yh));
        Yh_th(Yh > threshold & X == 1) = 1;
        Y = A(i, :)';
        Yh_combined = Yh_combined + Yh;
        
        % Compute average accuracy
        if sum(X) ~= 0
            acc(j) = acc(j) + 100*(sum(Yh_th == Y & X == 1)/sum(X))/Ntest;
        end
        
        % Compute combined accuracy
        Yhc = Yh_combined/j;
        if sum(X) ~= 0
            acc_combined(j) = acc_combined(j) + 100*(sum((Yhc > threshold) == Y & X == 1)/sum(X))/Ntest;
        end
    end
    
    % Display progress
    if mod(i, 500) == 0
        disp(['Progress: ' num2str(round(100*i/idx(end))) ' %']);
    end
end

% Display results
disp(['Average accuracy:      ' num2str(sum(acc)/Nnets) ' %']);
disp(['Highest ind. accuracy: ' num2str(max(acc)) ' %']);
disp(['Combined accuracy:     ' num2str(acc_combined(Nnets)) ' %']);

% Plot accuracy vs number of nets
figure;
plot(1:Nnets, acc_combined, '-b');
xlabel('Number of networks');
ylabel('Prediction accuracy');
grid on;









