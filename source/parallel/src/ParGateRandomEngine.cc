/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateRandomEngine.cc
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#include "ParGateRandomEngine.hh"
#include "CLHEP/Random/SprngRandom.h"
#include "globals.hh"
#include "ParGateMPI.hh"

ParGateRandomEngine::ParGateRandomEngine() :
		GateRandomEngine() {

}

ParGateRandomEngine::~ParGateRandomEngine() {

}

void ParGateRandomEngine::Initialize() {
	if (theRandomEngine->name() != "SprngRandom") { //TODO MAYBE TAKE OTHER ACTION IF IS A NON-SPRNG ENGINE
		delete theRandomEngine;
		SetRandomEngine("SprngMLFG");
	}
	if(theSeedFile != " "){
		std::stringstream sNumber;
		sNumber << ParGateMPI::GetInstance()->GetSourceWorldRank();
		theSeedFile.append(sNumber.str());
	}
	GateRandomEngine::Initialize();
}

void ParGateRandomEngine::saveEngineInto(const G4String& file){
	G4String filePerProcess;
	std::stringstream sNumber;
	sNumber << ParGateMPI::GetInstance()->GetSourceWorldRank();
	filePerProcess=file;
	filePerProcess.append(sNumber.str());
	GateRandomEngine::saveEngineInto(filePerProcess);
}

void ParGateRandomEngine::SetRandomEngine(const G4String& aName) {
	int number_streams = ParGateMPI::GetInstance()->GetSourceWorldSize();
	int stream_id = ParGateMPI::GetInstance()->GetSourceWorldRank();
	using namespace CLHEP;
	if (aName == "SprngLFG") {
		delete theRandomEngine;
		theRandomEngine = new SprngRandom(SprngRandom::LFG, number_streams,
				stream_id);
	} else if (aName == "SprngLCG") {
		delete theRandomEngine;
		theRandomEngine = new SprngRandom(SprngRandom::LCG, number_streams,
				stream_id);
	} else if (aName == "SprngLCG64") {
		delete theRandomEngine;
		theRandomEngine = new SprngRandom(SprngRandom::LCG64, number_streams,
				stream_id);
	} else if (aName == "SprngCMRG") {
		delete theRandomEngine;
		theRandomEngine = new SprngRandom(SprngRandom::CMRG, number_streams,
				stream_id);
	} else if (aName == "SprngMLFG") {
		delete theRandomEngine;
		theRandomEngine = new SprngRandom(SprngRandom::MLFG, number_streams,
				stream_id);
	} else if (aName == "SprngPMLCG") {
		delete theRandomEngine;
		theRandomEngine = new SprngRandom(SprngRandom::PMLCG, number_streams,
				stream_id);
	} else {
		G4Exception(
				"\n!!! GateRandomEngine::SetRandomEngine: "
						"Unknown random engine '" + aName
						+ "'. Computation aborted !!!\n");
	}
}

#endif
