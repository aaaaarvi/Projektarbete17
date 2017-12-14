
% Reformats the data set for training a neural network in the task of
% classifying which tube hits belong to the the final state proton. It
% reads the data from the csv-files dataTSPat* and creates the input and
% output matrices.

clear;

% Data properties
NtubesSTT = 4542;
Nfiles = 400;
Nevents = 1000;

% Import data
Tstt = sparse(zeros(1, 2*NtubesSTT));
A = sparse(zeros(1, NtubesSTT));
disp('Importing data...');
for i = 1:Nfiles
    disp(['i = ' num2str(i)]);
    csv = csvread(['../../dataTSPat/dataTSPat_' num2str(i) '.csv']);
    csvSize = size(csv);
    i_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    j_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    v_vect_T = zeros(csvSize(1)*csvSize(2), 1);
    n_vals_T = 0;
    i_vect_A = zeros(csvSize(1)*csvSize(2), 1);
    j_vect_A = zeros(csvSize(1)*csvSize(2), 1);
    v_vect_A = zeros(csvSize(1)*csvSize(2), 1);
    n_vals_A = 0;
    for j = 1:Nevents
        Nhits_tot = csv(j, 1);
        Nhits_p = csv(j, 2);
        i_temp_T = j*ones(2*Nhits_tot, 1);
        j_temp_T = zeros(2*Nhits_tot, 1);
        v_temp_T = zeros(2*Nhits_tot, 1);
        n_temp_T = 0;
        
        % Assemble the input matrix T
        offset = 2;
        for k = 1:2:2*Nhits_tot
            if ismember(csv(j, k + offset), j_temp_T)
                continue;
            end
            n_temp_T = n_temp_T + 1;
            j_temp_T(n_temp_T) = csv(j, k + offset);
            v_temp_T(n_temp_T) = 1;
            n_temp_T = n_temp_T + 1;
            j_temp_T(n_temp_T) = csv(j, k + offset) + NtubesSTT;
            v_temp_T(n_temp_T) = csv(j, k + offset + 1);
        end
        i_vect_T( (n_vals_T+1):(n_vals_T+n_temp_T) ) = i_temp_T(1:n_temp_T);
        j_vect_T( (n_vals_T+1):(n_vals_T+n_temp_T) ) = j_temp_T(1:n_temp_T);
        v_vect_T( (n_vals_T+1):(n_vals_T+n_temp_T) ) = v_temp_T(1:n_temp_T);
        n_vals_T = n_vals_T + n_temp_T;
        
        % Assmble the output matrix A
        offset = 2 + 2*Nhits_tot;
        for k = 1:Nhits_p
            n_vals_A = n_vals_A + 1;
            i_vect_A(n_vals_A) = j;
            j_vect_A(n_vals_A) = csv(j, k + offset);
            v_vect_A(n_vals_A) = 1;
        end
    end
    
    % Concatenate the new matrices and take care of matrix dimensions
    temp = sparse(i_vect_T(1:n_vals_T), j_vect_T(1:n_vals_T), v_vect_T(1:n_vals_T));
    tempSize = size(temp);
    temp2 = [[temp; zeros(Nevents - tempSize(1), tempSize(2))], zeros(Nevents, 2*NtubesSTT - tempSize(2))];
    Tstt = [Tstt; temp2];
    temp = sparse(i_vect_A(1:n_vals_A), j_vect_A(1:n_vals_A), v_vect_A(1:n_vals_A));
    tempSize = size(temp);
    temp2 = [[temp; zeros(Nevents - tempSize(1), tempSize(2))], zeros(Nevents, NtubesSTT - tempSize(2))];
    A = [A; temp2];
end
Tstt(1,:) = [];
A(1,:) = [];

% Make sure that the A matrix only contains 0 and 1
A(A ~= 0) = 1;

% Save the data
disp('Saving data...');
Npoints = Nfiles*Nevents;
save('../../mat/dataTSPat.mat', 'NtubesSTT', 'Npoints', 'A', 'Tstt');
disp('Done!');



