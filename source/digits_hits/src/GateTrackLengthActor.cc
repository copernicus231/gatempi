/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/
#ifdef G4ANALYSIS_USE_ROOT

/*
  \brief Class GateTrackLengthActor : 
  \brief 
 */

#ifndef GATETRACKLENGTHACTOR_CC
#define GATETRACKLENGTHACTOR_CC

#include "GateTrackLengthActor.hh"

#include "GateMiscFunctions.hh"

//-----------------------------------------------------------------------------
/// Constructors (Prototype)
GateTrackLengthActor::GateTrackLengthActor(G4String name, G4int depth):
  GateVActor(name,depth)
{
  GateDebugMessageInc("Actor",4,"GateTrackLengthActor() -- begin"<<G4endl);

   mLmin = 0.;
   mLmax = 50.;
   mNBins = 10;


  pMessenger = new GateTrackLengthActorMessenger(this);

  GateDebugMessageDec("Actor",4,"GateTrackLengthActor() -- end"<<G4endl);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/// Destructor 
GateTrackLengthActor::~GateTrackLengthActor() 
{
  GateDebugMessageInc("Actor",4,"~GateTrackLengthActor() -- begin"<<G4endl);
 


  GateDebugMessageDec("Actor",4,"~GateTrackLengthActor() -- end"<<G4endl);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Construct
void GateTrackLengthActor::Construct()
{
  GateVActor::Construct();

  // Enable callbacks
  EnableBeginOfRunAction(true);
  EnableBeginOfEventAction(false);
  EnablePreUserTrackingAction(false);
  EnableUserSteppingAction(false);
  EnablePostUserTrackingAction(true);

  //mHistName = "Precise/output/TrackLength.root";
   pTfile = new TFile(mSaveFilename,"RECREATE");
   pTrackLength = new TH1D("trackLength","TrackLength",GetNBins(),GetLmin() ,GetLmax() );
   pTrackLength->SetXTitle("Track Length (mm)");

  ResetData();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/// Save data
void GateTrackLengthActor::SaveData()
{
 pTfile->Write();
    pTfile->Close();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void GateTrackLengthActor::ResetData() 
{

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GateTrackLengthActor::BeginOfRunAction(const G4Run *)
{
  GateDebugMessage("Actor", 3, "GateTrackLengthActor -- Begin of Run" << G4endl);
  ResetData();
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
void GateTrackLengthActor::PostUserTrackingAction(const GateVVolume * /*vol*/, const G4Track* aTrack)
{
  pTrackLength->Fill( aTrack->GetTrackLength(),aTrack->GetWeight() );

}
//-----------------------------------------------------------------------------




#endif /* end #define GATETRACKLENGTHACTOR_CC */
#endif
