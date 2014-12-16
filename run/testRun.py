#!/bin/env python

import ROOT
from ROOT import gROOT
gROOT.ProcessLine('.x $ROOTCOREDIR/scripts/load_packages.C')

from ROOT import SH, EL
from ROOT import MyFirstxAODAnalysis

# Submit directory
submitDir = 'submitDir'

# Sampler handler
sh = SH.SampleHandler()

# Scan for files in directory
l = SH.DiskListLocal('/afs/cern.ch/atlas/project/PAT/xAODs/r5591/mc14_8TeV.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e1727_s1933_s1911_r5591/')
SH.scanSingleDir(sh, 'ttbar', l)
sh.setMetaString('nc_tree', 'CollectionTree')

# The job
job = EL.Job()
job.sampleHandler(sh)

# Our algorithm
GeV = 1000.
alg = MyFirstxAODAnalysis()
alg.lepPtMin = 10.*GeV
alg.jetPtMin = 50.*GeV

# Add the algorithm to the job
job.algsAdd(alg)

# Job driver
driver = EL.DirectDriver()

# Submit the job
driver.submit(job, submitDir)
