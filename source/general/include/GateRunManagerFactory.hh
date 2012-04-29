/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateRunManagerFactory.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifndef GATERUNMANAGERFACTORY_HH_
#define GATERUNMANAGERFACTORY_HH_
#include "GateRunManager.hh"

class GateRunManagerFactory {
public:
	static GateRunManager *GetRunManager();
};

#endif /* GATERUNMANAGERFACTORY_HH_ */
