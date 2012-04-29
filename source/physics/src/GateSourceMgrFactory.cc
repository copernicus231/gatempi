/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateSourceMgrFactory.cc
 \author copernicus email:copernicus231@gmail.com
 */

#include "GateSourceMgrFactory.hh"
#include "GateSourceMgr.hh"

#ifdef GATE_PARALLEL_MPI

#include "ParGateSourceMgr.hh"
#include "ParGateMPI.hh"

GateSourceMgr *GateSourceMgrFactory::GetSourceManager() {
	if (instance == 0) {
		if (ParGateMPI::GetInstance()->IsRunning()) {
			instance = new ParGateSourceMgr;
		} else {
			instance = new GateSourceMgr;
		}
	}
	return instance;
}

#else

GateSourceMgr *GateSourceMgrFactory::GetSourceManager() {
	if (instance == 0) {
		instance = new GateSourceMgr;
	}
	return instance;
}

#endif

GateSourceMgr * GateSourceMgrFactory::instance = 0;
