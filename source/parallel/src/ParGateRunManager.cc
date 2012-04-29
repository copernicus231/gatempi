/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateRunManager.cc
 \author copernicus email:copernicus231@gmail.com
 */
#ifdef GATE_PARALLEL_MPI

//TODO CLEAN INCLUDE AREA
#include "ParGateRunManager.hh"
#include "GateRunManager.hh"
#include "G4StateManager.hh"
#include "G4UImanager.hh"
#include "G4TransportationManager.hh"

#include "GateDetectorConstruction.hh"
#include "GateRunManagerMessenger.hh"
#include "GateHounsfieldToMaterialsBuilder.hh"
#include "G4Timer.hh"

#include "G4RunManager.hh"
#include "G4RunManagerKernel.hh"

#include "G4StateManager.hh"
#include "G4ApplicationState.hh"
#include "Randomize.hh"
#include "G4Run.hh"
#include "G4RunMessenger.hh"
#include "G4VUserPhysicsList.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4UserRunAction.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4VPersistencyManager.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessTable.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Material.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include <sstream>
#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "ParGateMPI.hh"
#include "ParGateSourceMgr.hh"
#include "SourceMgrFactory.hh"
#include <mpi.h>

ParGateRunManager::ParGateRunManager() :
		GateRunManager() {
	idCurrentEvent = 0;
	numberOfEventsPerJob = ParGateMPI::GetInstance()->GetNumberOfEvents();
}

ParGateRunManager::~ParGateRunManager() {

}

void ParGateRunManager::SignalEndOfEventIdRequest() {
	MPI_Send(NULL, 0, MPI_INT, 0, END_OF_EVENTS,
			ParGateMPI::GetInstance()->GetEventWorldComm());
	return;
}

void ParGateRunManager::DoEventLoop(G4int n_event, const char *macroFile,
		G4int n_select) {
	numberOfEventsPerJob = ParGateMPI::GetInstance()->GetNumberOfEvents();

	MPI_Barrier(MPI_COMM_WORLD);

	//Maintains the coherence if the macros change the number of events or if we will add a feature for a dynamic number of events
	if (verboseLevel > 0) {
		timer->Start();
	}

	G4String msg; //TODO SIMPLIFIED THE CODE GATE NEVER USE A MACRO FILE
	if (macroFile != 0) {
		if (n_select < 0)
			n_select = n_event;
		msg = "/control/execute ";
		msg += macroFile;
	} else {
		n_select = -1;
	}
	G4int i_event = 0;
	//obtain the source manager for coordinate all the process
	GateSourceMgr* sourceMgr = GateSourceMgrFactory::GetSourceManager();

	//prepare the nextrun all at the same time
	const G4Run* currentRun = G4RunManager::GetRunManager()->GetCurrentRun();
	sourceMgr->PrepareNextRun(currentRun);
	// Event loop
	if (ParGateMPI::GetInstance()->GetEventWorldRank() == 0) {
		//Code for the event manager coordinator
		WaitForEventIdRequest();
	} else {
		G4int i;
		while (true) {
			i_event = PullEventId(); //Pull for the id of the first event of the job
			for (i = i_event; i < i_event + numberOfEventsPerJob; i++) {
				currentEvent = GenerateEvent(i);
				eventManager->ProcessOneEvent(currentEvent);
				AnalyzeEvent(currentEvent);
				UpdateScoring();
				if (i_event < n_select)
					G4UImanager::GetUIpointer()->ApplyCommand(msg);
				StackPreviousEvent(currentEvent);
				currentEvent = 0;
				if (runAborted)
					break;
			}
			if (runAborted)
				break;
		}
		SignalEndOfEventIdRequest();
	}

	MPI_Barrier(ParGateMPI::GetInstance()->GetSourceWorldComm());

	if (verboseLevel > 0) {
		GateApplicationMgr* appMgr = GateApplicationMgr::GetInstance();
		timer->Stop();
		G4cout << "Run terminated." << G4endl;
		G4cout << "Run Summary" << G4endl;
		if (runAborted) {
			G4cout << "  Run Aborted after " << i_event + 1
					<< " events processed." << G4endl;
		} else {
			G4cout << "  Number of events processed : " << n_event << G4endl;
		}
		G4cout << "Synchronized m_time value:" << appMgr->GetCurrentTime()
				<< G4endl;
		G4cout << "  " << *timer << G4endl;
	}
}

void ParGateRunManager::WaitForEventIdRequest() {
	MPI_Status status;
	G4int numberOfProcessFinished = 0;
	ParGateSourceMgr* sourceMgr =
			static_cast<ParGateSourceMgr*>(GateSourceMgrFactory::GetSourceManager());

	idCurrentEvent = 0;
	while (true) {
		MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
				ParGateMPI::GetInstance()->GetEventWorldComm(), &status);

		if (status.MPI_TAG == END_OF_EVENTS) {
			++numberOfProcessFinished;
			if (ParGateMPI::GetInstance()->GetEventWorldSize()
					== (numberOfProcessFinished + 1)) {
				break;
			}
		} else if (status.MPI_TAG == EVENT_IDS_REQUEST) {
			MPI_Send(&idCurrentEvent, 1, MPI_INT, status.MPI_SOURCE,
					EVENT_IDS_ANSWER,
					ParGateMPI::GetInstance()->GetEventWorldComm());
			idCurrentEvent = idCurrentEvent + numberOfEventsPerJob;
			sourceMgr->WaitForTimeRequests();

		} else {
			continue;
		}
	}
	GateApplicationMgr::GetInstance()->SetCurrentTime(
			sourceMgr->GetCentralTime());
	return;
}

G4int ParGateRunManager::PullEventId() {
	G4int value = 0;
	MPI_Send(NULL, 0, MPI_INT, 0, EVENT_IDS_REQUEST,
			ParGateMPI::GetInstance()->GetEventWorldComm());
	MPI_Recv(&value, 1, MPI_INT, 0, EVENT_IDS_ANSWER,
			ParGateMPI::GetInstance()->GetEventWorldComm(), NULL);

	return value;
}

#endif
