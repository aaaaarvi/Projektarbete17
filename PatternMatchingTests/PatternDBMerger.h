/*
 * PatternDBMerger.h
 *
 *  Created on: Jun 26, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNDBMERGER_H_
#define PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNDBMERGER_H_
#include <TCanvas.h>
#include <TChain.h>
#include <TFile.h>
#include <TGraph.h>
#include <TString.h>
#include <TTree.h>

#include "Pattern.h"

//  number of identical entries, actual entries in file
typedef std::multimap<int, std::set<int>> IdenticalEntries;

class PatternDBMerger {
public:
  PatternDBMerger();
  virtual ~PatternDBMerger();

  void SetInputFilename(TString filename) {inputFilename = filename;}
  void SetInputTreename(TString treename) {inputTreename = treename;}
  void SetOutputFileName(TString filename) {outputFilename = filename;}
  void SetOutputTreename(TString treename) {outputTreename = treename;}

  void Execute();

private:
  bool InitInputChain();
  void InitOutputTree();
  IdenticalEntries FindIdenticalPatterns();
  std::vector<Pattern> GetPatternsForMerging(std::pair<int, std::set<int>> entryPair);
  Pattern MergePatterns(std::vector<Pattern> &entries);
  void FillOutputTree(Pattern &mergedPattern);


  TChain *patternTree;
  TString inputFilename;
  TFile *outputDBFile;
  TString outputFilename;
  TTree *outputTree;

//  TCanvas *canEntries;
  TGraph *grEntries;
  TGraph *grEntriesSim;

  TString inputTreename;
  TString outputTreename;

  Pattern *bPattern;
  Pattern *bMergedPattern;

  ClassDef(PatternDBMerger,1)
};

#endif /* PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNDBMERGER_H_ */
