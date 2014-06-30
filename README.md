TutorialAna
===========

The xAOD analysis-in-ROOT tutorial

I'm following the instructions from this twiki:
https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/SoftwareTutorialxAODAnalysisInROOT

Here are the rough installation instructions.

Checking out my package with git:

```git clone git@github.com:sparticlesteve/TutorialAna.git```

or with svn:

```svn co https://github.com/sparticlesteve/TutorialAna TutorialAna```

Setting up the release:

```bash
setupATLAS
rcSetup Base,2.0.3
rc find_packages
rc compile
```
