/*
 * TestTubeGenerator.h
 *
 *  Created on: Nov 2, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_PATTERNMATCHINGTESTS_TESTTUBEGENERATOR_H_
#define PNDTRACKERS_PATTERNMATCHINGTESTS_TESTTUBEGENERATOR_H_

#include <FairTask.h>
#include <PndGeoSttPar.h>
#include <PndGeoFtsPar.h>
#include <PndMCTrack.h>
#include <PndSttHit.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TTree.h>
#include <Pattern.h>

#include <iostream>
#include <fstream>


class TestTubeGenerator: public FairTask {
public:
  TestTubeGenerator();
  virtual ~TestTubeGenerator();

protected:
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void FinishTask();

private:
  
  // input
  PndGeoSttPar *fSttParameters;
  PndGeoFtsPar *fFtsParameters;
  TClonesArray *fSTTHitArray;
  TClonesArray *fFTSHitArray;
  TClonesArray *fMCTrackArray;
  TClonesArray *fTubeArray;
  
  // output
  TFile *outputFile;
  TTree *tTrackPatterns;
  TString outputFileName;
  
  // csv file
  std::ofstream csvFile;

  // tree members
  Pattern *bPattern;
  
  ClassDef(TestTubeGenerator,1)

};

#endif /* PNDTRACKERS_PATTERNMATCHINGTESTS_TESTCLASS_H_ */
