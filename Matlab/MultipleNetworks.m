
% Combines several similar networks.
% Note: Must have the same test set!

clear;

% Number of networks
Nnets = 10;

% Minimum momentum difference
minDiff1 = 10;   % percent (of magnitude)
minDiff2 = 5;    % absolute (of angle in degrees)

% Load test indices
load('../../saves/runs_1c/weights1c_1.mat', 'idx_test');
idx = idx_test;
Ntest = length(idx);

% Load weight sizes and pkeep
load('../../saves/runs_1c/weights1c_1.mat', 'pkeep', ...
    'n', 's1', 's2', 's3', 's4', 's5', 's6', 'm');

% Load event data
load('../../mat/dataPatMom.mat');
T = Tstt;

% Activation functions
sigma1  = @relu;
sigma2  = @relu;
sigma3  = @relu;
sigma4  = @relu;
sigma5  = @relu;
sigma6  = @relu;
sigmay  = @lin;

% Initialize weight and bias tensors
W1_all = zeros(s1, n, Nnets);
W2_all = zeros(s2, s1, Nnets);
W3_all = zeros(s3, s2, Nnets);
W4_all = zeros(s4, s3, Nnets);
W5_all = zeros(s5, s4, Nnets);
W6_all = zeros(s6, s5, Nnets);
Wy_all = zeros(m, s6, Nnets);
B1_all = zeros(s1, Nnets);
B2_all = zeros(s2, Nnets);
B3_all = zeros(s3, Nnets);
B4_all = zeros(s4, Nnets);
B5_all = zeros(s5, Nnets);
B6_all = zeros(s6, Nnets);
By_all = zeros(m, Nnets);

% Load weights and biases and store them
for i = 1:Nnets
    load(['../../saves/runs_1c/weights1c_' num2str(i) '.mat'], ...
        'W1', 'W2', 'W3', 'W4', 'W5', 'W6', 'Wy', ...
        'B1', 'B2', 'B3', 'B4', 'B5', 'B6', 'By');
    W1_all(:, :, i) = W1;
    W2_all(:, :, i) = W2;
    W3_all(:, :, i) = W3;
    W4_all(:, :, i) = W4;
    W5_all(:, :, i) = W5;
    W6_all(:, :, i) = W6;
    Wy_all(:, :, i) = Wy;
    B1_all(:, i) = B1;
    B2_all(:, i) = B2;
    B3_all(:, i) = B3;
    B4_all(:, i) = B4;
    B5_all(:, i) = B5;
    B6_all(:, i) = B6;
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
        Z5tilde = (W5_all(:,:,j)*Z4 + B5_all(:,j));
        Z5 = sigma5(Z5tilde);
        Z6tilde = (W6_all(:,:,j)*Z5 + B6_all(:,j));
        Z6 = sigma6(Z6tilde);
        Yp = Wy_all(:,:,j)*Z6 + By_all(:,j);
        Yh = sigmay(Yp);
        Y = A(i, :)';
        Yh_combined = Yh_combined + Yh;
        
        % Compute average accuracy
        [theta1, rho1] = cart2pol(Y(1), Y(2));
        [theta2, rho2] = cart2pol(Yh(1), Yh(2));
        theta1 = theta1*180/pi;
        theta2 = theta2*180/pi;
        theta_diff = theta2 - theta1;
        if (100*abs(rho2 - rho1)/rho1 < minDiff1) && ...
                (min([abs(theta_diff), abs(theta_diff + 360), abs(theta_diff - 360)]) < minDiff2)
            acc(j) = acc(j) + 100/Ntest;
        end
        
        % Compute combined accuracy
        Yhc = Yh_combined/j;
        [theta1, rho1] = cart2pol(Y(1), Y(2));
        [theta2, rho2] = cart2pol(Yhc(1), Yhc(2));
        theta1 = theta1*180/pi;
        theta2 = theta2*180/pi;
        theta_diff = theta2 - theta1;
        if (100*abs(rho2 - rho1)/rho1 < minDiff1) && ...
                (min([abs(theta_diff), abs(theta_diff + 360), abs(theta_diff - 360)]) < minDiff2)
            acc_combined(j) = acc_combined(j) + 100/Ntest;
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









