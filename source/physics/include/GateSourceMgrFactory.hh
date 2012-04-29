/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateSourceMgrFactory.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifndef GATESOURCEMGRFACTORY_HH_
#define GATESOURCEMGRFACTORY_HH_
#include "GateSourceMgr.hh"

class GateSourceMgrFactory {
public:
	static GateSourceMgr *GetSourceManager();
private:
	static GateSourceMgr* instance;
};

#endif /* GATESOURCEMGRFACTORY_HH_ */
