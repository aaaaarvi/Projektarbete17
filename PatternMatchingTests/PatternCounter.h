/*
 * PatternCounter.h
 *
 *  Created on: May 19, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_STTCELLTRACKFINDER_PATTERNCOUNTER_H_
#define PNDTRACKERS_STTCELLTRACKFINDER_PATTERNCOUNTER_H_

#include <FairTask.h>
#include "PndGeoSttPar.h"
#include "TClonesArray.h"
#include <map>
#include <vector>
#include <PndSttHit.h>

#include "Pattern.h"
//#include "PatternDBGenerator.h"
// pattern, number of occurrences
typedef std::map<std::set<int>, int> PatternMap;
typedef std::multimap<int, std::set<int>> InvertedPatternMap;
// sectorID, tubeID
typedef std::multimap<int, int> HitMap;

class PatternCounter: public FairTask {
public:
	PatternCounter();
	virtual ~PatternCounter();

protected:
	virtual void SetParContainers();
	virtual InitStatus Init();
	virtual void Exec(Option_t* opt);
	virtual void FinishTask();

private:
// Methods required for pattern generation
	void GenerateEventPatterns();
	void GenerateTrackPatterns();
	void GenerateSectorMaps(std::vector<HitMap> hitMapVector, std::vector<InvertedPatternMap> &sectorMaps);
	HitMap FillHitMap(std::vector<PndSttHit*> sttHitArray);
	std::vector<Pattern> FillPatterns(std::vector<HitMap> hitMapVector);

// Input data
	TClonesArray *fEventHeader;
	PndGeoSttPar *fSttParameters;
	TClonesArray *fTubeArray;
	TClonesArray *fSTTHitArray;
	TClonesArray *trackCands;
	TClonesArray *tracks;
	int sttBranchID;

	// vector of pattern maps for events and tracks, respectively
	// one vector element corresponds to one sector
	std::vector<InvertedPatternMap> eventSectorMaps;
	std::vector<InvertedPatternMap> trackSectorMaps;
	std::vector<PatternMap> tmpTrackMaps;

//	TFile *outFile;
//	One TTree per sector to store patterns
//	TTree *patternTree[6];
//	TBranch variables
	std::set<int> *bPattern;
	std::vector<Double_t> *bMomenta;

//	PatternDBGenerator dbGen;

	ClassDef(PatternCounter,1)
};

#endif /* PNDTRACKERS_STTCELLTRACKFINDER_PATTERNCOUNTER_H_ */
