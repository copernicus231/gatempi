/*----------------------
  GATE version name: gate_v6

  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/
  
#include "GateRandomEngineMessenger.hh"
#include "GateRandomEngine.hh"
#include "CLHEP/Random/defs.h"
#include "CLHEP/Random/Random.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/MTwistEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include <time.h>
#include <unistd.h>
#include "GateMessageManager.hh"

GateRandomEngine* GateRandomEngine::instance = 0;

///////////////////
//  Constructor  //
///////////////////

//!< Constructor
GateRandomEngine::GateRandomEngine()
{
  // Default
  //theRandomEngine = new CLHEP::MTwistEngine();
  theRandomEngine = new CLHEP::HepJamesRandom();
  theVerbosity = 0;
  theSeed="default";
  theSeedFile=" ";
  // Create the messenger
  theMessenger = new GateRandomEngineMessenger(this);  
  

}

//////////////////
//  Destructor  //
//////////////////

//!< Destructor
GateRandomEngine::~GateRandomEngine()
{
  delete theRandomEngine;
  delete theMessenger;
}

///////////////////////
//  SetRandomEngine  //
///////////////////////

//!< void SetRandomEngine
void GateRandomEngine::SetRandomEngine(const G4String& aName) {
  //--- Here is the list of the allowed random engines to be used ---//
  if (aName=="JamesRandom") {
    delete theRandomEngine;
    theRandomEngine = new CLHEP::HepJamesRandom();
  }
  else if (aName=="Ranlux64") {
    delete theRandomEngine;
    theRandomEngine = new CLHEP::Ranlux64Engine();
  }
  else if (aName=="MersenneTwister") {
    delete theRandomEngine;
    theRandomEngine = new CLHEP::MTwistEngine();
  }
  else {
    G4Exception("\n!!! GateRandomEngine::SetRandomEngine: "
                "Unknown random engine '"+aName+"'. Computation aborted !!!\n");
  }


}

/////////////////////
//  SetEngineSeed  //
/////////////////////

//!< void SetEngineSeed
void GateRandomEngine::SetEngineSeed(const G4String& value) {
  theSeed = value;
}

/////////////////////
//  SetEngineSeed from file //
/////////////////////
//!< void resetEngineFrom
void GateRandomEngine::resetEngineFrom(const G4String& file) { //TC
  theSeedFile = file;
} 


//////////////////
//  ShowStatus  //
//////////////////

//!< void ShowStatus
void GateRandomEngine::ShowStatus() {
  CLHEP::HepRandom::showEngineStatus();
}

//////////////////
//  Initialize  //
//////////////////

//!< void Initialize
void GateRandomEngine::Initialize() {
  bool isSeed = false;
  G4int seed=0;
  if (theSeed=="default" && theSeedFile==" ") isSeed=false;
  else if (theSeed=="auto") {
    seed = time(NULL)*getpid();
    if (seed<0) seed*=(-1);
    isSeed=true;
  }
  else {
    seed = atoi(theSeed.c_str());
    if (seed<0) seed*=(-1);
    isSeed=true;
  }
  if (isSeed)
  {
    if(theSeedFile !=" " && theSeed !="default") //TC
      G4Exception("ERROR !! => Please: choose between a status file and a seed (defined by a number) !"); //TC
    if(theSeedFile == " ") //TC
    {
      G4int rest = seed%10+seed%100+1;
      theRandomEngine->setSeed(seed,rest);
    }
    else theRandomEngine->restoreStatus(theSeedFile); //TC 
  }
  // True initialization
  CLHEP::HepRandom::setTheEngine(theRandomEngine);
}

