
% Reformats the data set for training a neural network in the task of
% classifying which final state particle gave rise to a specific track
% candidate in the stt. It reads the data from the csv-files dataClass* and
% creates the input and output matrices.

clear;

% Data properties
NtubesSTT = 4542;
Nfiles = 258;
Npoints = 0;
Ncols = 5;

% Import data
Tstt = sparse(zeros(1, NtubesSTT));
A = sparse(zeros(1, Ncols));
disp('Importing data...');
for i = 1:Nfiles
    csv = csvread(['../../dataClass/dataClass_' num2str(i) '.csv']);
    csvSize = size(csv);
    Nevents = csvSize(1);
    i_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    j_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    v_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    n_vals_T = 0;
    i_vect_A = zeros(Nevents, 1);
    j_vect_A = zeros(Nevents, 1);
    v_vect_A = zeros(Nevents, 1);
    
    % Create the A matrix
    for j = 1:Nevents
        type = csv(j, 1);
        Nhits = csv(j, 2);
        i_vect_A(j) = j;
        v_vect_A(j) = 1;
        switch type
            case 2212
                j_vect_A(j) = 1;
            case -2212
                j_vect_A(j) = 2;
            case 211
                j_vect_A(j) = 3;
            case -211
                j_vect_A(j) = 4;
            case 0
                j_vect_A(j) = 5;
        end
        
        % Create the T matrix
        for k = 1:Nhits
            n_vals_T = n_vals_T + 1;
            i_vect_T(n_vals_T) = j;
            j_vect_T(n_vals_T) = csv(j, k + 2);
            v_vect_T(n_vals_T) = 1;
        end
    end
    
    % Concatenate the new matrices and take care of matrix dimensions
    temp = sparse(i_vect_T(1:n_vals_T), j_vect_T(1:n_vals_T), v_vect_T(1:n_vals_T));
    tempSize = size(temp);
    temp2 = [[temp; zeros(Nevents - tempSize(1), tempSize(2))], zeros(Nevents, NtubesSTT - tempSize(2))];
    Tstt = [Tstt; temp2];
    temp = sparse(i_vect_A, j_vect_A, v_vect_A);
    tempSize = size(temp);
    temp2 = [[temp; zeros(Nevents - tempSize(1), tempSize(2))], zeros(Nevents, Ncols - tempSize(2))];
    A = [A; temp2];
    
    % Update the total number of points
    Npoints = Npoints + Nevents;
end
Tstt(1,:) = [];
A(1,:) = [];

% Save the data
disp('Saving data...');
save('../../mat/dataClass.mat', 'NtubesSTT', 'Npoints', 'Ncols', 'A', 'Tstt');
disp('Done!');



