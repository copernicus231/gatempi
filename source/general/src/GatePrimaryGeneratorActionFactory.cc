/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GatePrimaryGeneratorActionFactory.cc
 \author copernicus email:copernicus231@gmail.com
 */

#include "GatePrimaryGeneratorActionFactory.hh"

#ifdef GATE_PARALLEL_MPI

#include "ParGatePrimaryGeneratorAction.hh"
#include "ParGateMPI.hh"

GatePrimaryGeneratorAction *GatePrimaryGeneratorActionFactory::GetPrimaryGeneratorAction() {
	if (ParGateMPI::GetInstance()->IsRunning()) {
		return new ParGatePrimaryGeneratorAction();
	}
	return new GatePrimaryGeneratorAction();
}

#else

GatePrimaryGeneratorAction *GatePrimaryGeneratorActionFactory::GetPrimaryGeneratorAction() {
	return new GatePrimaryGeneratorAction();
}

#endif
