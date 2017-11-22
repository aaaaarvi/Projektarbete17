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


ClassImp(PatDataGenerator)

PatDataGenerator::PatDataGenerator() {
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
  dataFileName = "./dataPAT.csv";
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
  csvFile.open(dataFileName);
  
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
  //And the FTSHits branch
  fFTSHitArray = (TClonesArray*) ioman->GetObject("FTSHit");
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
  PndMCTrack *mcTrack;
  PndSttHit *sttHit;
  PndFtsHit *ftsHit;
  // Get FairRootManager instance to access objects through FairLinks
  FairRootManager *ioman = FairRootManager::Instance();
  
  // Get the number of track candidates
  int nTrackCands = trackCands->GetEntriesFast();
  
  //csvFile << nTrackCands << "\n";
  
  int nTubesSTT = fSTTHitArray->GetEntriesFast();
  int nTubesFTS = fFTSHitArray->GetEntriesFast();
  int nTracks = fMCTrackArray->GetEntriesFast();
  // Write the momenta of the particles
  PndTrackCand *cand;
  FairMultiLinkedData sttLinks;
    // Print the number of tube hits
  csvFile << nTubesSTT;
  
  int nTubes = 0;
  
  for (int iTrack = 0; iTrack < nTrackCands; ++iTrack) {
    cand = (PndTrackCand*) (trackCands->At(iTrack));
    FairMultiLinkedData mcTrackLinks = cand->GetLinksWithType(mcTrackID);
    FairLink mcTrackLink = mcTrackLinks.GetLink(0);
    mcTrack = (PndMCTrack*) (ioman->GetCloneOfLinkData(mcTrackLink));

    if (mcTrack->GetPdgCode()==2212 && mcTrack->GetMotherID() == -1){
    sttLinks = cand->GetLinksWithType(sttBranchID);
    nTubes += sttLinks.GetNLinks();
  }
  }
  csvFile << ", " << nTubes;
  
  // Print the STT tube IDs
  for (int iTube = 0; iTube < nTubesSTT; ++iTube) {
    sttHit = (PndSttHit*) (fSTTHitArray->At(iTube));
    csvFile << "," << sttHit->GetTubeID();
  }
  
  
  // Loop over all track candidates and write the tube IDs into the file
  for (int iTrack = 0; iTrack < nTrackCands; ++iTrack) {
    
    // Get the next track candidate
    cand = (PndTrackCand*) (trackCands->At(iTrack));
    
    FairMultiLinkedData mcTrackLinks = cand->GetLinksWithType(mcTrackID);
    FairLink mcTrackLink = mcTrackLinks.GetLink(0);
    mcTrack = (PndMCTrack*) (ioman->GetCloneOfLinkData(mcTrackLink));
    
    // Get the FairLinks from the candidate pointing at STTHit branch
    sttLinks = cand->GetLinksWithType(sttBranchID);
    
    // Write the tube IDs to the csv file
    //csvFile << ", p";
    if (mcTrack->GetPdgCode()==2212 && mcTrack->GetMotherID() == -1){
    for (int iLink = 0; iLink < sttLinks.GetNLinks(); ++iLink) {
      
      FairLink sttLink = sttLinks.GetLink(iLink);
      sttHit = (PndSttHit*) (ioman->GetCloneOfLinkData(sttLink));
      
      csvFile << ", p" << sttHit->GetTubeID();
    }
   }
  }
  
  csvFile << "\n";
}


void PatDataGenerator::FinishTask() {
  // close csv
  csvFile.close();
}


void PatDataGenerator::SetFileName(TString name) {
  dataFileName = name;
}







