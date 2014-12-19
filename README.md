parallellalinalg
================

Basic Linear Algebra programs with COPRTHR 1.6 compliant OpenCL kernels for use with the Adapteva Parallella.

Background
-
These programs were written by Marcus Näslund in 2014 and used by Marcus Näslund and Jacob Mattsson to extensively test the performence of the 16-core Parallella board for Linear Algebra computations. The full report of our work, including benchmark results to compare your results with, are available at...

*TODO: Add a link to the final report*

Contents
-
This repository contains four major kernels:

* Dense matrix-vector multiplication
* Dense matrix-matrix multiplication
* Sparse matrix-vector multiplication
* Sparse matrix-matrix multiplication

There is also a "matlab" directory which contains scripts for generating sparse matrices in the correct (SRW) format for use with the sparse programs.

Prerequisites
-
You will need a recent version of gcc and the COPRTHR SDK 1.6 or later, which gives you the clcc command. The programs are only tested on gcc 4.8.x running on Ubuntu (Linaro) 14.04 on the 16-core Desktop Parallella.

How to install
-
If you don't have git installed, first do that, for example:

    sudo apt-get install git

Then clone this git repo:
    
    git clone https://github.com/naslundx/parallellalinalg.git
    
In each subdirectory of the "src" directory there is a makefile. Open each folder and type

    make
    
and the program will be compiled. 

Dense programs
-
*TODO: Details on the dense programs and how to use them.*

Sparse programs
-
All the kernels accept files written in SRW (sparse row-wise) format. There are example files in the "matrices" directory. You must specify exactly how large the matrix is and how many nonzero elements there are. For a description of the SRW format, see the "Yale" format at http://en.wikipedia.org/wiki/Sparse_matrix.


*TODO: Link to SRW format description*

Timing
-
All programs use the built-in clock() functionality of C to measure time. Since the parallellism only takes place on the Epiphany chip, this works fine. After the program has run, it outputs four timings:

* t_alloc: The time it takes to allocate memory
* t_clalloc: The time it takes to fill this memory (with data from file and/or generated random data)
* t_calc: The time spent waiting on the Epiphany chip to calculate
* t_full: Full runtime.

License
-
All the software is distributed under the MIT License. See separate file LICENSE.
