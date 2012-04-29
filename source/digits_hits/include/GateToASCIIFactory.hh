/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateToASCIIFactory.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef G4ANALYSIS_USE_FILE

#ifndef GATETOASCIIFACTORY_HH_
#define GATETOASCIIFACTORY_HH_

#include "GateToASCII.hh"

class GateToASCIIFactory {
public:
	static GateToASCII* GetGateToASCII(const G4String& name,
			GateOutputMgr* outputMgr, DigiMode digiMode);
};

#endif /* GATETOASCIIFACTORY_HH_ */

#endif
