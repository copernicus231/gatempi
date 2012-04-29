/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateToASCII.cc
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#ifdef G4ANALYSIS_USE_FILE

#include "ParGateToASCII.hh"
#include "ParGateMPI.hh"
#include <sstream>

ParGateToASCII::ParGateToASCII(const G4String& name, GateOutputMgr* outputMgr,
		DigiMode digiMode) :
		GateToASCII(name, outputMgr, digiMode) {

}

ParGateToASCII::~ParGateToASCII() {

}

void ParGateToASCII::SetFileName(const G4String aName) {
	std::stringstream sNumber;
	sNumber << ParGateMPI::GetInstance()->GetSourceWorldRank();
	GateToASCII::SetFileName(aName + sNumber.str());
}

#endif

#endif
