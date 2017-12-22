
% Reformats the data set for training a neural network in the task of
% classifying which tube hits belong to the the final state proton. It
% reads the data from the csv-files dataPat* and creates the input and
% output matrices.

clear;

% Data properties
NtubesSTT = 4542;
Nfiles = 1336;
Nevents = 1000;

% Import data
Tstt = sparse(zeros(1, NtubesSTT));
A = zeros(Nfiles*Nevents, 2);
disp('Importing data...');
for i = 1:Nfiles
    disp(['i = ' num2str(i)]);
    csv = csvread(['../../dataPat/dataPat_' num2str(i) '.csv']);
    csvSize = size(csv);
    i_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    j_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    v_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    n_vals_T = 0;
    for j = 1:Nevents
        Nhits_tot = csv(j, 1);
        Nhits_p = csv(j, 2);
        
        % Assmble the input matrix T
        offset = 4 + Nhits_tot;
        for k = 1:Nhits_p
            n_vals_T = n_vals_T + 1;
            i_vect_T(n_vals_T) = j;
            j_vect_T(n_vals_T) = csv(j, k + offset);
            v_vect_T(n_vals_T) = 1;
        end
        
        % Assemle the output matrix A
        A((i-1)*Nevents + j, 1) = csv(j, 3);
        A((i-1)*Nevents + j, 2) = csv(j, 4);
    end
    
    % Concatenate the new matrices and take care of matrix dimensions
    temp = sparse(i_vect_T(1:n_vals_T), j_vect_T(1:n_vals_T), v_vect_T(1:n_vals_T));
    tempSize = size(temp);
    temp2 = [[temp; zeros(Nevents - tempSize(1), tempSize(2))], zeros(Nevents, NtubesSTT - tempSize(2))];
    Tstt = [Tstt; temp2];
end
Tstt(1,:) = [];

% Make sure that elements are only 0 or 1
Tstt(Tstt ~= 0) = 1;

% Save the data
disp('Saving data...');
Npoints = Nfiles*Nevents;
save('../../mat/dataPatMom.mat', 'NtubesSTT', 'Npoints', 'A', 'Tstt');
disp('Done!');



