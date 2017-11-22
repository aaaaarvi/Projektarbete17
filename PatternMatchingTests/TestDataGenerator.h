/*
 * TestDataGenerator.h
 *
 *  Created on: Nov 2, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_PATTERNMATCHINGTESTS_TESTDATAGENERATOR_H_
#define PNDTRACKERS_PATTERNMATCHINGTESTS_TESTDATAGENERATOR_H_

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


class TestDataGenerator: public FairTask {
public:
  TestDataGenerator();
  virtual ~TestDataGenerator();

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
  
  // output
  TFile *outputFile;
  TTree *tTrackPatterns;
  TString outputFileName;
  
  // csv file
  std::ofstream csvFile;

  // tree members
  Pattern *bPattern;
  
  ClassDef(TestDataGenerator,1)

};

#endif /* PNDTRACKERS_PATTERNMATCHINGTESTS_TESTCLASS_H_ */
