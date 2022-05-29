# SIRQD epidemic modelling

This project is made to simulate an epidemic spread based on two-layer graphs.

### How to start
1. To clone repo you need to use `--recurse-submodules` flag to include SortedSparseMatrix repo
2. Create data using python script included in repo

### How to use 
1. Use `.import_relations_from_file()` method and pass the path to the file. It's recommended that you use separate files for two layers of the sim
2. Create probabilities object that will hold probabilities for each state change
3. Use `create_params()` function to heap allocate data for the sim
4. Append pointer to this data to `parameters` vector
5. Call `run_simulations()` and pass `parameters` vector as argument
6. compile and run
