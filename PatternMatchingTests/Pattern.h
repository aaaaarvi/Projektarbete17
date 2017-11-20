/*
 * Pattern.h
 *
 *  Created on: May 29, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERN_H_
#define PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERN_H_

//#include <TNamed.h>
#include <Rtypes.h>
#include <set>
#include <map>
#include <vector>
#include <TVector3.h>

class Pattern : public TObject {
public:
	Pattern();
	virtual ~Pattern();

	void Clear();

//	std::set<int> GetTubeIDs() {return tubeIDs;}
	const std::set<int>& GetTubeIDs() const {
	    return tubeIDs;
	  }
  int GetSectorId() const {
    return sectorID;
  }
  int GetCount() const {
    return count;
  }
  const std::vector<TVector3>& getMomenta() const {
    return momenta;
  }

  const std::vector<std::map<int, Double_t> >& getTimeStamps() const {
    return timeStamps;
  }

  void SetTubeIDs(const std::set<int>& tubes) {
    this->tubeIDs = tubes;
  }

	void addTubeID(int tubeID) {tubeIDs.insert(tubeID);}
	void addMomentum(TVector3 momentum) {momenta.push_back(momentum);}
	void addMomenta(std::vector<TVector3> momentaVector);
	void addTimeStamp(int tubeID, Double_t timeStamp); // {timeStamps.insert(std::make_pair(tubeID, timeStamp));}
	void addTimeStamps(std::vector<std::map<int,Double_t>> timeStampsVector);
	void setSectorID(int id) {sectorID = id;}
	void setPatternCount(int n) {count = n;}
	void raisePatternCount() {count++;}
	bool IsEmpty();

private:
	std::set<int> tubeIDs;
	std::vector<TVector3> momenta;
	std::vector<std::map<int,Double_t>> timeStamps;
	int sectorID;
	int count;

	ClassDef(Pattern,1)


};

#endif /* PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERN_H_ */
