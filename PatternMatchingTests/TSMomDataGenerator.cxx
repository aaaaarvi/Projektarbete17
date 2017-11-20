/*
 * TSMomDataGenerator.cxx
 *
 *  Created on: Nov 2, 2017
 *      Author: vagrant
 */

#include "TSMomDataGenerator.h"

#include <FairMultiLinkedData.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <PndSttMapCreator.h>
#include <PndSttTube.h>
#include <PndSttHit.h>
#include <PndFtsHit.h>
#include <PndTrackCand.h>
#include <PndTrack.h>
#include <boost/range/adaptor/reversed.hpp>
#include <TArc.h>
#include <TArrow.h>
#include <TLine.h>
#include <TText.h>
#include <TMath.h>

#include <iostream>
#include <fstream>


ClassImp(TSMomDataGenerator)

TSMomDataGenerator::TSMomDataGenerator() {
  fSttParameters = NULL;
  fSTTHitArray = NULL;
  fFTSHitArray = NULL;
  fMCTrackArray = NULL;
  suffix = "";
}

TSMomDataGenerator::~TSMomDataGenerator() {
  // TODO Auto-generated destructor stub
}

void TSMomDataGenerator::SetParContainers() {
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus TSMomDataGenerator::Init() {
  
  // Create file
  TString dataFileName = "data/data" + suffix + ".csv";
  csvFile.open(dataFileName);
  
  // Get the running instance of the FairRootManager to access tree branches
  FairRootManager *ioman = FairRootManager::Instance();
  if(!ioman) {
    std::cout << "-E- PatternDBGenerator::Init: FairRootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Access hit arrays
  fSTTHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  fFTSHitArray = (TClonesArray*) ioman->GetObject("FTSHit");
  
  // Access the MCTrack branch
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  
  // Initialization successful
  std::cout << "-I- PatternDBGenerator: Initialisation successful" << std::endl;
  return kSUCCESS;
}


void TSMomDataGenerator::Exec(Option_t* opt) {
  PndMCTrack *mcTrack;
  PndSttHit *sttHit;
  PndFtsHit *ftsHit;
  TVector3 momentum;
  int mID, pdg;
  
  // Get FairRootManager instance to access objects through FairLinks
  FairRootManager *ioman = FairRootManager::Instance();
  
  // Get the number of MC tracks and tube hits
  int nTracks = fMCTrackArray->GetEntriesFast();
  int nTubesSTT = fSTTHitArray->GetEntriesFast();
  int nTubesFTS = fFTSHitArray->GetEntriesFast();
  
  // Count the number of particles and write to file
  int nParticles = 0;
  for (int iTrack = 0; iTrack < nTracks; ++iTrack) {
    mcTrack = (PndMCTrack*) (fMCTrackArray->At(iTrack));
    mID = mcTrack->GetMotherID();
    pdg = mcTrack->GetPdgCode();
    if (mID == -1 && (pdg == 2212 || pdg == -2212 || pdg == 211 || pdg == -211)) {
      nParticles++;
    }
  }
  csvFile << nParticles;
  
  // Print the momenta of the particles
  for (int iTrack = 0; iTrack < nTracks; ++iTrack) {
    mcTrack = (PndMCTrack*) (fMCTrackArray->At(iTrack));
    mID = mcTrack->GetMotherID();
    pdg = mcTrack->GetPdgCode();
    if (mID == -1 && (pdg == 2212 || pdg == -2212 || pdg == 211 || pdg == -211)) {
      momentum = mcTrack->GetMomentum();
      csvFile << "," << pdg << "," << momentum.Px() << "," << momentum.Py();
    }
  }
  
  // Print the number of tube hits
  csvFile << "," << nTubesSTT;
  csvFile << "," << nTubesFTS;
  
  // Print the STT tube IDs
  for (int iTube = 0; iTube < nTubesSTT; ++iTube) {
    sttHit = (PndSttHit*) (fSTTHitArray->At(iTube));
    csvFile << "," << sttHit->GetTubeID() << "," << sttHit->GetTimeStamp();
  }
  
  // Print the FTS tube IDs
  for (int iTube = 0; iTube < nTubesFTS; ++iTube) {
    ftsHit = (PndFtsHit*) (fFTSHitArray->At(iTube));
    csvFile << "," << ftsHit->GetTubeID();
  }
  
  csvFile << "\n";
}


void TSMomDataGenerator::FinishTask() {
  // close csv
  csvFile.close();
}


void TSMomDataGenerator::SetSuffix(TString suff) {
  suffix = suff;
}







