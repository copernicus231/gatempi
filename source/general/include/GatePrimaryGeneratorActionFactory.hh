/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  GatePrimaryGeneratorActionFactory.hh
 \author copernicus email:copernicus231@gmail.com
 */

#ifndef GATEPRIMARYGENERATORACTIONFACTORY_HH_
#define GATEPRIMARYGENERATORACTIONFACTORY_HH_
#include "GatePrimaryGeneratorAction.hh"

class GatePrimaryGeneratorActionFactory {
public:
	static GatePrimaryGeneratorAction *GetPrimaryGeneratorAction();
};

#endif /* GATEPRIMARYGENERATORACTIONFACTORY_HH_ */
