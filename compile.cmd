set RPATH=

if not exist "object_files" mkdir object_files

gcc -o object_files/main.o -c %RPATH%main.c
gcc -o object_files/bruteforce.o -c %RPATH%bruteforce.c
gcc -o object_files/bitset.o -c %RPATH%bitset.c
gcc -o object_files/graph.o -c %RPATH%graph.c
gcc -o object_files/binaryheap.o -c %RPATH%binaryheap.c
gcc -o object_files/utils.o -c %RPATH%utils.c
gcc -o object_files/graphgenerators.o -c %RPATH%graphgenerators.c
gcc -o object_files/exact.o -c %RPATH%exact.c
gcc -o object_files/algutils.o -c %RPATH%algutils.c
gcc -o object_files/greedy.o -c %RPATH%greedy.c
gcc -o object_files/priorityfunctions.o -c %RPATH%priorityfunctions.c
gcc -o object_files/hochbaum.o -c %RPATH%hochbaum.c
gcc -o object_files/plesnik.o -c %RPATH%plesnik.c
gcc -o object_files/backtracking.o -c %RPATH%backtracking.c
gcc -o object_files/heuristic.o -c %RPATH%heuristic.c
gcc -o object_files/result_to_json.o -c %RPATH%result_to_json.c
gcc -o object_files/tester.o -c %RPATH%tester.c
gcc -o object_files/setcover.o -c %RPATH%setcover.c
gcc -o object_files/independantset.o -c %RPATH%independantset.c

gcc -O3 -std=gnu17 -o centerclosing^
 object_files/main.o^
 object_files/bruteforce.o^
 object_files/bitset.o^
 object_files/graph.o^
 object_files/binaryheap.o^
 object_files/utils.o^
 object_files/graphgenerators.o^
 object_files/exact.o^
 object_files/algutils.o^
 object_files/greedy.o^
 object_files/priorityfunctions.o^
 object_files/hochbaum.o^
 object_files/plesnik.o^
 object_files/backtracking.o^
 object_files/heuristic.o^
 object_files/result_to_json.o^
 object_files/tester.o^
 object_files/setcover.o^
 object_files/independantset.o