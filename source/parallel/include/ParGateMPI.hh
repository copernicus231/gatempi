/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateMPI.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#ifndef PARGATEMPI_HH_
#define PARGATEMPI_HH_

#include <mpi.h>

//This class is defined for maintains the information relevant to any process on the mpi environment
class ParGateMPI {
protected:
	virtual ~ParGateMPI();
	ParGateMPI(MPI_Comm *eventWorldComm, MPI_Comm *sourceWorldComm);
	ParGateMPI();

public:
	static ParGateMPI* GetInstance(int argc, char** argv);
	static ParGateMPI* GetInstance();
	static void DestroyInstance();

	int GetEventWorldRank();
	int GetEventWorldSize();
	int GetSourceWorldRank();
	int GetSourceWorldSize();
	int GetNumberOfEvents();

	bool IsRunning();
	void SetNumberOfEvents(int size);
	MPI_Comm& GetEventWorldComm();
	MPI_Comm& GetSourceWorldComm();

private:
	static ParGateMPI* instance;
	MPI_Comm* EventWorldComm;
	MPI_Comm* SourceWorldComm;
	int EventWorldRank;
	int SourceWorldRank;
	int EventWorldSize;
	int SourceWorldSize;
	int NumberOfEvents;
	bool MPIRunning;
};

class GateMPIException {
public:
	GateMPIException(const char * message);
	virtual ~GateMPIException();
	const char * GetMsg();
private:
	const char *message;
};

#endif /* PARGATEMPI_HH_ */

#endif
