/*
 * PatternCounter.cxx
 *
 *  Created on: May 19, 2017
 *      Author: vagrant
 */

#include "PatternCounter.h"

#include <FairMultiLinkedData.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
//#include <PndSttHit.h>
#include <PndSttMapCreator.h>
#include <PndSttTube.h>
#include <PndTrackCand.h>
#include <iostream>
//#include <map>
#include <set>
#include <boost/range/adaptor/reversed.hpp>
#include <FairEventHeader.h>

ClassImp(PatternCounter)

PatternCounter::PatternCounter() {
// initialise members
	fSttParameters = NULL;
	fTubeArray = NULL;
	fEventHeader = NULL;
	fSTTHitArray = NULL;
	trackCands = NULL;
	tracks = NULL;
	sttBranchID = -1;

// reserve vector space
	InvertedPatternMap map;
	PatternMap map2;
	for (int iSector = 0; iSector < 6; ++iSector) {
		eventSectorMaps.push_back(map);
		trackSectorMaps.push_back(map);
		tmpTrackMaps.push_back(map2);
	}


	bPattern = NULL;
	bMomenta = NULL;

//	outFile = new TFile("patternDB.root","recreate");

//	for (int iSector = 0; iSector < 6; ++iSector) {
//		TString cTreeName;
//		cTreeName.Form("patterTreeSector%i",iSector);
//		patternTree[iSector] = new TTree(cTreeName,"Pattern database");
//		patternTree[iSector]->Branch("pattern","std::set<int>",&bPattern);
//		patternTree[iSector]->Branch("momenta","std::vector<Double_t>",&bMomenta);
//
//	}
//	patternTree = new TTree("patternTree","Pattern database");
//	patternTree->Branch("patterns","std::set<int>",&pPattern);
}

PatternCounter::~PatternCounter() {
}

void PatternCounter::SetParContainers() {
	FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
	fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}

InitStatus PatternCounter::Init() {
	// Get the running instance of the FairRootManager to access tree branches
	FairRootManager *ioman = FairRootManager::Instance();
	if(!ioman) {
		std::cout << "-E- PatternCounter::Init: FairRootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fEventHeader = (TClonesArray*) ioman->GetObject("EventHeader.");
	if (!fEventHeader) {
		std::cout << "-E- PatternCounter:Init: EventHeader not instantiated!" << std::endl;
	}

// Access the STTHit branch
	fSTTHitArray = (TClonesArray*) ioman->GetObject("STTHit");

// Initialize the STT map and get an array of all tubes
	PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
	fTubeArray = mapper->FillTubeArray();

// Get the ID of the STTHit branch ofr accessing the FairLinks later
	sttBranchID = ioman->GetBranchId("STTHit");
	if (sttBranchID == -1) {
		std::cout << "-E- PatternCounter:Init: STTHit branch not found!" << std::endl;
	}

// Get the track candidates from the ideal track finder
	trackCands = (TClonesArray*) ioman->GetObject("SttMvdGemIdealTrackCand");
	if (!trackCands) {
		std::cout << "-E- PatternCounter:Init: SttMvdGemIdealTrackCand not fount!" << std::endl;
	}

//	Get the tracks from the ideal track finder
	tracks = (TClonesArray*) ioman->GetObject("SttMvdGemIdealTrack");
	if (!tracks) {
		std::cout << "-E- PatternCounter:Init: SttMvdGemIdealTrack not found!" << std::endl;
	}


	std::cout << "-I- PatternCounter: Initialisation successful" << std::endl;
	return kSUCCESS;
}

void PatternCounter::Exec(Option_t *opt) {
	// Generate pattern for events and tracks
	GenerateEventPatterns();
	GenerateTrackPatterns();

	FairEventHeader *header = (FairEventHeader*) fEventHeader;
	if (header->GetMCEntryNumber() % 1000 == 0) {
	  std::cout << tmpTrackMaps.at(0).size() << std::endl;
	}
}

void PatternCounter::FinishTask() {
//  dbGen.write();
//	outFile->Write();

// Write data to stdout

//	std::multiset<int> nEventPatternRecurrences;
	std::multiset<int> nTrackPatternRecurrences;

	int iSector = 0;
	for (auto patternMap : eventSectorMaps) {
//		for (const auto& pattern : patternMap) {
//			nEventPatternRecurrences.insert(pattern.second);
//		}
		std::cout << "event patterns in sector " << iSector << ": " << patternMap.size() << std::endl;
		std::cout<<"event pattern recurrences: ";
		//	for (auto number : boost::adaptors::reverse(nEventPatternRecurrences)) {
		for (auto const& iter : boost::adaptors::reverse(patternMap)) {
			auto number = iter.first;
			if (number > 1)
			  std::cout << number << " ";
		}
		std::cout << std::endl;
		iSector++;
	}
	iSector = 0;
	for (auto patternMap : tmpTrackMaps) {
				for (const auto& pattern : patternMap) {
					nTrackPatternRecurrences.insert(pattern.second);
				}

		std::cout << "track patterns in sector " << iSector << ": " << patternMap.size() << std::endl;
		std::cout<<"track pattern recurrences: ";
			for (auto number : boost::adaptors::reverse(nTrackPatternRecurrences)) {
//		for (auto const& iter : boost::adaptors::reverse(patternMap)) {
//			auto number = iter.first;
			if (number > 1)
			  std::cout << number << " ";
		}
		std::cout << std::endl;
		nTrackPatternRecurrences.clear();
		iSector++;
	}

//	std::cout<<"event pattern recurrences: ";
////	for (auto number : boost::adaptors::reverse(nEventPatternRecurrences)) {
//	for (auto const& iter : eventSectorMaps) {
//		auto number = iter.first;
//		std::cout << number << " ";
//	}
//	std::cout << std::endl;
//
//	std::cout<<"track pattern recurrences: ";
////	for (auto number : boost::adaptors::reverse(nTrackPatternRecurrences)) {
//	for (auto const& iter : trackSectorMaps) {
//		auto number = iter.first;
//		std::cout << number << " ";
//	}
//	std::cout << std::endl;


}

void PatternCounter::GenerateSectorMaps(std::vector<HitMap> hitMapVector, std::vector<InvertedPatternMap> &sectorMaps) {
// general method for creating sector maps of hit patterns
	for (int iSector = 0; iSector < 6; ++iSector) {
		// access the pattern map for the corresponding sector
		PatternMap patternMap;
		auto& invPatternMap = sectorMaps.at(iSector);
		auto& tmpMap = tmpTrackMaps.at(iSector);
		// loop over all hitMaps in the vector (see event and track specific methods for further comments)
		for (auto const& hitMap : hitMapVector) {
			// select only elements in hitMap that correspond to current sector
			auto range = hitMap.equal_range(iSector);
			int ihit = 0;
			std::set<int> pattern;
			bPattern = &pattern;
			// loop over hitIDs in the current sector
			for (auto iter = range.first; iter != range.second; ++iter) {
				int hitID = iter->second;
				// construct pattern from hitIDs
				pattern.insert(hitID);
				ihit++;
			}

//			if (pattern.size() == 0) continue;
			// insert pattern into patternMap and increment occurrence count by one
			patternMap[pattern]++;
			if (pattern.size() > 0)
			  tmpMap[pattern]++;
//			patternTree[iSector]->Fill();
		}
		for (auto const& iter : patternMap) {
//			invPatternMap[iter.second] = iter.first;
			invPatternMap.insert(std::make_pair(iter.second,iter.first));
		}
	}
}

HitMap PatternCounter::FillHitMap(std::vector<PndSttHit*> sttHitArray) {
	// construct hit map from sectorID and tubeID
	// note that sectorID is only accessible through PndSttTube
	size_t nSttHits = sttHitArray.size();
	HitMap hitMap;
	for (size_t iHit = 0; iHit < nSttHits; iHit++) {
		// Access PndSttHit to get tubeID
		PndSttHit* sttHit = sttHitArray.at(iHit);
		int tubeID = sttHit->GetTubeID();
		// Get sectorID from PndSttTube
		PndSttTube* tube = (PndSttTube*) (fTubeArray->At(tubeID));
		int sectorID = tube->GetSectorID();
		// Insert pairs of sectorID and tubeID into map
		hitMap.insert(std::make_pair(sectorID, tubeID));
	}
	return hitMap;
}

std::vector<Pattern> PatternCounter::FillPatterns(std::vector<HitMap> hitMapVector) {
  std::vector<Pattern> patterns;

    for (int iSector = 0; iSector < 6; ++iSector) {
      int iTrack = 0;
      for (auto const& hitMap : hitMapVector) {
        PndTrack* track = (PndTrack*) tracks->At(iTrack);
        TVector3 momentum;
        momentum.SetXYZ(1, 2, 3);
//        momentum = track->GetParamFirst().GetSDMomentum();
        auto range = hitMap.equal_range(iSector);
        //      int ihit = 0;
        Pattern pattern;
        pattern.setSectorID(iSector);
        for (auto iter = range.first; iter != range.second; ++iter) {
          int hitID = iter->second;
          //        pattern.insert(hitID);
          //        ihit++;
          pattern.addTubeID(hitID);
        }
      pattern.addMomentum(momentum);
      patterns.push_back(pattern);
      iTrack++;
    }
  }

  return patterns;
}

void PatternCounter::GenerateEventPatterns() {
	// This method generates sector maps for complete events
	std::vector<PndSttHit*> sttHitArray;
	// For a complete event the sttHitArray contains all STT hits of the event,
	// which are stored in fSTTHitArray
	for (int iEntry = 0; iEntry < fSTTHitArray->GetEntriesFast(); iEntry++) {
		PndSttHit* sttHit = (PndSttHit*) fSTTHitArray->At(iEntry);
		sttHitArray.push_back(sttHit);
	}
	// Generate a hitMap from the sttHitArray
	HitMap hitMap = FillHitMap(sttHitArray);
	// Fill hitMap into a vector (only one element here, but more are needed
	// when using individual tracks)
	std::vector<HitMap> hitMapVector;
	hitMapVector.push_back(hitMap);
	// Generate the sector maps for the event from the hitMaps
	auto &sectorMaps = eventSectorMaps;
	GenerateSectorMaps(hitMapVector, sectorMaps);
}

void PatternCounter::GenerateTrackPatterns() {
	// This method generates track by track sector maps

	// FairRootManager is needed to access FairLinks
	FairRootManager *ioman = FairRootManager::Instance();

	std::vector<HitMap> hitMapVector;
	// Loop over all track candidates
	int nTrackCands = trackCands->GetEntriesFast();
	for (int iTrackCand = 0; iTrackCand < nTrackCands; iTrackCand++) {
		// Get the next track candidate
		PndTrackCand *cand = (PndTrackCand*) (trackCands->At(iTrackCand));
		// Get the FairLinks from the candidate pointing at the STTHit branch
		FairMultiLinkedData links = cand->GetLinksWithType(sttBranchID);
		// Construct the sttHitArray for each track candidate,
		// taking into account all STTHits of the corresponding track
		std::vector<PndSttHit*> sttHitArray;
		for (int iLink = 0; iLink < links.GetNLinks(); ++iLink) {
			FairLink link = links.GetLink(iLink);
			PndSttHit *sttHit = (PndSttHit*) (ioman->GetCloneOfLinkData(link));
			sttHitArray.push_back(sttHit);
		}
		// Generate the hitMap from the sttHitArray
		HitMap hitMap = FillHitMap(sttHitArray);
		// Store the hitMaps of all tracks in a vector
		hitMapVector.push_back(hitMap);
	}
	// Generate the sector maps for individual tracks from the hitMapVector
	auto &sectorMaps = trackSectorMaps;
	GenerateSectorMaps(hitMapVector, sectorMaps);

//	std::vector<Pattern> patterns = FillPatterns(hitMapVector);
//	for (auto const& pattern : patterns) {
//	  dbGen.addPatternEntry(pattern);
//	}
}
