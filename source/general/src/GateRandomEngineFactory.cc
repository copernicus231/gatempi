/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateRandomEngineFactory.cc
 \author copernicus email:copernicus231@gmail.com
 */

#include "GateRandomEngineFactory.hh"
#include "GateRandomEngine.hh"

#ifdef GATE_PARALLEL_MPI

#include "ParGateRandomEngine.hh"
#include "ParGateMPI.hh"

GateRandomEngine* GateRandomEngineFactory::GetRandomEngine() {
	if (instance == 0) {
		if (ParGateMPI::GetInstance()->IsRunning()) {
			instance = new ParGateRandomEngine;
		} else {
			instance = new GateRandomEngine;
		}
	}
	return instance;
}

#else

GateRandomEngine* GateRandomEngineFactory::GetRandomEngine() {
	if(instance == 0) {
		instance = new GateRandomEngine;
	}
	return instance;
}

#endif

GateRandomEngine* GateRandomEngineFactory::instance = 0;
