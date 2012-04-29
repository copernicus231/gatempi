/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateRunManager.hh
 \author copernicus email:copernicus231@gmail.com
 */
#ifdef GATE_PARALLEL_MPI

#ifndef PARGATERUNMANAGER_HH_
#define PARGATERUNMANAGER_HH_

#include "GateRunManager.hh"

class ParGateRunManager: public GateRunManager {
protected:
	ParGateRunManager();

public:
	virtual ~ParGateRunManager();

	/*
	 * override of G4RunManager method
	 */
	virtual void DoEventLoop(G4int n_event, const char* macroFile,
			G4int n_select);

	/**
	 *
	 */
	void WaitForEventIdRequest();

	/**
	 *
	 */
	G4int PullEventId();

	/**
	 *
	 */
	void SignalEndOfEventIdRequest();

	enum messages {
		EVENT_IDS_REQUEST = 99, EVENT_IDS_ANSWER = 100, END_OF_EVENTS = 101
	};

private:
	G4int idCurrentEvent;
	G4int numberOfEventsPerJob;
	friend class GateRunManagerFactory;
};

#endif /* PARGATERUNMANAGER_HH_ */

#endif
