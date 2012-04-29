/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateRandomEngine.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#ifndef PARGATERANDOMENGINE_HH_
#define PARGATERANDOMENGINE_HH_

#include "GateRandomEngine.hh"
#include "G4Types.hh"

class ParGateRandomEngine: public GateRandomEngine {
protected:
	ParGateRandomEngine();

public:
	virtual ~ParGateRandomEngine();
	virtual void Initialize();
	virtual void SetRandomEngine(const G4String& aName);

private:
	friend class GateRandomEngineFactory;
};

#endif /* PARGATERANDOMENGINE_HH_ */

#endif
