/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateSourceMgr.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#ifndef PARGATESOURCEMGR_HH_
#define PARGATESOURCEMGR_HH_

#include "GateSourceMgr.hh"
#include <vector>

using namespace std;
class ParGateSourceMgr: public GateSourceMgr {
public:
	virtual ~ParGateSourceMgr();
	virtual GateVSource* GetNextSource();
	virtual G4int PrepareNextEvent(G4Event* event);
	virtual G4int PrepareNextRun(const G4Run* run);
	void WaitForTimeRequests();
	G4double GetCentralTime();
	void SetCentralTime(G4double);

	enum messages {
		CURRENT_TIME = 2,
		GET_CURRENT_TIME = 3,
		SET_CURRENT_TIME = 4,
		END_LISTEN = 5
	};

protected:
	ParGateSourceMgr();
	vector<G4double> m_firstTime_vector;
	vector<GateVSource*> pFirstSource_vector;
	vector<G4int> m_currentSourceID_vector;
	G4int sourceIndex;
	G4int numberOfEvents;

private:
	friend class GateSourceMgrFactory;
};

#endif /* PARGATESOURCEMGR_HH_ */

#endif
