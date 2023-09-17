#!/bin/bash

make
./program
rm t1_attr1_AVL.bin
rm t1_attr1_AVL_duplicateFile.bin
make clean
