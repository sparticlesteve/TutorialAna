#!/bin/env python

# Just a quick reflex dictionary test
import PyCintex
import ROOT
from ROOT import gROOT
gROOT.ProcessLine('.x $ROOTCOREDIR/scripts/load_packages.C')

from ROOT import JetCleaningTool

# Configure the JetCleaningTool
jetCleaning = ROOT.JetCleaningTool('JetCleaningTool')
