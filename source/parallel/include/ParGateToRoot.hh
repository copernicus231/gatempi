/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateToRoot.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#ifdef G4ANALYSIS_USE_ROOT

#ifndef PARGATETOROOT_HH_
#define PARGATETOROOT_HH_

#include "GateToRoot.hh"

class ParGateToRoot: public GateToRoot {
protected:
	ParGateToRoot(const G4String& name, GateOutputMgr* outputMgr,
			DigiMode digiMode);

public:
	virtual ~ParGateToRoot();
	void SetFileName(const G4String aName);

private:
	friend class GateToRootFactory;
};

#endif /* PARGATETOROOT_HH_ */

#endif

#endif
