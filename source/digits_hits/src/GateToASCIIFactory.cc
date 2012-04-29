/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateToASCIIFactory.cc
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef G4ANALYSIS_USE_FILE

#include "GateToASCIIFactory.hh"
#include "GateToASCII.hh"

#ifdef GATE_PARALLEL_MPI

#include "ParGateToASCII.hh"

GateToASCII* GateToASCIIFactory::GetGateToASCII(const G4String& name,
		GateOutputMgr* outputMgr, DigiMode digiMode) {
	if (ParGateMPI::GetInstance()->IsRunning()) {
		return new ParGateToASCII(name, outputMgr, digiMode);
	} else {
		return new GateToASCII(name, outputMgr, digiMode);
	}
}

#else

GateToASCII* GateToASCIIFactory::GetGateToASCII(const G4String& name,
		GateOutputMgr* outputMgr, DigiMode digiMode) {
	return new GateToASCII(name, outputMgr, digiMode);
}

#endif

#endif
