/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateRunManagerFactory.cc
 \author copernicus email:copernicus231@gmail.com
 */

#include "GateRunManagerFactory.hh"
#include "GateRunManager.hh"

#ifdef GATE_PARALLEL_MPI

#include "ParGateRunManager.hh"
#include "ParGateMPI.hh"

GateRunManager *GateRunManagerFactory::GetRunManager() {
	if (ParGateMPI::GetInstance()->IsRunning()) {
		return new ParGateRunManager();
	}
	return new GateRunManager();
}

#else

GateRunManager *GateRunManagerFactory::GetRunManager() {
	return new GateRunManager();
}

#endif
