/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateMPI.cc
 \author copernicus email:copernicus231@gmail.com
 */
#ifdef GATE_PARALLEL_MPI

#include "ParGateMPI.hh"
#include "G4ios.hh"
#include <stdlib.h>
#include <mpi.h>

ParGateMPI* ParGateMPI::instance = 0;

ParGateMPI* ParGateMPI::GetInstance(int argc, char** argv) {
	if (instance == 0) {
		int numberOfProcess = 0;
		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcess);
		G4cout << "Number of processor current Run:" << numberOfProcess
				<< G4endl;
		if (numberOfProcess <= 1) {
			MPI_Finalize();
			instance = new ParGateMPI();
			return instance;
		}

		if (numberOfProcess == 2) {
			MPI_Finalize();
			throw GateMPIException(
					"Number of Process Should be at less 3 for ParGate");
		}
	}
	return ParGateMPI::GetInstance();
}

ParGateMPI* ParGateMPI::GetInstance() {
	if (instance == 0) {
		int rank, size;
		int *sourceWorldRanks, *eventWorldRanks;
		MPI_Group worldGroup;
		MPI_Group sourcesWorldGroup;
		MPI_Group eventsWorldGroup;
		MPI_Comm *eventWorldComm = (MPI_Comm *) malloc(sizeof(MPI_Comm));
		MPI_Comm *sourceWorldComm = (MPI_Comm *) malloc(sizeof(MPI_Comm));
		;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		sourceWorldRanks = new int[size];
		eventWorldRanks = new int[size];
		for (int i = 0; i < size; i++) {
			sourceWorldRanks[i] = i;
			eventWorldRanks[i] = i;
		}
		MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
		MPI_Group_incl(worldGroup, size, sourceWorldRanks, &sourcesWorldGroup);
		MPI_Comm_create(MPI_COMM_WORLD, sourcesWorldGroup, sourceWorldComm);

		MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
		MPI_Group_incl(worldGroup, size, eventWorldRanks, &eventsWorldGroup);
		MPI_Comm_create(MPI_COMM_WORLD, eventsWorldGroup, eventWorldComm);
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank == 0) {
			instance = new ParGateMPI(eventWorldComm, sourceWorldComm);
		} else {
			instance = new ParGateMPI(eventWorldComm, sourceWorldComm);
		}
		delete sourceWorldRanks;
		delete eventWorldRanks;
	}
	return instance;
}

void ParGateMPI::DestroyInstance() {
	if (instance != 0) {
		if (instance->IsRunning()) {
			MPI_Barrier(MPI_COMM_WORLD);
			G4cout << "End MPI:" << G4endl;
			MPI_Finalize();
		}
		delete instance;
	}
}

ParGateMPI::ParGateMPI(MPI_Comm *eventWorldComm, MPI_Comm *sourceWorldComm) {

	if (eventWorldComm == NULL) {
		EventWorldRank = -1;
		EventWorldSize = -1;
	} else {
		MPI_Comm_rank(*eventWorldComm, &EventWorldRank);
		MPI_Comm_size(*eventWorldComm, &EventWorldSize);
	}
	MPI_Comm_rank(*sourceWorldComm, &SourceWorldRank);
	MPI_Comm_size(*sourceWorldComm, &SourceWorldSize);
	EventWorldComm = eventWorldComm;
	SourceWorldComm = sourceWorldComm;
	NumberOfEvents = 1000;
	MPIRunning = true;
}
ParGateMPI::ParGateMPI() :
		EventWorldComm(NULL), SourceWorldComm(NULL), EventWorldRank(0), SourceWorldRank(
				0), EventWorldSize(0), SourceWorldSize(0), NumberOfEvents(0), MPIRunning(
				false) {
}

ParGateMPI::~ParGateMPI() {
	if (EventWorldComm != NULL) {
		free(EventWorldComm);
	}
	if (SourceWorldComm != NULL) {
		free(SourceWorldComm);
	}
}

int ParGateMPI::GetNumberOfEvents() {
	return NumberOfEvents;
}

void ParGateMPI::SetNumberOfEvents(int size) {
	NumberOfEvents = size;
}

int ParGateMPI::GetEventWorldSize() {
	return EventWorldSize;
}

int ParGateMPI::GetSourceWorldRank() {
	return SourceWorldRank;
}

MPI_Comm &ParGateMPI::GetEventWorldComm() {
	return *EventWorldComm;
}

int ParGateMPI::GetEventWorldRank() {
	return EventWorldRank;
}

MPI_Comm &ParGateMPI::GetSourceWorldComm() {
	return *SourceWorldComm;
}

int ParGateMPI::GetSourceWorldSize() {
	return SourceWorldSize;
}

bool ParGateMPI::IsRunning() {
	return MPIRunning;
}

GateMPIException::GateMPIException(const char * message) {
	this->message = message;
}

GateMPIException::~GateMPIException() {

}

const char * GateMPIException::GetMsg() {
	return message;
}

#endif
