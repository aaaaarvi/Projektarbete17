# Projektarbete17

This repository contains code for the Project course in Scientific Computing created by Arvi Jonnarth and Adam Hedkvist. It contains scripts for running simulations in the pandaroot environment, generating data, reformatting data and training neural networks.

The data simulation scripts are written in C++ and are located in the ‘PatternMatchingTests’ folder. These files are meant to be compiled together with pandaroot. The folder should be included in the CMakeLists.txt of pandaroot and linked in /pandaroot/PndTrackers/. These scripts generate csv-files which contain data for training the neural networks.

The ‘macros’ folder contains the macros (C-files) for running the C++ files. It also contains some bash files for running several runs in a batch.

The ‘Matlab’ folder contains all the m-files which first rewrites the data into matrices (CreateData*-files) and then trains the neural networks (NeuralNetwork*-files). This folder also contains separate files for the activation and loss functions and their derivatives.

-----------------------------------------------TEST RUNS---------------------------------------------

We have provided some smaller example data set. To run the Pattern recognition network, you need to go into the Matlab folder and change the load command in NeuralNet3 which is located in the beginning of the script from

load('../../mat/dataPat.mat');

to

load('./dataPat.mat');

This will train a network for extracting the final state proton. In order to train the network for momentum regression, go into the Matlab folder again and change the load command in NeuralNet1 from 

load('../../mat/dataPatMom.mat');

to

load('./dataPatMom.mat');





