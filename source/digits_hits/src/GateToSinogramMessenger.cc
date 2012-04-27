/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#include "GateToSinogramMessenger.hh"
#include "GateToSinogram.hh"
#include "GateOutputMgr.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"





GateToSinogramMessenger::GateToSinogramMessenger(GateToSinogram* gateToSinogram)
  : GateOutputModuleMessenger(gateToSinogram)
  , m_gateToSinogram(gateToSinogram)
{ 
  G4String cmdName;

  cmdName = GetDirectoryName()+"setFileName";
  SetFileNameCmd = new G4UIcmdWithAString(cmdName,this);
  SetFileNameCmd->SetGuidance("Set the name of the output raw sinogram files");
  SetFileNameCmd->SetParameterName("Name",false);

  cmdName = GetDirectoryName()+"TruesOnly";
  TruesOnlyCmd = new G4UIcmdWithABool(cmdName,this);
  TruesOnlyCmd->SetGuidance("Record only true coincidences");
  TruesOnlyCmd->SetParameterName("flag",true);
  TruesOnlyCmd->SetDefaultValue(true);
  
  cmdName = GetDirectoryName()+"RadialBins";
  SetRadialElemNbCmd = new G4UIcmdWithAnInteger(cmdName,this);
  SetRadialElemNbCmd->SetGuidance("Set the number of radial sinogram bins");
  SetRadialElemNbCmd->SetParameterName("Number",false);
  SetRadialElemNbCmd->SetRange("Number>0");

  cmdName = GetDirectoryName()+"RawOutputEnable";
  RawOutputCmd = new G4UIcmdWithABool(cmdName,this);
  RawOutputCmd->SetGuidance("Enables 2D sinograms output in a raw file");
  RawOutputCmd->SetParameterName("flag",true);
  RawOutputCmd->SetDefaultValue(true);

  cmdName = GetDirectoryName()+"setInputDataName";
  SetInputDataCmd = new G4UIcmdWithAString(cmdName,this);
  SetInputDataCmd->SetGuidance("Set the name of the input data to store into the sinogram");
  SetInputDataCmd->SetParameterName("Name",false);
  
  cmdName = GetDirectoryName()+"setTangCrystalBlurring";
  SetTangCrystalResolCmd = new G4UIcmdWithADoubleAndUnit(cmdName,this);
  SetTangCrystalResolCmd->SetGuidance("Set the crystal location blurring FWHM in the tangential direction");
  SetTangCrystalResolCmd->SetParameterName("Number",false);
  SetTangCrystalResolCmd->SetRange("Number>=0.");
  SetTangCrystalResolCmd->SetUnitCategory("Length");

  cmdName = GetDirectoryName()+"setAxialCrystalBlurring";
  SetAxialCrystalResolCmd = new G4UIcmdWithADoubleAndUnit(cmdName,this);
  SetAxialCrystalResolCmd->SetGuidance("Set the crystal location blurring FWHM in the axial direction");
  SetAxialCrystalResolCmd->SetParameterName("Number",false);
  SetAxialCrystalResolCmd->SetRange("Number>=0.");
  SetAxialCrystalResolCmd->SetUnitCategory("Length");
  

  // 07.02.2006, C. Comtat, Store randoms and scatters sino
  cmdName = GetDirectoryName()+"StoreDelayeds";
  StoreDelayedsCmd = new G4UIcmdWithABool(cmdName,this);
  StoreDelayedsCmd->SetGuidance("Record delayed coincidences with data=1 and prompt coincidences with data=0");
  StoreDelayedsCmd->SetParameterName("flag",true);
  StoreDelayedsCmd->SetDefaultValue(true);
  cmdName = GetDirectoryName()+"StoreScatters";
  StoreScattersCmd = new G4UIcmdWithABool(cmdName,this);
  StoreScattersCmd->SetGuidance("Record scattered coincidences with data=4");
  StoreScattersCmd->SetParameterName("flag",true);
  StoreScattersCmd->SetDefaultValue(true);


}
GateToSinogramMessenger::~GateToSinogramMessenger()
{
  delete SetFileNameCmd;
  delete TruesOnlyCmd;
  delete SetRadialElemNbCmd;
  delete RawOutputCmd;
  delete SetTangCrystalResolCmd;
  delete SetAxialCrystalResolCmd;
  delete SetInputDataCmd;
  
  // 07.02.2006, C. Comtat, Store randoms and scatters sino
  delete StoreDelayedsCmd;
  delete StoreScattersCmd;
}

void GateToSinogramMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if (command == SetFileNameCmd) 
    { m_gateToSinogram->SetFileName(newValue); }
  else if ( command==TruesOnlyCmd )
    { m_gateToSinogram->TruesOnly(TruesOnlyCmd->GetNewBoolValue(newValue)) ; }
  else if ( command==SetRadialElemNbCmd )
    { m_gateToSinogram->SetRadialElemNb(SetRadialElemNbCmd->GetNewIntValue(newValue)); }
  else if ( command==RawOutputCmd )
    { m_gateToSinogram->RawOutputEnable(RawOutputCmd->GetNewBoolValue(newValue)); }   
  else if ( command==SetTangCrystalResolCmd )
    { m_gateToSinogram->SetTangCrystalResolution(SetTangCrystalResolCmd->GetNewDoubleValue(newValue)); }  
  else if ( command==SetAxialCrystalResolCmd )
    { m_gateToSinogram->SetAxialCrystalResolution(SetAxialCrystalResolCmd->GetNewDoubleValue(newValue)); }  
  else if (command == SetInputDataCmd) 
    { m_gateToSinogram->SetOutputDataName(newValue); }
    
  // 07.02.2006, C. Comtat, Store randoms and scatters sino
  else if (command == StoreDelayedsCmd)
    { m_gateToSinogram->StoreDelayeds(StoreDelayedsCmd->GetNewBoolValue(newValue)) ; }
  else if (command == StoreScattersCmd)
    { m_gateToSinogram->StoreScatters(StoreScattersCmd->GetNewBoolValue(newValue)) ; }
      
  else 
    { GateOutputModuleMessenger::SetNewValue(command,newValue); }   
}
