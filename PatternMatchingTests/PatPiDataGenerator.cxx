/*
 * PatPiDataGenerator.cxx
 *
 *  Created on: Nov 2017
 *      Author: Adam Hedkvist & Arvi Jonnarth
 
 This file generates data that is written into a csv file. The data contains the number of STThits and all the tubeIDs of the STThits. 
 It also saves all the STThits from the final state pion. It is mainly used in CreateDataPat.m to preprocess the data before training
 it in a neural network.
 
 
 The data is ordered in the following way:
 
 NumberOfSttHits,NumberOfPionsSttHits,SttHit1,SttHit2 ... SttHitN, PionSttHit1, PionSttHit2 .. ProtonSttHitN
 
 
 */

#include "PatPiDataGenerator.h"

#include <FairMultiLinkedData.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <PndSttMapCreator.h>
#include <PndSttTube.h>
#include <PndTrackCand.h>
#include <PndFtsHit.h>
#include <PndSttHit.h>
#include <boost/range/adaptor/reversed.hpp>
#include <TArc.h>
#include <TArrow.h>
#include <TLine.h>
#include <TText.h>
#include <TMath.h>

#include <iostream>
#include <fstream>


ClassImp(PatPiDataGenerator)

PatPiDataGenerator::PatPiDataGenerator() {
  fSttParameters = NULL;
  fEventHeader = NULL;
  fTubeArray = NULL;
  fSTTHitArray = NULL;
  fFTSHitArray = NULL;
  fMCTrackArray = NULL;
  trackCands = NULL;
  idealTracks = NULL;
  genTracks = NULL;
  sttBranchID = -1;
  mcTrackID = -1;
  dataFileName = "./dataPATPi.csv";
}

PatPiDataGenerator::~PatPiDataGenerator() {
  // TODO Auto-generated destructor stub
}

void PatPiDataGenerator::SetParContainers() {
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus PatPiDataGenerator::Init() {
  
  // Create file
  csvFile.open(dataFileName);
  
  // Get the running instance of the FairRootManager to access tree branches
  FairRootManager *ioman = FairRootManager::Instance();
  if(!ioman) {
    std::cout << "-E- PatternDBGenerator::Init: FairRootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Access the STTHit branch
  fSTTHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  // Access MCTrack pointers
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  // Get the ID of the STTHit branch for accessing the FairLinks later
  sttBranchID = ioman->GetBranchId("STTHit");
  if (sttBranchID == -1) {
    std::cout << "-E- PatternDBGenerator:Init: STTHit branch not found!" << std::endl;
  }
  
  // Get the ID of the MCTrack branch for accessing through FairLinks later
  mcTrackID = ioman->GetBranchId("MCTrack");
  if (mcTrackID == -1) {
    std::cout << "-E- PatternDBGenerator:Init: MCTrack branch not found!" << std::endl;
  }
  
  // Get the track candidates from the ideal track finder
  trackCands = (TClonesArray*) ioman->GetObject("SttMvdGemIdealTrackCand");
  if (!trackCands) {
    std::cout << "-E- PatternDBGenerator:Init: SttMvdGemIdealTrackCand not fount!" << std::endl;
  }
  
  // Initialization successful
  std::cout << "-I- PatternDBGenerator: Initialisation successful" << std::endl;
  return kSUCCESS;
}


void PatPiDataGenerator::Exec(Option_t* opt) {
  PndMCTrack *mcTrack;
  PndSttHit *sttHit;
  PndFtsHit *ftsHit;
  PndTrackCand *cand;
  FairMultiLinkedData sttLinks;
  TVector3 momentum;
  
  // Get FairRootManager instance to access objects through FairLinks
  FairRootManager *ioman = FairRootManager::Instance();
  
  // Get the number of track candidates
  int nTrackCands = trackCands->GetEntriesFast();
  
  // Print the number of tube hits
  int nTubesSTT = fSTTHitArray->GetEntriesFast();
  csvFile << nTubesSTT;
  
  
  // Print the number of tube hits created by the final state proton
  int nTubes = 0;
  for (int iTrack = 0; iTrack < nTrackCands; ++iTrack) {
    cand = (PndTrackCand*) (trackCands->At(iTrack));
    FairMultiLinkedData mcTrackLinks = cand->GetLinksWithType(mcTrackID);
    FairLink mcTrackLink = mcTrackLinks.GetLink(0);
    mcTrack = (PndMCTrack*) (ioman->GetCloneOfLinkData(mcTrackLink));

    if (mcTrack->GetPdgCode() == -211 && mcTrack->GetMotherID() == -1) {
      sttLinks = cand->GetLinksWithType(sttBranchID);
      nTubes += sttLinks.GetNLinks();
      momentum = mcTrack->GetMomentum();
    }
  }
  csvFile << "," << nTubes << "," << momentum.Px() << "," << momentum.Py();
  
  // Print all the STT tube IDs
  for (int iTube = 0; iTube < nTubesSTT; ++iTube) {
    sttHit = (PndSttHit*) (fSTTHitArray->At(iTube));
    csvFile << "," << sttHit->GetTubeID();
  }
  
  // Loop over all track candidates and write the tube IDs associated with the final state proton
  for (int iTrack = 0; iTrack < nTrackCands; ++iTrack) {
    
    // Get the next track candidate
    cand = (PndTrackCand*) (trackCands->At(iTrack));
    
    // Get the MC track
    FairMultiLinkedData mcTrackLinks = cand->GetLinksWithType(mcTrackID);
    FairLink mcTrackLink = mcTrackLinks.GetLink(0);
    mcTrack = (PndMCTrack*) (ioman->GetCloneOfLinkData(mcTrackLink));
    
    // Get the FairLinks from the candidate pointing at STTHit branch
    sttLinks = cand->GetLinksWithType(sttBranchID);
    
    // Write the tube IDs to the csv file
    if (mcTrack->GetPdgCode() == -211 && mcTrack->GetMotherID() == -1) {
      for (int iLink = 0; iLink < sttLinks.GetNLinks(); ++iLink) {
        FairLink sttLink = sttLinks.GetLink(iLink);
        sttHit = (PndSttHit*) (ioman->GetCloneOfLinkData(sttLink));
        csvFile << "," << sttHit->GetTubeID();
      }
    }
  }
  
  csvFile << "\n";
}


void PatPiDataGenerator::FinishTask() {
  // close csv
  csvFile.close();
}


void PatPiDataGenerator::SetFileName(TString name) {
  dataFileName = name;
}







