/*----------------------
  GATE version name: gate_v6

  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/

#include "GateToProjectionSet.hh"

#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

#include "GateSingleDigi.hh"
#include "GateOutputMgr.hh"
#include "GateProjectionSet.hh"
#include "GateToProjectionSetMessenger.hh"
#include "GateToInterfile.hh"
#include "GateTools.hh"
#include "GateVSystem.hh"
#include "GateApplicationMgr.hh"
#include "G4DigiManager.hh"

/*
 *  GateToInterfile is used to write as an output file the result of the GateToProjectionSet module.
 *  This 2 classes are strickly working together.
 *  All macro commands (inherited from GateVOutputModule) of the GateToInterfileMessenger are overloaded to
 *  have no action at all. The enable and disable command, verbose and setFileName are now managed by
 *  GateToProjectionSet. The describe command of GateToInterfile will do nothing. The enable and disable
 *  commands of the GateToProjectionSet class will also enable or disable the GateToProjectionSet module
 *  (the same for the verbose level).
 */

// Public constructor (creates an empty, uninitialised, project set)
GateToProjectionSet::GateToProjectionSet(const G4String& name, GateOutputMgr* outputMgr,GateVSystem* itsSystem,DigiMode digiMode) 
  : GateVOutputModule(name,outputMgr,digiMode)
  , m_projectionSet(0)
  , m_projNb(0)
  , m_headNb(0)
  , m_orbitingStep(0.)
  , m_headAngularPitch(0.)
  , m_projectionPlane("Unknown")
  , m_coordX(1)
  , m_coordY(2)
  , m_studyDuration(0.)
  , m_system(itsSystem)
  , m_inputDataChannel("Singles")
  , m_inputDataChannelID(-1)
{

  m_isEnabled = false; // Keep this flag false: all output are disabled by default
  m_projectionSet = new GateProjectionSet();

  m_messenger = new GateToProjectionSetMessenger(this);
  
  SetVerboseLevel(0);
}





GateToProjectionSet::~GateToProjectionSet() 
{
  delete m_projectionSet;
}


const G4String& GateToProjectionSet::GiveNameOfFile()
{
  m_noFileName = "  "; // 2 spaces for output module with no fileName
  return m_noFileName;
}

// ======================================================================================================
// Functions for messenger commands that have a link with the GateToInterfile class
void GateToProjectionSet::SetOutputFileName(const G4String& aName)
{
  GateToInterfile* interfileModule = dynamic_cast<GateToInterfile*>( GateOutputMgr::GetInstance()->GetModule("interfile") );
  if (!interfileModule) G4Exception("No GateToInterfile module has been constructed, so no output can be possible with GateToProjectionSet");
  interfileModule->SetFileName(aName); // It is the GateToInterfile module that manages the output file
}

void GateToProjectionSet::SetVerboseToProjectionSetAndInterfile(G4int aVerbosity)
{
  GateToInterfile* interfileModule = dynamic_cast<GateToInterfile*>( GateOutputMgr::GetInstance()->GetModule("interfile") );
  if (!interfileModule) G4Exception("No GateToInterfile module has been constructed, so no output can be possible with GateToProjectionSet");
  interfileModule->SetVerboseLevel(aVerbosity); // We update the verbosity level for the GateToInterfile module
  SetVerboseLevel(aVerbosity); // We update the verbosity level for the GateToProjectionSet module
}

void GateToProjectionSet::SendDescribeToProjectionSetAndInterfile()
{
  GateToInterfile* interfileModule = dynamic_cast<GateToInterfile*>( GateOutputMgr::GetInstance()->GetModule("interfile") );
  if (!interfileModule) G4Exception("No GateToInterfile module has been constructed, so no output can be possible with GateToProjectionSet");
  interfileModule->Describe(); // The GateToInterfile module describes itself
  Describe(); // The GateToProjectionSet module describes itself
}

void GateToProjectionSet::SetEnableToProjectionSetAndInterfile()
{
  GateToInterfile* interfileModule = dynamic_cast<GateToInterfile*>( GateOutputMgr::GetInstance()->GetModule("interfile") );
  if (!interfileModule) G4Exception("No GateToInterfile module has been constructed, so no output can be possible with GateToProjectionSet");
  interfileModule->Enable(true); // We enable the GateToInterfile module
  Enable(true); // We enable the GateToProjectionSet module
}

void GateToProjectionSet::SetDisableToProjectionSetAndInterfile()
{
  GateToInterfile* interfileModule = dynamic_cast<GateToInterfile*>( GateOutputMgr::GetInstance()->GetModule("interfile") );
  if (!interfileModule) G4Exception("No GateToInterfile module has been constructed, so no output can be possible with GateToProjectionSet");
  interfileModule->Enable(false); // We disable the GateToInterfile module
  Enable(false); // We disable the GateToProjectionSet module
}
// End of functions for messenger commands that have a link with the GateToInterfile class
// ======================================================================================================


// Initialisation of the projection set
void GateToProjectionSet::RecordBeginOfAcquisition()
{
  if (nVerboseLevel>2) G4cout << "entering [GateToProjectionSet::RecordBeginOfAcquisition]" << G4endl;
  
  m_inputDataChannelID = G4DigiManager::GetDMpointer()->GetDigiCollectionID(m_inputDataChannel);      

  // First, we check that all the parameters are valid
  if ( GetPixelNbX() <= 0 ) {
    G4cerr << 	G4endl << "[GateToProjectionSet::RecordBeginOfAcquisition]:" << G4endl
      	   <<   "Sorry, but the number of pixels along X for the projection-set (" << GetPixelNbX() << ") is invalid" << G4endl;
    G4Exception("You must either define this number with:\n"
		"\t/gate/output/projection/pixelNumberX NUMBER\n"
		"or disable the projection-maker using:\n"
		"\t/gate/output/projection/disable\n");
  }
  if ( GetPixelNbY() <= 0 ) {
     G4cerr << 	G4endl << "[GateToProjectionSet::RecordBeginOfAcquisition]:" << G4endl
      	    <<   "Sorry, but the number of pixels along Y for the projection-set (" << GetPixelNbY() << ") is invalid" << G4endl;
     G4Exception("You must either define this number with:\n"
		"\t/gate/output/projection/pixelNumberX NUMBER\n"
		"or disable the projection-maker using:\n"
		"\t/gate/output/projection/disable\n");
  }
  if ( GetPixelSizeX() <= 0 ) {
    G4cerr  << 	G4endl << "[GateToProjectionSet::RecordBeginOfAcquisition]:" << G4endl
      	    <<   "Sorry, but the pixel size along X for the projection-set (" << G4BestUnit(GetPixelSizeX(),"Length") << ") is invalid" << G4endl;
    G4Exception("You must either define this size with:\n"
		"\t/gate/output/projection/pixelSizeX SIZE UNIT\n"
		"or disable the projection-maker using:\n"
		"\t/gate/output/projection/disable\n");
  }
  if ( GetPixelSizeY() <= 0 ) {
    G4cerr  << 	G4endl << "[GateToProjectionSet::RecordBeginOfAcquisition]:" << G4endl
      	    <<   "Sorry, but the pixel size along Y for the projection-set (" << G4BestUnit(GetPixelSizeY(),"Length") << ") is invalid" << G4endl;
    G4Exception("You must either define this size with:\n"
		"\t/gate/output/projection/pixelSizeY SIZE UNIT\n"
		"or disable the projection-maker using:\n"
		"\t/gate/output/projection/disable\n");
  }
   if ( m_projectionPlane == "Unknown" ) {
    G4cerr  << 	G4endl << "[GateToProjectionSet::RecordBeginOfAcquisition]:" << G4endl
      	    <<   "Sorry, but the pixel size along Y for the projection-set (" << G4BestUnit(GetPixelSizeY(),"Length") << ") is invalid" << G4endl;
    G4Exception("\n[GateToProjectionSet::RecordBeginOfAcquisition]:\n"
      	      	"Sorry, but you have not defined the projection plane.\n"
      	      	"You must either define this plane with:\n"
		"\t/gate/output/projection/projectionPlane PLANE (XY or YZ or ZX )\n"
		"or disable the projection-maker using:\n"
		"\t/gate/output/projection/disable\n");
  }
 
  // Retrieve the parameters of the experiment
  G4double timeStart = GateApplicationMgr::GetInstance()->GetTimeStart();
  G4double timeStop  = GateApplicationMgr::GetInstance()->GetTimeStop();
  G4double timeStep  = GateApplicationMgr::GetInstance()->GetTimeSlice();
  G4double duration  = timeStop-timeStart;
  m_studyDuration = duration;

  G4double fstepNumber = duration / timeStep;
  if ( fabs(fstepNumber-rint(fstepNumber)) >= 1.e-5 ) {
    G4cerr  << 	G4endl << "[GateToProjectionSet::RecordBeginOfAcquisition]:" << G4endl
      	    <<   "Sorry, but the study duration (" << G4BestUnit(duration,"Time") << ") " 
	    <<   " does not seem to be a multiple of the time-slice (" << G4BestUnit(timeStep,"Time") << ")." << G4endl;
    G4Exception("You must change these parameters then restart the simulation\n");
  }
  m_projNb = static_cast<size_t>(rint(fstepNumber));
  if (nVerboseLevel>2) G4cout << "Number of acquisition steps: " << m_projNb << G4endl;
  

  // Retrieve the parameters of the rotation
  GateSystemComponent* baseComponent = m_system->GetBaseComponent();
  G4double orbitingSpeed = baseComponent->GetOrbitingVelocity();
  m_orbitingStep  = orbitingSpeed * timeStep;
  if (!m_orbitingStep) m_orbitingStep=360.*deg;
  if (nVerboseLevel>2) G4cout << "Rotation step: " << m_orbitingStep/degree << " deg" << G4endl;


  // Retrieve the parameters of the repeater (number of heads)
  m_headNb 	      = baseComponent->GetAngularRepeatNumber();
  m_headAngularPitch = baseComponent->GetAngularRepeatPitch();
  if (!m_headAngularPitch) m_headAngularPitch=360.*deg;
  if (nVerboseLevel>2) G4cout << "Angular pitch between heads: " << m_headAngularPitch/degree << " deg" << G4endl;

  // Prepare the projection set 
  m_projectionSet->Reset(m_headNb,m_projNb);

  if (nVerboseLevel>2) G4cout << "leaving [GateToProjectionSet::RecordBeginOfAcquisition]" << G4endl;
}




// We leave the projection set as it is (so that it can be stored afterwards)
// but we still have to destroy the array of projection IDs
void GateToProjectionSet::RecordEndOfAcquisition()
{
}


// Reset the projection data
void GateToProjectionSet::RecordBeginOfRun(const G4Run * r)
{
  m_projectionSet->ClearData(r->GetRunID());
}





// Update the target projections with regards to the digis acquired for this event
void GateToProjectionSet::RecordEndOfEvent(const G4Event* )
{
  if (nVerboseLevel>2)
   G4cout << "entering [GateToProjectionSet::RecordEndOfEvent]" << G4endl;

  G4DigiManager * fDM = G4DigiManager::GetDMpointer();
  const GateSingleDigiCollection * SDC  = 
    (GateSingleDigiCollection*) (fDM->GetDigiCollection( m_inputDataChannelID ));


  if (!SDC) {
    if (nVerboseLevel>2) G4cout << "No digi collection for this event" << G4endl
      	      	      	      	 << "leaving [GateToProjectionSet::RecordEndOfEvent]" << G4endl;
    return;
  }
  
  G4int n_digi =  SDC->entries();
  for (G4int iDigi=0;iDigi<n_digi;iDigi++) {
    G4int headID = m_system->GetMainComponentID( (*SDC)[iDigi]->GetPulse() );
    G4double xProj = (*SDC)[iDigi]->GetLocalPos()[m_coordX];
    G4double yProj = (*SDC)[iDigi]->GetLocalPos()[m_coordY];
    if (nVerboseLevel>=2) {
      G4cout << "[GateToProjectionSet]: Processing count on head " << headID << " at position " << G4BestUnit((*SDC)[iDigi]->GetLocalPos(),"Length") << G4endl;
      G4cout << "Extracting projection coordinates: " << G4BestUnit(xProj,"Length") << " , " << G4BestUnit(yProj,"Length") << G4endl;
    }
    m_projectionSet->Fill( headID, xProj, yProj);
  }    

  if (nVerboseLevel>2)
   G4cout << "leaving [GateToProjectionSet::RecordEndOfEvent]" << G4endl;
}




/* Overload of the base-class' virtual method to print-out a description of the module

	indent: the print-out indentation (cosmetic parameter)
*/    
void GateToProjectionSet::Describe(size_t indent)
{
  GateVOutputModule::Describe(indent);
  G4cout << GateTools::Indent(indent) << "Job:                   build a set of projections from a SPECT simulation" << G4endl;
  G4cout << GateTools::Indent(indent) << "Is enabled?            " << ( IsEnabled() ? "Yes" : "No") << G4endl;
  G4cout << GateTools::Indent(indent) << "Projection plane       '" << m_projectionPlane << "'" << G4endl;
  G4cout << GateTools::Indent(indent) << "Number of pixels       " << GetPixelNbX() << " x " << GetPixelNbY() << G4endl;
  G4cout << GateTools::Indent(indent) << "Pixel size             " << G4BestUnit(GetPixelSizeX(),"Length") << " x " << G4BestUnit(GetPixelSizeY(),"Length") << G4endl;
  G4cout << GateTools::Indent(indent) << "Filled?                " << ( m_projectionSet->GetData() ? "Yes" : "No" ) << G4endl;
  if ( GetProjectionNb() ) G4cout << GateTools::Indent(indent) << "Number of projections: " << GetProjectionNb() << G4endl;
  G4cout << GateTools::Indent(indent) << "Attached to system:    '" << m_system->GetObjectName() << "'" << G4endl;
}





// Set the sampling plane
void GateToProjectionSet::SetProjectionPlane(const G4String& planeName)
{
  if ( planeName == "XY" ) {
    m_projectionPlane = planeName;
    m_coordX = 0;
    m_coordY = 1;
  } else if ( planeName == "YZ" ) {
    m_projectionPlane = planeName;
    m_coordX = 1;
    m_coordY = 2;
  } else if ( planeName == "ZX" ) {
    m_projectionPlane = planeName;
    m_coordX = 2;
    m_coordY = 0;
  } else {
    G4cerr << G4endl  << "[GateToProjectionSet::SetProjectionPlane]:" << G4endl
      	      	    << "\tI did not recognise the plane name '" << planeName << "'" << G4endl
		    << "\tValid names are 'XY', 'YZ' and 'ZX'." << G4endl
		    << "Setting request will be ignored!" << G4endl;
  }
}





