#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"

#include "MyAnalysis/MyFirstxAODAnalysis.h"

int main(int argc, char* argv[])
{
  // Take the submit directory from the input if provided:
  std::string submitDir = "submitDir";
  if(argc > 1) submitDir = argv[1];

  // Setup the job for xAOD access
  xAOD::Init().ignore();

  // Create a new sample handler to describe the data files we use
  SH::SampleHandler sh;

  // Scan for datasets in the given directory
  SH::scanDir (sh, "/afs/cern.ch/atlas/project/PAT/xAODs/r5534/");
  // or for data
  //SH::scanDir (sh, "/afs/cern.ch/atlas/project/PAT/xAODs/24.04.2014/");

  // Set the name of the tree in our files
  sh.setMetaString ("nc_tree", "CollectionTree");

  // Further sample handler configuration may go here

  // Print out the samples we found
  sh.print ();

  // The basic description of our job
  EL::Job job;
  job.sampleHandler (sh);

  // Instantiate our algorithm
  MyFirstxAODAnalysis* alg = new MyFirstxAODAnalysis;

  // Later on we'll add some configuration options for our algorithm that go here

  // Add our algorithm to the job
  job.algsAdd(alg);

  // Make the driver we want to use
  // This one works by running the algorithm directly:
  EL::DirectDriver driver;

  // Process the job using the driver
  driver.submit(job, submitDir);

  return 0;
} 
