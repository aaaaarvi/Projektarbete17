/*
 * PatDataGenerator.cxx
 *
 *  Created on: Nov 2, 2017
 *      Author: vagrant
 */

#include "PatDataGenerator.h"

#include <FairMultiLinkedData.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <PndSttMapCreator.h>
#include <PndSttTube.h>
#include <PndTrackCand.h>
#include <boost/range/adaptor/reversed.hpp>
#include <TArc.h>
#include <TArrow.h>
#include <TLine.h>
#include <TText.h>
#include <TMath.h>

#include <iostream>
#include <fstream>


ClassImp(PatDataGenerator)

PatDataGenerator::PatDataGenerator() {
  fSttParameters = NULL;
  fEventHeader = NULL;
  fTubeArray = NULL;
  fSTTHitArray = NULL;
  fMCTrackArray = NULL;
  trackCands = NULL;
  idealTracks = NULL;
  genTracks = NULL;
  sttBranchID = -1;
  mcTrackID = -1;
}

PatDataGenerator::~PatDataGenerator() {
  // TODO Auto-generated destructor stub
}

void PatDataGenerator::SetParContainers() {
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus PatDataGenerator::Init() {
  
  // Create file
  csvFile.open("data.csv");
  
  // Get the running instance of the FairRootManager to access tree branches
  FairRootManager *ioman = FairRootManager::Instance();
  if(!ioman) {
    std::cout << "-E- PatternDBGenerator::Init: FairRootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Event header info
  fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    std::cout << "-E- PatternDBGenerator:Init: EventHeader not instantiated!" << std::endl;
  }
  
  // Access the STTHit branch
  fSTTHitArray = (TClonesArray*) ioman->GetObject("STTHit");
  
  // Initialize the STT map and get an array of all tubes
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();
  
  // Get the ID of the STTHit branch for accessing the FairLinks later
  sttBranchID = ioman->GetBranchId("STTHit");
  if (sttBranchID == -1) {
    std::cout << "-E- PatternDBGenerator:Init: STTHit branch not found!" << std::endl;
  }
  
  // Access the MCTrack branch
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");
  
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
  
  // Get the ideal tracks from the ideal track finder
  idealTracks = (TClonesArray*) ioman->GetObject("SttMvdGemIdealTrack");
  if (!idealTracks) {
    std::cout << "-E- PatternDBGenerator:Init: SttMvdGemIdealTrack not found!" << std::endl;
  }
  
  // Get the gen tracks from the ideal track finder
  genTracks = (TClonesArray*) ioman->GetObject("SttMvdGemGenTrack");
  if (!genTracks) {
    std::cout << "-E- PatternDBGenerator:Init: SttMvdGemGenTrack not found!" << std::endl;
  }
  
  // Initialization successful
  std::cout << "-I- PatternDBGenerator: Initialisation successful" << std::endl;
  return kSUCCESS;
}


void PatDataGenerator::Exec(Option_t* opt) {
  
  // Get FairRootManager instance to access objects through FairLinks
  FairRootManager *ioman = FairRootManager::Instance();
  
  // Get the number of track candidates
  int nTrackCands = trackCands->GetEntriesFast();
  
  csvFile << nTrackCands << "\n";
  
  // Write the momenta of the particles
  PndTrackCand *cand;
  FairMultiLinkedData sttLinks;
  for (int iTrackCand = 0; iTrackCand < nTrackCands; ++iTrackCand) {
    
    // Get the next track candidate
    cand = (PndTrackCand*) (trackCands->At(iTrackCand));
    
    // Get link to corresponding MC track
    FairMultiLinkedData mcTrackLinks = cand->GetLinksWithType(mcTrackID);
    
    // Check if only one MC track was found
    if (mcTrackLinks.GetNLinks() == 0) {
      std::cout << "WARNING: PatternDBGenerator:GenerateTrackPatterns: No MCTrack found, skipping event!" << std::endl;
      continue;
    }
    if (mcTrackLinks.GetNLinks() > 1) {
      std::cout << "WARNING: PatternDBGenerator:GenerateTrackPatterns: Found more than one MCTrack link!!!" << std::endl;
    }
    
    // Get only first link since only one should be associated with track candidate
    FairLink mcTrackLink = mcTrackLinks.GetLink(0);
    PndMCTrack *mcTrack = (PndMCTrack*) (ioman->GetCloneOfLinkData(mcTrackLink));
    
    // Write the momentum into the csv file
    TVector3 momentum = mcTrack->GetMomentum();
    csvFile << momentum.Px() << ", " << momentum.Py() << ", " << mcTrack->GetPdgCode();
    csvFile << "\n";
  }
  csvFile << "\n";
  
  // Compute the total number of tubes
  int nTubes = 0;
  for (int iTrackCand = 0; iTrackCand < nTrackCands; ++iTrackCand) {
    cand = (PndTrackCand*) (trackCands->At(iTrackCand));
    sttLinks = cand->GetLinksWithType(sttBranchID);
    nTubes += sttLinks.GetNLinks();
  }
  //csvFile << ", " << nTubes;
  
  // Loop over all track candidates and write the tube IDs into the file
  for (int iTrackCand = 0; iTrackCand < nTrackCands; ++iTrackCand) {
    
    // Get the next track candidate
    cand = (PndTrackCand*) (trackCands->At(iTrackCand));
    
    // Get the FairLinks from the candidate pointing at STTHit branch
    sttLinks = cand->GetLinksWithType(sttBranchID);
    
    // Write the tube IDs to the csv file
    for (int iLink = 0; iLink < sttLinks.GetNLinks(); ++iLink) {
      FairLink sttLink = sttLinks.GetLink(iLink);
      PndSttHit *sttHit = (PndSttHit*) (ioman->GetCloneOfLinkData(sttLink));
      //csvFile << ", " << sttHit->GetTubeID();
    }
    
  }
  
  csvFile << "\n";
}


void PatDataGenerator::FinishTask() {
  // close csv
  csvFile.close();
}










