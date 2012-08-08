/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateToRoot.cc
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#ifdef G4ANALYSIS_USE_ROOT

#include "ParGateToRoot.hh"
#include "ParGateMPI.hh"
#include <sstream>
#include "GateOutputMgr.hh"

ParGateToRoot::ParGateToRoot(const G4String& name, GateOutputMgr* outputMgr,
		DigiMode digiMode) :
		GateToRoot(name, outputMgr, digiMode) {

}
ParGateToRoot::~ParGateToRoot() {

}

void ParGateToRoot::Enable(G4bool val){
	if(ParGateMPI::GetInstance()->GetSourceWorldRank() == 0){
		GateToRoot::Enable(false);
		m_outputMgr->AllowNoOutput();
		return;
	}
	GateToRoot::Enable(val);
}

void ParGateToRoot::SetFileName(const G4String aName) {
	if(ParGateMPI::GetInstance()->GetSourceWorldRank() != 0){
		std::stringstream sNumber;
		sNumber << ParGateMPI::GetInstance()->GetSourceWorldRank();
		GateToRoot::SetFileName(aName + sNumber.str());
	}
}

#endif

#endif
