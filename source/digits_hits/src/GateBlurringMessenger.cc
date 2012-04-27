/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#include "GateBlurringMessenger.hh"

#include "GateBlurring.hh"

#include  "G4UIcmdWithADoubleAndUnit.hh"

#include "G4UIcmdWithADouble.hh"

GateBlurringMessenger::GateBlurringMessenger(GateBlurring* itsResolution)
    : GatePulseProcessorMessenger(itsResolution)
{
  G4String guidance;
  G4String cmdName;
  G4String cmdName2;

  cmdName = GetDirectoryName() + "setResolution";
  resolutionCmd = new G4UIcmdWithADouble(cmdName,this);
  resolutionCmd->SetGuidance("Set the resolution in energie for gaussian blurring");

  cmdName2 = GetDirectoryName() + "setEnergyOfReference";
  erefCmd = new G4UIcmdWithADoubleAndUnit(cmdName2,this);
  erefCmd->SetGuidance("Set the energy of reference (in keV) for the selected resolution");
  erefCmd->SetUnitCategory("Energy");
}


GateBlurringMessenger::~GateBlurringMessenger()
{
  delete resolutionCmd;
  delete erefCmd;
}


void GateBlurringMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if ( command==resolutionCmd )
    { GetBlurring()->SetResolution(resolutionCmd->GetNewDoubleValue(newValue)); }   
  else if ( command==erefCmd )
    { GetBlurring()->SetRefEnergy(erefCmd->GetNewDoubleValue(newValue)); }   
  else
    GatePulseProcessorMessenger::SetNewValue(command,newValue); 
}


