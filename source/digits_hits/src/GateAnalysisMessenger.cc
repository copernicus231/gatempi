/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#include "GateAnalysisMessenger.hh"
#include "GateAnalysis.hh"
#include "GateOutputMgr.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

GateAnalysisMessenger::GateAnalysisMessenger(GateAnalysis* gateAnalysis)
  : GateOutputModuleMessenger(gateAnalysis)
  , m_gateAnalysis(gateAnalysis)
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

GateAnalysisMessenger::~GateAnalysisMessenger()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void GateAnalysisMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  GateOutputModuleMessenger::SetNewValue(command,newValue);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

