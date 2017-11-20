/*
 * PatternMatcher.cxx
 *
 *  Created on: Jun 16, 2017
 *      Author: vagrant
 */

#include "PatternMatcher.h"

#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <PndSttHit.h>

#include "Pattern.h"
#include <TTreeReaderValue.h>
#include <algorithm>


ClassImp(PatternMatcher)

PatternMatcher::PatternMatcher() {
  dbFile = NULL;
  patternTree = NULL;
//  treeReader = NULL;
  fSttParameters = NULL;
  fEventHeader = NULL;
  fSTTHitArray = NULL;
}

PatternMatcher::~PatternMatcher() {
}

void PatternMatcher::SetParContainers() {
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}
InitStatus PatternMatcher::Init() {
  LoadPatternDB("patternDB.root");

// Get the running instance of the FairRootManager to access tree branches
  FairRootManager *ioman = FairRootManager::Instance();
  if(!ioman) {
    std::cout << "-E- PatternDBGenerator::Init: FairRootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    std::cout << "-E- PatternDBGenerator:Init: EventHeader not instantiated!" << std::endl;
  }

// Access the STTHit branch
  fSTTHitArray = (TClonesArray*) ioman->GetObject("STTHit");


  return kSUCCESS;
}
void PatternMatcher::Exec(Option_t *opt) {
  FindMatch();
}
void PatternMatcher::FinishTask() {

}

void PatternMatcher::FindMatch() {
  int nSttHits = fSTTHitArray->GetEntriesFast();
  std::set<int> tubeIDs;
  for (int iHit = 0; iHit < nSttHits; ++iHit) {
    PndSttHit *sttHit = (PndSttHit*) fSTTHitArray->At(iHit);
    int tubeID = sttHit->GetTubeID();
    tubeIDs.insert(tubeID);
  }

//  std::cout << dbTubeIDs.at(1).size() << std::endl;


  int matches = 0;
  for (auto const& dbPattern : dbTubeIDs) {
    std::set<int> intersection;
    intersection.clear();
    std::set_intersection(tubeIDs.begin(), tubeIDs.end(), dbPattern.begin(), dbPattern.end(), std::inserter(intersection,intersection.begin()));
    float matchRatio = (float)intersection.size() / (float)dbPattern.size();
    if (matchRatio > 0.5) {
      std::cout<< "match ratio: " << matchRatio << std::endl;
      matches++;

      if (intersection.size() > 0) {
        std::cout << "matching tubes: ";
        for (auto const& tube : intersection) {
          std::cout << tube << " ";
        }
        std::cout << std::endl;
      }
    }
  }
  std::cout << "partial matches found: " << matches << std::endl;
}
void PatternMatcher::LoadPatternDB(TString filename) {
  dbFile = new TFile(filename,"READ");
  patternTree = (TTree*) dbFile->Get("trackPatterns");
//  TTreeReader reader("trackPatterns",dbFile);

  dbTubeIDs.clear();

  Pattern *pattern;
  patternTree->SetBranchAddress("pattern", &pattern);

  Long64_t nPatterns = patternTree->GetEntriesFast();
//  TTreeReaderValue<Pattern> pattern(reader,"pattern");
  for (Long64_t iEntry = 0; iEntry < nPatterns; ++iEntry) {
    patternTree->GetEntry(iEntry);
    std::set<int> tubeIDs = pattern->GetTubeIDs();
//    std::cout << pattern->GetSectorId() << std::endl;
    dbTubeIDs.push_back(tubeIDs);
  }
  std::cout << "# DB patterns: " << dbTubeIDs.size() << std::endl;

}
