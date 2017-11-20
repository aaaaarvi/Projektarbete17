/*
 * Pattern.cxx
 *
 *  Created on: May 29, 2017
 *      Author: vagrant
 */

#include "Pattern.h"

ClassImp(Pattern)

Pattern::Pattern() {
  Clear();
  std::map<int,Double_t> tmpStamps;
  timeStamps.push_back(tmpStamps);
}

Pattern::~Pattern() {
}

void Pattern::Clear() {
  tubeIDs.clear();
  momenta.clear();
  timeStamps.clear();
  sectorID = -1;
  count = 0;
}

bool Pattern::IsEmpty() {
  if (tubeIDs.size() == 0) {
    return true;
  } else {
    return false;
  }
}


void Pattern::addMomenta(std::vector<TVector3> momentaVector) {
  momenta.insert(std::end(momenta), std::begin(momentaVector), std::end(momentaVector));
}
void Pattern::addTimeStamp(int tubeID, Double_t timeStamp) {
  timeStamps.back().insert(std::make_pair(tubeID, timeStamp));
}
void Pattern::addTimeStamps(std::vector<std::map<int,Double_t>> timeStampsVector) {
  timeStamps.insert(std::end(timeStamps), std::begin(timeStampsVector), std::end(timeStampsVector));
}
