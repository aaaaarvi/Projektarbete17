/*
 * TestTubeGenerator.cxx
 *
 *  Created on: Nov 2, 2017
 *      Author: vagrant
 */

#include "TestTubeGenerator.h"

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


ClassImp(TestTubeGenerator)

TestTubeGenerator::TestTubeGenerator() {
  fSttParameters = NULL;
  fSTTHitArray = NULL;
  fFTSHitArray = NULL;
  fMCTrackArray = NULL;
  fFtsParameters = NULL;
  fTubeArray = NULL;
}

TestTubeGenerator::~TestTubeGenerator() {
  // TODO Auto-generated destructor stub
}

void TestTubeGenerator::SetParContainers() {
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");
  fFtsParameters = (PndGeoFtsPar*) rtdb->getContainer("PndGeoFtsPar");
	
}

InitStatus TestTubeGenerator::Init() {
  
  // Create file
  csvFile.open("tubeData.csv");
  
  // Get the running instance of the FairRootManager to access tree branches
  FairRootManager *ioman = FairRootManager::Instance();
  if(!ioman) {
    std::cout << "-E- PatternDBGenerator::Init: FairRootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  // Initialize the STT map and get an array of all tubes
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();
	
  // Initialization successful
  std::cout << "-I- PatternDBGenerator: Initialisation successful" << std::endl;
  return kSUCCESS;
}


void TestTubeGenerator::Exec(Option_t* opt) {
  TVector3 pos;
  double x, y, z;
  PndSttTube* tube;
  for (int j = 0; j < fTubeArray->GetEntriesFast(); ++j) {
    if (!fTubeArray->At(j)) continue;
    tube = (PndSttTube*) fTubeArray->At(j);
    pos = tube->GetPosition();
    x = pos.X();
    y = pos.Y();
    z = pos.Z();
    csvFile << tube-> GetTubeID() << "," << x << "," << y << "," << z << "\n";	
  }
  csvFile << "\n" << "\n" << "\n"; 
}


void TestTubeGenerator::FinishTask() {
  // close csv
  csvFile.close();
}










