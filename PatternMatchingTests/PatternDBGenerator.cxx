/*
 * PatternDBGenerator.cxx
 *
 *  Created on: Jun 12, 2017
 *      Author: vagrant
 */

#include "PatternDBGenerator.h"

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


ClassImp(PatternDBGenerator)

PatternDBGenerator::PatternDBGenerator() {
  fEventHeader = NULL;
  fTubeArray = NULL;
  fSTTHitArray = NULL;
  trackCands = NULL;
  idealTracks = NULL;
  genTracks = NULL;
  fSttParameters = NULL;
  sttBranchID = -1;

  outputFile = NULL;
  tTrackPatterns = NULL;

  bPattern = NULL;

//  set default filename, can be overwritten with setOutputFile
  outputFileName = "patternDB.root";
}

PatternDBGenerator::~PatternDBGenerator() {
}

void PatternDBGenerator::SetParContainers() {
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
}
InitStatus PatternDBGenerator::Init() {
  std::cout << "Filename for pattern DB: " << outputFileName << std::endl;
  outputFile = new TFile(outputFileName,"RECREATE");
  tTrackPatterns = new TTree("trackPatterns","Tree containing track patterns");
  tTrackPatterns->Branch("pattern",&bPattern);

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

// Initialize the STT map and get an array of all tubes
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

// Get the ID of the STTHit branch for accessing the FairLinks later
  sttBranchID = ioman->GetBranchId("STTHit");
  if (sttBranchID == -1) {
    std::cout << "-E- PatternDBGenerator:Init: STTHit branch not found!" << std::endl;
  }

//  Access the MCTrack branch
  fMCTrackArray = (TClonesArray*) ioman->GetObject("MCTrack");

//  Get the ID of the MCTrack branch for accessing through FairLinks later
  mcTrackID = ioman->GetBranchId("MCTrack");
  if (mcTrackID == -1) {
    std::cout << "-E- PatternDBGenerator:Init: MCTrack branch not found!" << std::endl;
  }

// Get the track candidates from the ideal track finder
  trackCands = (TClonesArray*) ioman->GetObject("SttMvdGemIdealTrackCand");
  if (!trackCands) {
    std::cout << "-E- PatternDBGenerator:Init: SttMvdGemIdealTrackCand not fount!" << std::endl;
  }

//  Get the ideal tracks from the ideal track finder
  idealTracks = (TClonesArray*) ioman->GetObject("SttMvdGemIdealTrack");
  if (!idealTracks) {
    std::cout << "-E- PatternDBGenerator:Init: SttMvdGemIdealTrack not found!" << std::endl;
  }
//  Get the gen tracks from the ideal track finder
  genTracks = (TClonesArray*) ioman->GetObject("SttMvdGemGenTrack");
  if (!genTracks) {
    std::cout << "-E- PatternDBGenerator:Init: SttMvdGemGenTrack not found!" << std::endl;
  }

  std::cout << "-I- PatternDBGenerator: Initialisation successful" << std::endl;
  return kSUCCESS;
}
void PatternDBGenerator::Exec(Option_t *opt) {
  GenerateTrackPatterns();
  
  Bool_t DrawEvent = kTRUE;
  
  //if(eventNumber == 1) {
  if (DrawEvent) {
    SttXYproj = new TCanvas();
    SttXYproj->SetCanvasSize(1000, 1000);//1000,1000
    //SttXYproj->SetTitle("SttXYproj");
    Double_t w = gPad->GetWw()*gPad->GetAbsWNDC();
    Double_t h = gPad->GetWh()*gPad->GetAbsHNDC();
    Double_t xmin = -42;//-42 -25
    Double_t xmax = 42;//42 -5
    Double_t ymin = -42;//-42 10
    Double_t ymax = 42;//42 40
    SttXYproj->SetFixedAspectRatio();
    SttXYproj->Range(xmin,ymin,xmax,ymax);
    SttXYproj->cd();

    //Drawing Stt here
    DrawStt(true,true);
  }
  hitsDraw.clear();
}
void PatternDBGenerator::FinishTask() {

//  write();
//  tTrackPatterns->Write();
  tTrackPatterns->AutoSave();
//  outputFile->Write();
  outputFile->Close();
//  sleep(10);
//  mergeIdenticalPatterns();
}

void PatternDBGenerator::GenerateTrackPatterns() {
//  This method generates hit patterns by track and by sector

//  Get FairRootManager instance to access objects through FairLinks
  FairRootManager *ioman = FairRootManager::Instance();

//  Loop over all track candidates
  int nTrackCands = trackCands->GetEntriesFast();
  for (int iTrackCand = 0; iTrackCand < nTrackCands; ++iTrackCand) {
//    Get the next track candidate
    PndTrackCand *cand = (PndTrackCand*) (trackCands->At(iTrackCand));
//    Get the FairLinks from the candidate pointing at STTHit branch
    FairMultiLinkedData sttLinks = cand->GetLinksWithType(sttBranchID);
//    Construct sttHitArray for each track candidate,
//    taking into account all STTHits of the corresponding track
    HitArray sttHitArray;
    for (int iLink = 0; iLink < sttLinks.GetNLinks(); ++iLink) {
      FairLink sttLink = sttLinks.GetLink(iLink);
      PndSttHit *sttHit = (PndSttHit*) (ioman->GetCloneOfLinkData(sttLink));
      sttHitArray.push_back(sttHit);
      hitsDraw.insert(sttHit->GetTubeID());
    }
//    Get link to corresponding MC track
    FairMultiLinkedData mcTrackLinks = cand->GetLinksWithType(mcTrackID);
//    std::cout << "Number of track links: " << mcTrackLinks.GetNLinks() << std::endl;
//    Check if only one MC track was found
    if (mcTrackLinks.GetNLinks() == 0) {
      std::cout << "WARNING: PatternDBGenerator:GenerateTrackPatterns: No MCTrack found, skipping event!" << std::endl;
      continue;
    }
    if (mcTrackLinks.GetNLinks() > 1) {
      std::cout << "WARNING: PatternDBGenerator:GenerateTrackPatterns: Found more than one MCTrack link!!!" << std::endl;
    }
//    Get only first link since only one should be associated with track candidate
    FairLink mcTrackLink = mcTrackLinks.GetLink(0);
//    mcTrackLink.PrintLinkInfo();
    PndMCTrack *mcTrack = (PndMCTrack*) (ioman->GetCloneOfLinkData(mcTrackLink));

    vector<Pattern> patterns = FillPatterns(sttHitArray, mcTrack);
    for (auto const& pattern : patterns) {
      addPatternEntry(pattern);
    }
  }
}
HitMap PatternDBGenerator::FillHitMap(HitArray sttHitArray) {
//  size_t nSttHits = sttHitArray.size();
  HitMap hitMap;
  for (auto const& sttHit : sttHitArray) {
    int tubeID = sttHit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) (fTubeArray->At(tubeID));
    int sectorID = tube->GetSectorID();
    hitMap.insert(std::make_pair(sectorID, tubeID));
  }
  return hitMap;
}
TimeStamps PatternDBGenerator::FillTimeStamps(HitArray sttHitArray) {
  TimeStamps stamps;
  for (auto const& sttHit : sttHitArray) {
    int tubeID = sttHit->GetTubeID();
    Double_t timeStamp = sttHit->GetTimeStamp();
    stamps[tubeID] = timeStamp;
  }

  return stamps;
}
std::vector<Pattern> PatternDBGenerator::FillPatterns(HitArray sttHitArray, PndMCTrack *mcTrack) {
  vector<Pattern> patterns;
  HitMap hitMap = FillHitMap(sttHitArray);
  TimeStamps timeStamps = FillTimeStamps(sttHitArray);
  TVector3 momentum = mcTrack->GetMomentum();
  for (int iSector = 0; iSector < 6; ++iSector) {
    auto hitMapSectorRange = hitMap.equal_range(iSector);
    Pattern pattern;
    pattern.setSectorID(iSector);
    pattern.addMomentum(momentum);
    for (auto iter = hitMapSectorRange.first; iter != hitMapSectorRange.second; ++iter) {
      int tubeID = iter->second;
      pattern.addTubeID(tubeID);
      pattern.addTimeStamp(tubeID, timeStamps[tubeID]);
    }
    pattern.raisePatternCount();
    if (!pattern.IsEmpty()) {
      patterns.push_back(pattern);
    }
  }


  return patterns;
}

void PatternDBGenerator::mergeIdenticalPatterns() {
//  DEPRECATED, will be moved to PatternDBMerger
  TFile *patternFile = TFile::Open(outputFileName,"READ");
  TTree *patternTree = 0;
  patternTree = (TTree*) patternFile->Get("trackPatterns");

  Pattern *pattern;
  patternTree->SetBranchAddress("pattern",&pattern);
  std::map<std::set<int>, std::set<int>> patternEntries;
  Long64_t nEntries = patternTree->GetEntriesFast();
  std::cout << "looping over " << nEntries << " patterns" << std::endl;
  for (Long64_t iEntry = 0; iEntry < nEntries; ++iEntry) {
//    std::cout << "Getting entry " << iEntry << std::endl;
    patternTree->GetEntry(iEntry);
    std::set<int> tubePattern = pattern->GetTubeIDs();
    patternEntries[tubePattern].insert(iEntry);
  }
  std::cout << "Unique patterns identified: " << patternEntries.size() << std::endl;
  std::multimap<int, std::set<int>> mapEntries;
  std::cout << "vEntries.size(): ";
  for (auto iter : patternEntries) {
    std::set<int> &vEntries = iter.second;
//    set this to 1 to only consider reoccurring patterns
    if (vEntries.size() > 0) {
      mapEntries.insert(std::make_pair(vEntries.size(), vEntries));
//      std::cout << "pattern entries: ";
      std::cout << vEntries.size() << " ";

//      for (auto const& entry : vEntries) {
//        std::cout << entry << " ";
//      }
//      std::cout << std::endl;
    }
  }
  std::cout << std::endl;

  TFile *newFile = new TFile("patternDBclean.root","RECREATE");
  TTree *newPatternTree = new TTree("mergedTrackPatterns","Tree containing merged track patterns");
  Pattern *bNewPattern = 0;
  newPatternTree->Branch("pattern",&bNewPattern);

  for (auto iter : boost::adaptors::reverse(mapEntries)) {
//    std::cout << "occurrences: " << iter.first << ", entries: ";
    std::vector<Pattern> patternsInEntry;
    Pattern newPattern;
    bNewPattern = &newPattern;
    for (auto const& entry : iter.second) {
//      std::cout << entry << " ";
      patternTree->GetEntry(entry);
      patternsInEntry.push_back(*pattern);
    }
    newPattern = mergeEntries(patternsInEntry);
    newPatternTree->Fill();
//    std::cout << std::endl;
  }
  newPatternTree->AutoSave();
}
Pattern PatternDBGenerator::mergeEntries(std::vector<Pattern> &entries) {
//  DEPRECATED, will be moved to PatternDBMerger
  Pattern merged;
//  std::cout << "patterns to be merged: " << entries.size() << std::endl;
  if (entries.size() == 0) return merged;
  merged.SetTubeIDs(entries.at(0).GetTubeIDs());
  merged.setSectorID(entries.at(0).GetSectorId());

  for (auto const& pattern : entries) {
    int currentCount = merged.GetCount();
    int newCount = currentCount + pattern.GetCount();
    merged.setPatternCount(newCount);
  }

  return merged;
}

void PatternDBGenerator::addPatternEntry(Pattern pattern) {
//  DEPRECATED, remove soon
  bPattern = &pattern;
  tTrackPatterns->Fill();
}

void PatternDBGenerator::write() {

  outputFile->Write();
//  outputFile->Close();
}

void PatternDBGenerator::DrawStt(bool isskew, bool onlyhits) {
//  TODO move to custom class for drawing
  TArc *riecirc = new TArc(0., 0., 41.);
  riecirc->SetFillStyle(0);
  riecirc->Draw("SAME");
  PndSttTube* tube;
  TVector3 pos;
  double r,x,y,z;


  for (int j = 0; j < fTubeArray->GetEntriesFast(); ++j) {
    if (!fTubeArray->At(j)) continue;
    auto search = hitsDraw.find(j);
    if(onlyhits == true && search == hitsDraw.end()) continue;
    tube = (PndSttTube*) fTubeArray->At(j);
    int colour = tube->GetSectorID() + 1;
    pos = tube->GetPosition();
    x = pos.X();
    y = pos.Y();
    z = pos.Z();
    if(tube->IsParallel()){
      r = tube->GetRadIn();
      TArc *circ = new TArc(x, y, r);
      circ->SetFillStyle(0);
//      circ->SetLineColor(kGreen-8);
      circ->SetLineColor(colour);
      circ->Draw("SAME");
    }
    else if (isskew && tube->IsSkew()) {
      if (tube->GetLayerID() == 10 || tube->GetLayerID() == 11 || tube->GetLayerID() == 14 ||tube->GetLayerID() == 15) {
        r = tube->GetRadIn();
        TArc *circ = new TArc(x, y, r);
        circ->SetFillStyle(0);
//        circ->SetLineColor(kRed);
        circ->SetLineColor(colour);
        circ->Draw("SAME");
      }
      else{
        r = tube->GetRadIn();
        TArc *circ = new TArc(x, y, r);
        circ->SetFillStyle(0);
//        circ->SetLineColor(kBlue);
        circ->SetLineColor(colour);
        circ->Draw("SAME");
      }
    }
  }

  TText *xa = new TText(30,0.1,"X-axis");
  xa->SetTextAlign(11);
  xa->SetTextSize(0.05);
  xa->Draw("SAME");
  TText *ya = new TText(-0.1,30,"Y-axis");
  ya->SetTextAlign(11);
  ya->SetTextAngle(90);
  ya->SetTextSize(0.05);
  ya->Draw("SAME");
  TText *le = new TText(20,-12,"41 cm");
  le->SetTextAlign(11);
  le->SetTextAngle(-31);
  le->SetTextSize(0.05);
  le->Draw("SAME");
  TArrow *firstLine = new TArrow(0., 0., 35., -TMath::Sqrt(41.*41. - 35.*35.), 0.9,"|>");
  firstLine->SetLineColor(kBlack);
  firstLine->SetLineWidth(3);
  firstLine->Draw("SAME");

  //Mark origin
  TLine* xLine = new TLine(-50, 0, 50, 0);
  TLine* yLine = new TLine(0, 50, 0, -50);
  xLine->Draw("SAME");
  yLine->Draw("SAME");

}
