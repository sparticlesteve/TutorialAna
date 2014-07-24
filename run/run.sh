#!/bin/bash

[ -d submitDir ] && rm -rf submitDir

#root -l 'ATestRun.cxx ("submitDir")'
testRun submitDir
