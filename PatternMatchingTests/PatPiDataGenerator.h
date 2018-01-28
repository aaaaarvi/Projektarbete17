/*
 * PatPiDataGenerator.h
 *
 *  Created on: Nov 2, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_PATTERNMATCHINGTESTS_PATPIDATAGENERATOR_H_
#define PNDTRACKERS_PATTERNMATCHINGTESTS_PATPIDATAGENERATOR_H_

#include <FairTask.h>
#include <PndGeoSttPar.h>
#include <PndMCTrack.h>
#include <PndSttHit.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TTree.h>
#include <Pattern.h>

#include <iostream>
#include <fstream>


class PatPiDataGenerator: public FairTask {
public:
  PatPiDataGenerator();
  virtual ~PatPiDataGenerator();
  void SetFileName(TString name);

protected:
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void FinishTask();

private:
  
  // input
  PndGeoSttPar *fSttParameters;
  TClonesArray *fEventHeader;
  TClonesArray *fTubeArray;
  TClonesArray *fSTTHitArray;
  TClonesArray *fFTSHitArray;  
  TClonesArray *fMCTrackArray;
  TClonesArray *trackCands;
  TClonesArray *idealTracks;
  TClonesArray *genTracks;
  int sttBranchID;
  int mcTrackID;
  
  //  output
  TFile *outputFile;
  TTree *tTrackPatterns;
  TString outputFileName;
  
  // csv file
  std::ofstream csvFile;

  //  tree members
  Pattern *bPattern;
  
  // File name
  TString dataFileName;
  
  ClassDef(PatPiDataGenerator,1)

};

#endif /* PNDTRACKERS_PATTERNMATCHINGTESTS_TESTCLASS_H_ */
