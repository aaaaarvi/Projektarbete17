/*
 * PatternMatcher.h
 *
 *  Created on: Jun 16, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNMATCHER_H_
#define PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNMATCHER_H_

#include <FairTask.h>
#include <PndGeoSttPar.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TTree.h>

class PatternMatcher: public FairTask {
public:
  PatternMatcher();
  virtual ~PatternMatcher();

protected:
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask();

private:
  void LoadPatternDB(TString filename);
  void FindMatch();

//  Input data
  PndGeoSttPar *fSttParameters;
  TClonesArray *fEventHeader;
  TClonesArray *fSTTHitArray;

  TFile *dbFile;
  TTree *patternTree;
  std::vector<std::set<int>> dbTubeIDs;

  ClassDef(PatternMatcher,1)
};

#endif /* PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNMATCHER_H_ */
