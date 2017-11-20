/*
 * PatternDBMerger.cxx
 *
 *  Created on: Jun 26, 2017
 *      Author: vagrant
 */

#include "PatternDBMerger.h"
#include "boost/range/adaptor/reversed.hpp"
#include <iostream>

#include <TChain.h>
#include <TTree.h>
ClassImp(PatternDBMerger)

PatternDBMerger::PatternDBMerger() {
  patternTree = NULL;
  outputDBFile = NULL;
  outputTree = NULL;
  bPattern = NULL;
  bMergedPattern = NULL;

  grEntries = NULL;
  grEntriesSim = NULL;

  inputFilename = "patternDB.root";
  inputTreename = "trackPatterns";
  outputFilename = "patternDBclean.root";
  outputTreename = inputTreename;

}

PatternDBMerger::~PatternDBMerger() {
}

void PatternDBMerger::Execute() {
  std::cout << "Executing PatternDBMerger" << std::endl;

  bool bInput = InitInputChain();
  if (bInput) {
    std::cout << "Input chain successfully initialised" << std::endl;
  }

  InitOutputTree();

  IdenticalEntries entriesToBeMerged;
  entriesToBeMerged = FindIdenticalPatterns();

  std::cout << entriesToBeMerged.size() << " merged entries to be created" << std::endl;
  for (auto iter : boost::adaptors::reverse(entriesToBeMerged)) {
    std::vector<Pattern> patternsToBeMerged = GetPatternsForMerging(iter);
    Pattern mergedPattern = MergePatterns(patternsToBeMerged);
    FillOutputTree(mergedPattern);
  }

  std::cout << "Saving output tree" << std::endl;
  outputTree->AutoSave();
  outputDBFile->Close();
}

bool PatternDBMerger::InitInputChain() {
  std::cout << "Initialising input chain with TTree " << inputTreename << std::endl;
  patternTree = new TChain(inputTreename);
  std::cout << "Adding input file(s) with name " << inputFilename << std::endl;
  patternTree->Add(inputFilename);

  patternTree->SetBranchAddress("pattern",&bPattern);

  if (!patternTree) return false;
  return true;
}
void PatternDBMerger::InitOutputTree() {
  std::cout << "Initialising output file and tree" << std::endl;
  outputDBFile = new TFile(outputFilename,"RECREATE");
  outputTree = new TTree(outputTreename,"Tree containing merged patterns");
  outputTree->Branch("pattern",&bMergedPattern);

//  canEntries = new TCanvas();
//  grEntries = new TGraph();
}
IdenticalEntries PatternDBMerger::FindIdenticalPatterns() {
  std::cout << "Finding identical patterns in input chain" << std::endl;
//  Pattern *pattern;
//  patternTree->SetBranchAddress("pattern",&pattern);
//  pattern, entries in file
  std::map<std::set<int>, std::set<int>> patternEntries;
  std::map<std::set<int>, std::set<int>> patternEntriesSimilar; // TEMPORARY
  float condSimilar = 0.9; // condition for similarity, TEMPORARY
  Long64_t nEntries = patternTree->GetEntries();
  std::cout << nEntries << " entries to process" << std::endl;
  std::vector<int> vEntry;
  std::vector<int> vSize;
  std::vector<int> vSizeSim; // TEMPORARY
  for (Long64_t iEntry = 0; iEntry < nEntries; ++iEntry) {
    if (iEntry % 10000 == 0) {
      std::cout << "processing entry " << iEntry << " of " << nEntries << std::endl;
    }
    patternTree->GetEntry(iEntry);
    std::set<int> tubeIDs = bPattern->GetTubeIDs();
    patternEntries[tubeIDs].insert(iEntry);

//    TEMPORARY
    if (patternEntriesSimilar.size() == 0) {
      patternEntriesSimilar[tubeIDs].insert(iEntry);
    }

//    TEMPORARY LOOP
    bool bInserted = false;
    if (patternEntriesSimilar.size() > 0) {
      for (auto iter : patternEntriesSimilar) {
        std::set<int> intersection;
        intersection.clear();
        std::set_intersection(iter.first.begin(), iter.first.end(), tubeIDs.begin(), tubeIDs.end(), std::inserter(intersection, intersection.begin()));
        float matchRatio = (float)intersection.size() / (float)iter.first.size();
        if (matchRatio >= condSimilar) {
          iter.second.insert(iEntry);
          bInserted = true;
        }
      }
      if (bInserted == false) {
        patternEntriesSimilar[tubeIDs].insert(iEntry);
      }
    }


    if ( iEntry % 100000 == 0 ) {
//      std::cout << iEntry << " " << patternEntries.size() << std::endl;
      vEntry.push_back(iEntry);
      vSize.push_back(patternEntries.size());
      vSizeSim.push_back(patternEntriesSimilar.size());
    }

  }

  Int_t n = vEntry.size();
  Int_t *x = vEntry.data();
  Int_t *y = vSize.data();
  Int_t *ySim = vSizeSim.data();

  grEntries = new TGraph(n,x,y);
//  grEntries->Draw("AP");
  grEntries->Write("Entries", TObject::kWriteDelete);

  grEntriesSim = new TGraph(n,x,ySim);
  grEntriesSim->Write("EntriesSim", TObject::kWriteDelete);

//  canEntries->Update();
//  canEntries->Write("", TObject::kWriteDelete);

  std::cout << patternEntries.size() << " unique patterns found in input chain" << std::endl;

  std::cout << "Creating map with entries to be merged" << std::endl;
//  number of entries, actual entries in file
  IdenticalEntries mapEntries;
  for (auto iter : patternEntries) {
    std::set<int> &vEntries = iter.second;
//    set this to 1 to only consider recurring patterns
    if (vEntries.size() > 0) {
      mapEntries.insert(std::make_pair(vEntries.size(), vEntries));
    }
  }

  return mapEntries;
//  for (auto iter : boost::adaptors::reverse(mapEntries)) {
//
//  }
}
std::vector<Pattern> PatternDBMerger::GetPatternsForMerging(std::pair<int, std::set<int>> entryPair) {
  std::vector<Pattern> patternsToBeMerged;
  for (auto const& entry : entryPair.second) {
    patternTree->GetEntry(entry);
    patternsToBeMerged.push_back(*bPattern);
  }

  return patternsToBeMerged;
}
Pattern PatternDBMerger::MergePatterns(std::vector<Pattern> &entries) {
  Pattern merged;
  if (entries.size() == 0) return merged;
  merged.SetTubeIDs(entries.at(0).GetTubeIDs());
  merged.setSectorID(entries.at(0).GetSectorId());

  for (auto const& pattern : entries) {
    int currentCount = merged.GetCount();
    int newCount = currentCount + pattern.GetCount();
    merged.setPatternCount(newCount);
    std::vector<TVector3> momenta = pattern.getMomenta();
    merged.addMomenta(momenta);
    std::vector<std::map<int, Double_t>> timeStamps = pattern.getTimeStamps();
    merged.addTimeStamps(timeStamps);
  }

  return merged;
}
void PatternDBMerger::FillOutputTree(Pattern &mergedPattern) {
  bMergedPattern = &mergedPattern;
  outputTree->Fill();
}
