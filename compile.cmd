set FOLDER=/

gcc -o object_files/main.o -c %FOLDER%main.c"
gcc -o object_files/bruteforce.o -c %FOLDER%bruteforce.c
gcc -o object_files/bitset.o -c %FOLDER%bitset.c
gcc -o object_files/graph.o -c %FOLDER%graph.c
gcc -o object_files/binaryheap.o -c %FOLDER%binaryheap.c
gcc -o object_files/utils.o -c %FOLDER%utils.c
gcc -o object_files/graphgenerators.o -c %FOLDER%graphgenerators.c
gcc -o object_files/exact.o -c %FOLDER%exact.c
gcc -o object_files/algutils.o -c %FOLDER%algutils.c
gcc -o object_files/greedy.o -c %FOLDER%greedy.c
gcc -o object_files/priorityfunctions.o -c %FOLDER%priorityfunctions.c
gcc -o object_files/hochbaum.o -c %FOLDER%hochbaum.c
gcc -o object_files/plesnik.o -c %FOLDER%plesnik.c
gcc -o object_files/backtracking.o -c %FOLDER%backtracking.c
gcc -o object_files/heuristic.o -c %FOLDER%heuristic.c
gcc -o object_files/result_to_json.o -c %FOLDER%result_to_json.c
gcc -o object_files/tester.o -c %FOLDER%tester.c
gcc -o object_files/setcover.o -c %FOLDER%setcover.c
gcc -o object_files/independantset.o -c %FOLDER%independantset.c

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