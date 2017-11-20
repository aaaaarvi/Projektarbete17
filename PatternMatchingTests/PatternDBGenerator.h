/*
 * PatternDBGenerator.h
 *
 *  Created on: Jun 12, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNDBGENERATOR_H_
#define PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNDBGENERATOR_H_

#include <FairTask.h>
#include <PndGeoSttPar.h>
#include <PndMCTrack.h>
#include <PndSttHit.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TTree.h>
#include <Pattern.h>

typedef std::vector<PndSttHit*> HitArray;
typedef std::multimap<int, int> HitMap;
typedef std::map<int,Double_t> TimeStamps;

class PatternDBGenerator : public FairTask {
public:
  PatternDBGenerator();
  virtual ~PatternDBGenerator();

  void addPatternEntry(Pattern pattern); // DEPRECATED, remove soon
  void write();

  void setOutputFile(TString filename = "patternDB.root") {outputFileName = filename;}


protected:
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void FinishTask();

private:
  void GenerateTrackPatterns();
  HitMap FillHitMap(HitArray sttHitArray);
  TimeStamps FillTimeStamps(HitArray sttHitArray);
  std::vector<Pattern> FillPatterns(HitArray array, PndMCTrack* mcTrack);

  void mergeIdenticalPatterns(); // DEPRECATED
  void mergeSimilarPatterns(float matchingRatio); // DEPRECATED
  Pattern mergeEntries(std::vector<Pattern> &entries); // DEPRECATED

  void DrawStt(bool isskew, bool onlyhits);

//  output
  TFile *outputFile;
  TTree *tTrackPatterns;
  TString outputFileName;

//  tree members
  Pattern *bPattern;

//  Input data
  PndGeoSttPar *fSttParameters;
  TClonesArray *fEventHeader;
  TClonesArray *fTubeArray;
  TClonesArray *fSTTHitArray;
  TClonesArray *fMCTrackArray;
  TClonesArray *trackCands;
  TClonesArray *idealTracks;
  TClonesArray *genTracks;
  int sttBranchID;
  int mcTrackID;

  TCanvas *SttXYproj;
  std::set<int> hitsDraw;


  ClassDef(PatternDBGenerator,1)

};

#endif /* PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNDBGENERATOR_H_ */
