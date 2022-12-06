# SIRQD epidemic modelling

This project is made to simulate an epidemic spread based on two-layer graphs.

## How to start
1. To clone repo you need to use `--recurse-submodules` flag to include SortedSparseMatrix repo
2. Create data using python script included in repo

## How to use 
1. Set parameters in main.cpp
2. compile and run

## Contents
```
+- data
| - contains input data for the app
+- include
| - double_buffer
| - FastRandomLib
| - SortedSparseMatrix
+- raport
| - raport about the state of the matrices used in the simulation 
+- results
| - holds the output files of the app (currently just a bin, as we are in testing phase)
+- src
| - main.cpp file 
| - relations generator (generate_random_conections.py)
| - other modules that are project specific (SIRQD_bulk)
+
CMakeLists.txt
.gitignore
.gitmodules
README.md

```
