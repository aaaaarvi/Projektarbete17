
% Reformats the data set for training a neural network in the task of
% regressing the momenta of four the final state particles using the tube
% hits in the stt and fts together with the time stamps. It reads the data
% from the csv-files dataTSMom* and creates the input and output matrices.
% The possibility exists to use PCA as well.

clear;

% Data properties
NtubesSTT = 4542;
NtubesFTS = 12224; % 13056
Nfiles = 19;
Nevents = 1000;

% Choose number of PCA components
NcompSTT = 1000;
NcompFTS = 2000;

% Import data
Tstt = sparse(zeros(1, NtubesSTT));
Tfts = sparse(zeros(1, NtubesFTS));
A = zeros(Nfiles*Nevents, 8);
disp('Importing data...');
for i = 1:Nfiles
    csv = csvread(['../../dataTS/dataTSMom_' num2str(i) '.csv']);
    csvSize = size(csv);
    i_vect_stt = zeros(csvSize(1)*csvSize(2), 1);
    j_vect_stt = zeros(csvSize(1)*csvSize(2), 1);
    v_vect_stt = zeros(csvSize(1)*csvSize(2), 1);
    n_vals_stt = 0;
    i_vect_fts = zeros(csvSize(1)*csvSize(2), 1);
    j_vect_fts = zeros(csvSize(1)*csvSize(2), 1);
    v_vect_fts = zeros(csvSize(1)*csvSize(2), 1);
    n_vals_fts = 0;
    for j = 1:Nevents
        eventIdx = j + (i-1)*Nevents;
        Ntracks = csv(j, 1);
        for k = 1:Ntracks
            type = csv(j, 3*k - 1);
            switch type
                case 2212
                    A(eventIdx, 1) = csv(j, 3*k);
                    A(eventIdx, 2) = csv(j, 3*k + 1);
                case -2212
                    A(eventIdx, 3) = csv(j, 3*k);
                    A(eventIdx, 4) = csv(j, 3*k + 1);
                case 211
                    A(eventIdx, 5) = csv(j, 3*k);
                    A(eventIdx, 6) = csv(j, 3*k + 1);
                case -211
                    A(eventIdx, 7) = csv(j, 3*k);
                    A(eventIdx, 8) = csv(j, 3*k + 1);
            end
        end
        
        % Offsets and number of hits
        offsetSTT = 3 + 3*Ntracks;
        hitsSTT = csv(j, offsetSTT-1);
        hitsFTS = csv(j, offsetSTT);
        offsetFTS = offsetSTT + 2*hitsSTT;
        
        % Add the values to the vectors
        for k = 1:2:2*hitsSTT
            n_vals_stt = n_vals_stt + 1;
            i_vect_stt(n_vals_stt) = j;
            j_vect_stt(n_vals_stt) = csv(j, k + offsetSTT);
            v_vect_stt(n_vals_stt) = csv(j, k + offsetSTT + 1);
            %Tstt(eventIdx, csv(j, k + offsetSTT)) = 1;
        end
        for k = 1:hitsFTS
            n_vals_fts = n_vals_fts + 1;
            i_vect_fts(n_vals_fts) = j;
            j_vect_fts(n_vals_fts) = csv(j, k + offsetFTS);
            v_vect_fts(n_vals_fts) = 1;
            %Tfts(eventIdx, csv(j, k + offsetFTS)) = 1;
        end
    end
    
    % Concatenate the new matrices and take care of matrix dimensions
    temp = sparse(i_vect_stt(1:n_vals_stt), j_vect_stt(1:n_vals_stt), v_vect_stt(1:n_vals_stt));
    tempSize = size(temp);
    temp2 = [[temp; zeros(1000 - tempSize(1), tempSize(2))], zeros(1000, NtubesSTT - tempSize(2))];
    Tstt = [Tstt; temp2];
    temp = sparse(i_vect_fts(1:n_vals_fts), j_vect_fts(1:n_vals_fts), v_vect_fts(1:n_vals_fts));
    tempSize = size(temp);
    temp2 = [[temp; zeros(1000 - tempSize(1), tempSize(2))], zeros(1000, NtubesFTS - tempSize(2))];
    Tfts = [Tfts; temp2];
end
Tstt(1,:) = [];
Tfts(1,:) = [];

% Perform PCA
%disp('Starting PCA...');
%load('../../mat/dataTS.mat', 'NcompSTT', 'NcompFTS', 'coeffSTT', 'coeffFTS');
%coeffSTT = pca(full(Tstt(1:50000,:)));%, 'NumComponents', NcompSTT);
%coeffFTS = pca(full(Tfts(1:50000,:)));%, 'NumComponents', NcompFTS);

% Save the data
disp('Saving data...');
Npoints = Nfiles*Nevents;
save('../../mat/dataTS.mat', 'NtubesSTT', 'NtubesFTS', 'Npoints', 'A', 'Tstt', 'Tfts');
%save('../../mat/dataTS.mat', 'NtubesSTT', 'NtubesFTS', 'Npoints', 'A', 'Tstt', 'Tfts', ...
%    'NcompSTT', 'NcompFTS', 'coeffSTT', 'coeffFTS');
disp('Done!');



