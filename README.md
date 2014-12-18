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
- Dense matrix-vector multiplication
- Dense matrix-matrix multiplication
- Sparse matrix-vector multiplication
- Sparse matrix-matrix multiplication

Prerequisites
-
You will need a recent version of gcc and the COPRTHR SDK 1.6 or later, which gives you the clcc command. The programs are only tested on gcc 4.8.x running on Ubuntu (Linaro) 14.04 on the 16-core Desktop Parallella.

How to install
-
Simply clone this git repo:
    
    git clone ...

*TODO: Fix easy compilation and write here how to do*

Dense programs
-
*TODO: Details on the dense programs and how to use them.*

Sparse programs
-
All the kernels accept files written in SRW (sparse row-wise) format. There are example files in the "matrices" directory. You must specify exactly how large the matrix is and how many nonzero elements there are.

*TODO: Link to SRW format description*
