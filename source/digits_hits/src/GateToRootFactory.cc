/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateToRootFactory.cc
 \author copernicus email:copernicus231@gmail.com
 */
#ifdef G4ANALYSIS_USE_ROOT

#include "GateToRootFactory.hh"
#include "GateToRoot.hh"

#ifdef GATE_PARALLEL_MPI

#include "ParGateToRoot.hh"

GateToRoot* GateToRootFactory::GetGateToRoot(const G4String& name,
		GateOutputMgr* outputMgr, DigiMode digiMode) {
	if (ParGateMPI::GetInstance()->IsRunning()) {
		return new ParGateToRoot(name, outputMgr, digiMode);
	} else {
		return new GateToRoot(name, outputMgr, digiMode);
	}
}

#else

GateToRoot* GateToRootFactory::GetGateToRoot(const G4String& name,
		GateOutputMgr* outputMgr, DigiMode digiMode) {
	return new GateToRoot(name, outputMgr, digiMode);
}

#endif
#endif
