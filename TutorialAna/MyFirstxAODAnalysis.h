#ifndef TutorialAna_MyFirstxAODAnalysis_H
#define TutorialAna_MyFirstxAODAnalysis_H

#include <EventLoop/Algorithm.h>

// Infrastructure includes
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#include "TH1.h"

class JetCleaningTool;

class MyFirstxAODAnalysis : public EL::Algorithm
{

public:
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.

  // float cutValue;

public:
  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)

  // Tree *myTree; //!
  // TH1 *myHist; //!

protected:
  xAOD::TEvent* m_event; //!

  #ifndef __CINT__
  JetCleaningTool* m_jetCleaning; //!
  #endif

  int m_eventCounter; //!
  int m_numCleanEvents; //!

  TH1* h_jetPt; //!

public:
  // this is a standard constructor
  MyFirstxAODAnalysis ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  // this is needed to distribute the algorithm to the workers
  ClassDef(MyFirstxAODAnalysis, 1);
};

#endif
