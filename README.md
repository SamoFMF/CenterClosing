# CenterClosing
Masters thesis. Implementation of algorithms for Center Closing Problem.

## Compilation
To compile run `compile.cmd`. Set `RPATH` to relative path from `compile.cmd` to the project. Works in Windows CMD, adapt it if you're using a different OS.

## Execution
`CenterClosing.exe filename [flags]`  

where `filename` is the name of the input file. Flags are discussed in [Flags](#flags).

## Flags

There are several optional flags that can be added.

### `-a`

Specifies algorithms to be used. See [Algorithms](#algorithms) for algorithm abbreviations.  
Eg. `-a abc`

### `-t`

Input type, check [Input types](#input-types) for more information.  
Eg. `-t 0`

### `-i`

Takes an integer iterates this many iterations of the input file, by adding index at the end of it.  
Eg. using `-i 2` and filename `graph.txt`, it would run tests on `graph1.txt` and `graph2.txt`.

### `-j`

Takes an integer and sets starting point for [flag `-i`](#-i).  
Eg. `-j 5`

### `-w`

Solves weighted problem. If flag is missing, weights will be set to `1`.  
Eg. `-w`

### `-n`

Enable notifications.  
Eg. `-n`

### `-k`

Set parameter `k` (number of closed centers).  
Eg. `-k 26`

### `-r`

Set range for parameter `k` (number of closed centers). The following 3 integers represent `start`, `stop` and `step`.  
Eg. `-r 2 5 1`

### `-s`

Set number of repetitions for random algorithms, that is the number of times non-deterministic algorithms will be run.  
Eg. `-s 100`

### `-f`

Folder in which results are to be stored.  
Eg. `-f results/`


## Algorithms

Below is a table of algorithm tag (see thesis for more info) & corresponding abbreviations.

<table>
<tr><th>Exact algorithms</th><th>Approximate algorithms</th><th>Approximate algorithms</th></tr>
<tr><td align="center">

| Name | Abb |
|:----:|:---:|
| bnb | a |
| bf | b |
| bt | c |
| btd | d |
| sc | e |
| bnb+ | f |

</td><td align="center">

| Name | Abb |
|:----:|:---:|
| hs | g |
| prand | h |
| pfirst | i |
| pclst | j |
| prand+ | k |
| pfirst+ | l |
| pclst+ | m |

</td><td align="center">

| Name | Abb |
|:----:|:---:|
| hrand | n |
| hbest | o |
| h+ | p |
| greedy | q |
| card | r |
| radius | t |

</td></tr> </table>

## Input types

### `0`

Input file is in the same format as `or-lib`'s `pmed`, see [file](examples/pmed.txt).  
The format of these data files is:  
- number of vertices, number of edges, k  
- for each edge: the end vertices and the cost of the edge

### `1`

Input file is in the same format as `or-lib`'s `cap`, see [file](examples/cap.txt).  
The format of these data files is:
* number of centers (m), number of consumers (n)
* for each center i (i=1,...,m): 
  * capacity, fixed cost (irrelevant)
* for each customer j (j=1,...,n):
  * weight
  * distance to center i (i=1,...,m)

### default

Reads gmaps data for the thesis, should not be used by other users. For that reason no example files were uploaded.

## Results

All results are returned in JSON format.
