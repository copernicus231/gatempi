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
#include "GateOutputMgr.hh"

ParGateToASCII::ParGateToASCII(const G4String& name, GateOutputMgr* outputMgr,
		DigiMode digiMode) :
		GateToASCII(name, outputMgr, digiMode) {

}

ParGateToASCII::~ParGateToASCII() {

}

void ParGateToASCII::Enable(G4bool val){
	if(ParGateMPI::GetInstance()->GetSourceWorldRank() == 0){
		GateToASCII::Enable(false);
		m_outputMgr->AllowNoOutput();
		return;
	}
	GateToASCII::Enable(val);
}

void ParGateToASCII::SetFileName(const G4String aName) {
	if(ParGateMPI::GetInstance()->GetSourceWorldRank() != 0){
		std::stringstream sNumber;
		sNumber << ParGateMPI::GetInstance()->GetSourceWorldRank();
		GateToASCII::SetFileName(aName + sNumber.str());
	}
}

#endif

#endif
