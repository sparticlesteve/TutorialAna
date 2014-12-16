#!/bin/bash

[ -d submitDir ] && rm -rf submitDir

#root -l 'ATestRun.cxx ("submitDir")'

#time testRun submitDir
time ./testRun.py -b
