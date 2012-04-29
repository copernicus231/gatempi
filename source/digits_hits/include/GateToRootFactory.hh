/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateToRootFactory.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef G4ANALYSIS_USE_ROOT

#ifndef GATETOROOTFACTORY_HH_
#define GATETOROOTFACTORY_HH_

#include "GateToRoot.hh"

class GateToRootFactory {
public:
	static GateToRoot* GetGateToRoot(const G4String& name,
			GateOutputMgr* outputMgr, DigiMode digiMode);
};

#endif /* GATETOROOTFACTORY_HH_ */

#endif
