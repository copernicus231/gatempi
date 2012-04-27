/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#include "GateSystemComponentMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

#include "GateSystemComponent.hh"
#include "GateObjectStore.hh"
#include "GateVSystem.hh"
#include "GateVVolume.hh"


// Constructor
GateSystemComponentMessenger::GateSystemComponentMessenger(GateSystemComponent* itsSystemComponent)
: GateClockDependentMessenger( itsSystemComponent)
{ 
  
  G4String guidance;
  G4String cmdName;

  SetDirectoryGuidance(G4String("Controls the system-component '") + itsSystemComponent->GetObjectName() + "'" );

  cmdName = GetDirectoryName() + "attach";
    
  AttachCmd = new G4UIcmdWithAString(cmdName,this);
  AttachCmd->SetGuidance("Attach a new volume to the system-component.");
  AttachCmd->SetParameterName("choice",false);

}



// Destructor
GateSystemComponentMessenger::~GateSystemComponentMessenger()
{
  delete AttachCmd;
}



// UI command interpreter method
void GateSystemComponentMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if( command==AttachCmd )
    { 
    AddCreator(newValue); }   
  else
    GateClockDependentMessenger::SetNewValue(command,newValue);
}





// Method to apply the UI command 'attach'
// Finds an creator from its name and attaches this creator to the system component
void GateSystemComponentMessenger::AddCreator(const G4String& creatorName)
{ 
  
  // Find the creator from the creator's name
  GateVVolume* anCreator = GateObjectStore::GetInstance()->FindCreator(creatorName);
  
//  G4cout << " GateObjectStore::GetInstance()->FindCreator(creatorName) "  << G4endl;
  
  // If an creator was found, ask the system component to perform the attachement  
  if (anCreator) 
    {  GetSystemComponent()->SetCreator(anCreator); }
  else
    G4cerr  << "[GateSystemComponentMessenger]: " << G4endl
      	    << "could not find a volume creator for the name '" << creatorName << "'" << G4endl
	    << "Attachment request will be ignored!" << G4endl << G4endl;
}



