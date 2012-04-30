/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateToASCII.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#ifdef G4ANALYSIS_USE_FILE

#ifndef PARGATETOASCII_HH
#define PARGATETOASCII_HH

#include "GateToASCII.hh"

class ParGateToASCII: public GateToASCII {
protected:
	ParGateToASCII(const G4String& name, GateOutputMgr* outputMgr,
			DigiMode digiMode);

public:
	virtual ~ParGateToASCII();
	void SetFileName(const G4String aName);
private:
	friend class GateToASCIIFactory;
};

#endif /* PARGATETOASCII_HH */

#endif

#endif
