#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <EventLoop/OutputStream.h>
#include <TutorialAna/MyFirstxAODAnalysis.h>

#include "JetSelectorTools/JetCleaningTool.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

const double GeV = 1000.;

// this is needed to distribute the algorithm to the workers
ClassImp(MyFirstxAODAnalysis)



MyFirstxAODAnalysis :: MyFirstxAODAnalysis ()
    : lepPtMin(20.*GeV),
      jetPtMin(20.*GeV)
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}



EL::StatusCode MyFirstxAODAnalysis :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.
  job.useXAOD();

  // Initialize the algo to use xAODRootAccess
  xAOD::Init("MyFirstxAODAnalysis").ignore();

  // Output stream
  EL::OutputStream out("outputLabel");
  job.outputAdd(out);

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyFirstxAODAnalysis :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  h_jetPt = new TH1F("h_jetPt", "h_jetPt", 100, 0, 500);
  h_mJJ = new TH1F("h_mJJ", "h_mJJ;m_jj [GeV];Jet pairs", 100, 0, 200);
  wk()->addOutput(h_jetPt);
  wk()->addOutput(h_mJJ);

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyFirstxAODAnalysis :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyFirstxAODAnalysis :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyFirstxAODAnalysis :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  m_event = wk()->xaodEvent();

  // Initialize and configure jet tool
  m_jetCleaning = new JetCleaningTool("JetCleaning");
  m_jetCleaning->msg().setLevel(MSG::DEBUG);
  if(m_jetCleaning->setProperty("CutLevel", "MediumBad").isFailure())
    return EL::StatusCode::FAILURE;
  if(m_jetCleaning->initialize().isFailure())
    return EL::StatusCode::FAILURE;

  // Output xAOD
  TFile* file = wk()->getOutputFile("outputLabel");
  if(m_event->writeTo(file).isFailure())
    return EL::StatusCode::FAILURE;

  m_eventCounter = 0;
  m_numCleanEvents = 0;

  Info("initialize()", "Number of events = %lli", m_event->getEntries());

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyFirstxAODAnalysis :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  // Print every 100 events
  if((m_eventCounter % 100) == 0) Info("execute()", "Event number = %i", m_eventCounter);
  m_eventCounter++;

  // Event information
  const xAOD::EventInfo* eventInfo = 0;
  if(!m_event->retrieve(eventInfo, "EventInfo").isSuccess()){
    Error("execute()", "Failed to retrieve event info collection. Exiting.");
    return EL::StatusCode::FAILURE;
  }

  // Check if event is data or MC
  bool isMC = false;
  if(eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION)){
    isMC = true;
  }

  // Apply some event cleaning
  if(!isMC){
    if(eventInfo->eventFlags(xAOD::EventInfo::LAr)==2 ||
       eventInfo->eventFlags(xAOD::EventInfo::Tile)==2 ||
       eventInfo->eventFlags(xAOD::EventInfo::Core)!=0){
      return EL::StatusCode::SUCCESS;
    }
  }
  m_numCleanEvents++;

  // Get input electron container
  const xAOD::ElectronContainer* electrons = 0;
  if(!m_event->retrieve(electrons, "ElectronCollection").isSuccess()){
    Error("execute()", "Failed to retrieve ElectronCollection");
    return EL::StatusCode::FAILURE;
  }
  Info("execute()", " number of elecs = %lu", electrons->size());

  // Get input muon container
  const xAOD::MuonContainer* muons = 0;
  if(!m_event->retrieve(muons, "Muons").isSuccess()){
    Error("execute()", "Failed to retrieve Muons");
    return EL::StatusCode::FAILURE;
  }
  Info("execute()", " number of muons = %lu", muons->size());

  // Get input jet container
  const xAOD::JetContainer* jets = 0;
  if(!m_event->retrieve(jets, "AntiKt4LCTopoJets").isSuccess()){
    Error("execute()", "Failed to retrieve AntiKt4LCTopoJets container");
    return EL::StatusCode::FAILURE;
  }
  Info("execute()", " number of jets = %lu", jets->size());

  // Count the number of good electrons
  int numGoodEle = 0;
  xAOD::ElectronContainer::const_iterator eleItr;
  for(eleItr = electrons->begin(); eleItr != electrons->end(); ++eleItr){
    // Select good electrons
    if((*eleItr)->pt() < lepPtMin) continue;
    if(fabs((*eleItr)->eta()) < 2.47) continue;
    numGoodEle++;
  }
  Info("execute()", "numGoodEle = %i", numGoodEle);

  // Count the number of good muons
  int numGoodMu = 0;
  xAOD::MuonContainer::const_iterator muItr;
  for(muItr = muons->begin(); muItr != muons->end(); ++muItr){
    // Select good muons
    if((*muItr)->pt() < lepPtMin) continue;
    if(fabs((*muItr)->eta()) < 2.47) continue;
    numGoodMu++;
  }
  Info("execute()", "numGoodMu = %i", numGoodMu);

  // Create the output jet container and its aux store
  xAOD::JetContainer* goodJets = new xAOD::JetContainer();
  xAOD::JetAuxContainer* goodJetsAux = new xAOD::JetAuxContainer();
  goodJets->setStore(goodJetsAux);

  // Loop over jets in the container
  int numGoodJets = 0;
  xAOD::JetContainer::const_iterator jetItr;
  for(jetItr = jets->begin(); jetItr != jets->end(); ++jetItr){
    // Select good jets
    if(!m_jetCleaning->accept(**jetItr)) continue;
    if((*jetItr)->pt() < jetPtMin) continue;
    if(fabs((*jetItr)->eta()) > 2.5) continue;
    numGoodJets++;
    Info("execute()", "  jet pt = %.2f GeV", (*jetItr)->pt() * 0.001);

    // Fill histogram
    h_jetPt->Fill(((*jetItr)->pt())*.001);

    // Copy this jet to the output container
    xAOD::Jet* jet = new xAOD::Jet();
    jet->makePrivateStore(**jetItr);
    goodJets->push_back(jet);
  }
  Info("execute()", "numGoodJets = %i", numGoodJets);

  // Select events for l+jets
  if(numGoodEle + numGoodMu < 1) return EL::StatusCode::SUCCESS;
  if(numGoodJets < 3) return EL::StatusCode::SUCCESS;

  // Fill the dijet mass plot
  xAOD::JetContainer::const_iterator jetItr2;
  for(jetItr = goodJets->begin(); jetItr != goodJets->end(); ++jetItr){
    for(jetItr2 = jetItr+1; jetItr2 != goodJets->end(); ++jetItr2){
      // Calculate di-jet mass
      float mJJ = ((*jetItr)->p4() + (*jetItr2)->p4()).M();
      // Fill the histogram
      h_mJJ->Fill(mJJ*.001);
    }
  }

  // Copy full jet container to output
  if(m_event->copy("AntiKt4LCTopoJets").isFailure())
    return EL::StatusCode::FAILURE;

  // Record the good jets into the output
  if(!m_event->record(goodJets, "GoodJets")){
    Error("execute()", "record good jets failed");
  }
  if(!m_event->record(goodJetsAux, "GoodJetsAux.")){
    Error("execute()", "record good jets aux failed");
  }

  // Save the event
  m_event->fill();

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyFirstxAODAnalysis :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyFirstxAODAnalysis :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  if(m_jetCleaning) delete m_jetCleaning;

  // Finalize and close our output xAOD file
  TFile* file = wk()->getOutputFile("outputLabel");
  if(m_event->finishWritingTo(file).isFailure())
    return EL::StatusCode::FAILURE;

  Info("finalize()", "Number of clean events = %i", m_numCleanEvents);

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyFirstxAODAnalysis :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  return EL::StatusCode::SUCCESS;
}
