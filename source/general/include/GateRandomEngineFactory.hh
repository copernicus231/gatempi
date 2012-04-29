/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GateRandomEngineFactory.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifndef GATERANDOMENGINEFACTORY_HH_
#define GATERANDOMENGINEFACTORY_HH_
#include "GateRandomEngine.hh"

class GateRandomEngineFactory {
public:
	static GateRandomEngine* GetRandomEngine();
private:
	static GateRandomEngine* instance;
};

#endif /* GATERANDOMENGINEFACTORY_HH_ */
