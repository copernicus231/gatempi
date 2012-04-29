/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGatePrimaryGeneratorAction.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#ifndef PARGATEPRIMARYGENERATORACTION_HH_
#define PARGATEPRIMARYGENERATORACTION_HH_

#include "GatePrimaryGeneratorAction.hh"

class ParGatePrimaryGeneratorAction: public GatePrimaryGeneratorAction {
protected:
	ParGatePrimaryGeneratorAction();

public:
	virtual ~ParGatePrimaryGeneratorAction();
	virtual void GenerateSimulationPrimaries(G4Event* event);

private:
	friend class GatePrimaryGeneratorActionFactory;
};

#endif /* PARGATEPRIMARYGENERATORACTION_HH_ */

#endif
