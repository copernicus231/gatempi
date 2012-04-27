/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#ifdef GATE_USE_OPTICAL

#include "GateFastAnalysis.hh"
#include "GateFastAnalysisMessenger.hh"
#include "GateVVolume.hh"

#include "globals.hh"

#include "G4Run.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4RunManager.hh"
#include "GateCrystalHit.hh"
#include "GateSourceMgr.hh"
#include "GateOutputMgr.hh"

GateFastAnalysis::GateFastAnalysis(const G4String& name, GateOutputMgr* outputMgr, DigiMode digiMode) 
  : GateVOutputModule(name,outputMgr,digiMode)
{
  m_messenger = new GateFastAnalysisMessenger(this);
  SetVerboseLevel(0);
  // this module is disabled by default
  Enable(false);
}

GateFastAnalysis::~GateFastAnalysis() 
{
  delete m_messenger;
  if (nVerboseLevel > 0) G4cout << "GateFastAnalysis deleting..." << G4endl;
}

const G4String& GateFastAnalysis::GiveNameOfFile()
{
  m_noFileName = "  "; // 2 spaces for output module with no fileName
  return m_noFileName;
}

void GateFastAnalysis::RecordBeginOfAcquisition()
{
  if (nVerboseLevel > 2)
    G4cout << "GateFastAnalysis::RecordBeginOfAcquisition" << G4endl;
}

void GateFastAnalysis::RecordEndOfAcquisition()
{
  if (nVerboseLevel > 2)
    G4cout << "GateFastAnalysis::RecordEndOfAcquisition" << G4endl;
}

void GateFastAnalysis::RecordBeginOfRun(const G4Run * )
{
  if (nVerboseLevel > 2)
    G4cout << "GateFastAnalysis::RecordBeginOfRun" << G4endl;
}

void GateFastAnalysis::RecordEndOfRun(const G4Run * )
{
  if (nVerboseLevel > 2)
    G4cout << "GateFastAnalysis::RecordEndOfRun" << G4endl;
}

void GateFastAnalysis::RecordBeginOfEvent(const G4Event* )
{
  if (nVerboseLevel > 2)
    G4cout << "GateFastAnalysis::RecordBeginOfEvent" << G4endl;
}

void GateFastAnalysis::RecordEndOfEvent(const G4Event* event)
{

 if (nVerboseLevel > 2)
    G4cout << "GateFastAnalysis::RecordEndOfEvent" << G4endl;

  GateCrystalHitsCollection* CHC = GetOutputMgr()->GetCrystalHitCollection();

  if (CHC)
  {   
    G4int NbHits = CHC->entries();
  
    G4int sourceID = (((GateSourceMgr::GetInstance())->GetSourcesForThisEvent())[0])->GetSourceID();
    G4int eventID  = event->GetEventID();
    G4int runID    = G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID();

    for (G4int iHit = 0; iHit < NbHits; iHit++)
    {
      if ((*CHC)[iHit]->GoodForAnalysis())
      {
	(*CHC)[iHit]->SetSourceID(sourceID);
	(*CHC)[iHit]->SetEventID(eventID);
	(*CHC)[iHit]->SetRunID(runID);
	// the following parameters are not calculated and are therefore set to -1
	// or "NULL"  to indicate no value
	(*CHC)[iHit]->SetSourcePosition(-1);
	(*CHC)[iHit]->SetNPhantomCompton(-1);
	(*CHC)[iHit]->SetNPhantomRayleigh(-1);
	(*CHC)[iHit]->SetComptonVolumeName("NULL");
	(*CHC)[iHit]->SetRayleighVolumeName("NULL");
	(*CHC)[iHit]->SetPhotonID(-1);
	(*CHC)[iHit]->SetPrimaryID(-1);
	(*CHC)[iHit]->SetNCrystalCompton(-1); 
	(*CHC)[iHit]->SetNCrystalRayleigh(-1);
      }
    }
  }
}

void GateFastAnalysis::RecordStepWithVolume(const GateVVolume * v, const G4Step* )
{
  if (nVerboseLevel > 2)
    G4cout << "GateFastAnalysis::RecordStep" << G4endl;
}

void GateFastAnalysis::SetVerboseLevel(G4int val) 
{ nVerboseLevel = val;}

#endif
