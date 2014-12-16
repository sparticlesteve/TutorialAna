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

    // Standard constructor
    MyFirstxAODAnalysis();

    // Algorithm methods
    virtual EL::StatusCode setupJob(EL::Job& job);
    virtual EL::StatusCode fileExecute();
    virtual EL::StatusCode histInitialize();
    virtual EL::StatusCode changeInput(bool firstFile);
    virtual EL::StatusCode initialize();
    virtual EL::StatusCode execute();
    virtual EL::StatusCode postExecute();
    virtual EL::StatusCode finalize();
    virtual EL::StatusCode histFinalize();


    // Public configuration variables
    float lepPtMin;
    float jetPtMin;

    ClassDef(MyFirstxAODAnalysis, 1);

  protected:

    xAOD::TEvent* m_event; //!

    #ifndef __CINT__
    JetCleaningTool* m_jetCleaning; //!
    #endif

    int m_eventCounter; //!
    int m_numCleanEvents; //!

    TH1* h_jetPt; //!
    TH1* h_mJJ; //!

};

#endif
