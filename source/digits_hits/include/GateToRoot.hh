/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#ifndef GateToRoot_H
#define GateToRoot_H

#ifdef G4ANALYSIS_USE_ROOT

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TNtuple.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TBranch.h"

#include "globals.hh"
#include <fstream>

#include "G4Run.hh"
#include "G4Step.hh"
#include "G4Event.hh"

#include "GateRootDefs.hh"
#include "GateVOutputModule.hh"

/* PY Descourt 08/09/2009 */
#include "GateActions.hh"
#include "GateTrack.hh"

class GateToRootMessenger;
class GateVVolume;

 class ComptonRayleighData
{ public:
  G4int photon1_phantom_Rayleigh,photon2_phantom_Rayleigh;
  G4int photon1_phantom_compton , photon2_phantom_compton;
  Char_t theComptonVolumeName1[60] , theComptonVolumeName2[60], theRayleighVolumeName1[60],theRayleighVolumeName2[60];
  ComptonRayleighData( ComptonRayleighData &);
  ComptonRayleighData & operator = ( const ComptonRayleighData & );
  ComptonRayleighData();
};
//--------------------------------------------------------------------------
class GateToRoot :  public GateVOutputModule
{
public:

  GateToRoot(const G4String& name, GateOutputMgr* outputMgr,DigiMode digiMode);
  virtual ~GateToRoot();
  const G4String& GiveNameOfFile();

  void RecordBeginOfAcquisition();
  void RecordEndOfAcquisition();
  void RecordBeginOfRun(const G4Run *);
  void RecordEndOfRun(const G4Run *);
  void RecordBeginOfEvent(const G4Event *);
  void RecordEndOfEvent(const G4Event *);
  void RecordStepWithVolume(const GateVVolume * v, const G4Step *);

  //! saves the geometry voxel information
  void RecordVoxels(GateVGeometryVoxelStore *);

  void RecordDigitizer(const G4Event *);

  void RecordVoxels(const G4Step *);

  void Book();
  void Store();
  void Reset();

  G4int GetRecordFlag()           { return m_recordFlag; };
  void  SetRecordFlag(G4int flag) { m_recordFlag = flag; };
  
  /* PY Descourt 08/09/2009 */
  void OpenTracksFile();
  //! saves the geometry voxel information
  void RecordTracks(GateSteppingAction *);
  void RecordRecStepData( const G4Event* evt );
  G4int CheckEOF();
  GateTrack* GetCurrentTracksData();
  void GetCurrentRecStepData( const G4Event* );
  G4int GetHeadNo(){ return m_currentTracksData; };
  void ReadForward() { m_currentTracksData++;};
  void ReadBackward(){
   if ( tracksTuple != 0 ) 
    {
     if ( m_currentTracksData > 0 ) {m_currentTracksData--;}
    }
   };
  void CloseTracksRootFile();
  void RecordPHData( ComptonRayleighData aCRData );
  void GetPHData ( ComptonRayleighData& aCRData );
  /* PY Descourt 08/09/2009 */
  
  //--------------------------------------------------------------------------
  class VOutputChannel
  {
  public:
    inline VOutputChannel(const G4String& aCollectionName, G4bool outputFlag)
      : nVerboseLevel(0),
	m_outputFlag(outputFlag),
	m_collectionName(aCollectionName),
	m_collectionID(-1)
    { }
    virtual inline ~VOutputChannel() {}
      
    virtual void Clear() = 0;
    virtual void RecordDigitizer() = 0 ;
    virtual void Book() = 0;

      
    inline void SetOutputFlag(G4bool flag) { m_outputFlag = flag; };
    inline void SetVerboseLevel(G4int val) { nVerboseLevel = val; };

      
    G4int             nVerboseLevel;
    G4bool            m_outputFlag;
    G4String          m_collectionName;
    G4int		m_collectionID;
  };
  

  //--------------------------------------------------------------------------
  class SingleOutputChannel : public VOutputChannel
  {
  public:
    inline SingleOutputChannel(const G4String& aCollectionName,G4bool outputFlag)
      : VOutputChannel( aCollectionName,outputFlag ),
	m_tree(0)
    {  m_buffer.Clear(); }
    virtual inline ~SingleOutputChannel() {}

    inline void Clear()
    {  m_buffer.Clear();}
    inline void Book() 
    {  
      m_collectionID=-1;
      if (m_outputFlag) {
	m_tree  = new GateSingleTree(m_collectionName);
	m_tree->Init(m_buffer);
      }
    }

    void RecordDigitizer();

    GateRootSingleBuffer         m_buffer; 
    GateSingleTree*              m_tree; 
  };



  //--------------------------------------------------------------------------
  class CoincidenceOutputChannel : public VOutputChannel
  {
  public:
    inline CoincidenceOutputChannel(const G4String& aCollectionName,G4bool outputFlag)
      : VOutputChannel( aCollectionName, outputFlag ),
	m_tree(0)
    {  m_buffer.Clear(); }
    virtual inline ~CoincidenceOutputChannel() {}

    inline void Clear()
    {  m_buffer.Clear();}
    inline void Book() 
    {
      m_collectionID=-1;
      if (m_outputFlag) {
	m_tree  = new GateCoincTree(m_collectionName);
	m_tree->Init(m_buffer);
      }
    }

    void RecordDigitizer();

    GateRootCoincBuffer         m_buffer; 
    GateCoincTree*              m_tree; 
  };



  //! flag to decide if it writes or not Hits, Singles and Digis to the ROOT file
  G4bool GetRootHitFlag()                   { return m_rootHitFlag; };
  void   SetRootHitFlag(G4bool flag)        { m_rootHitFlag = flag; };
  G4bool GetRootNtupleFlag()                { return m_rootNtupleFlag; };
  void   SetRootNtupleFlag(G4bool flag)     { m_rootNtupleFlag = flag; };
  G4bool GetSaveRndmFlag()                  { return m_saveRndmFlag; };
  void   SetSaveRndmFlag(G4bool flag)       { m_saveRndmFlag = flag; };

  //! Get the output file name
  const  G4String& GetFileName()             { return m_fileName; };
  //! Set the output file name
  virtual void   SetFileName(const G4String aName)   { m_fileName = aName + ".root"; };
  //! Get the output file path
  const char* GetFilePath()                  { return m_fileName.c_str(); };

  void  SetRndmFreq(G4int val) {saveRndm = val;}
  G4int GetRndmFreq()          {return saveRndm;}
  
  void   RegisterNewSingleDigiCollection(const G4String& aCollectionName,G4bool outputFlag);
  void   RegisterNewCoincidenceDigiCollection(const G4String& aCollectionName,G4bool outputFlag);
  
void  PrintRecStep();

  void SetVerboseLevel(G4int val) 
  { 
    GateVOutputModule::SetVerboseLevel(val);
    for (size_t i=0; i<m_outputChannelList.size(); ++i)
      m_outputChannelList[i]->SetVerboseLevel(val);
  };

private:

  G4ThreeVector  m_ionDecayPos;
  G4ThreeVector  m_positronGenerationPos;
  G4ThreeVector  m_positronAnnihilPos;
  
  G4double dxg1,dyg1,dzg1,dxg2,dyg2,dzg2;
  
  G4int saveRndm;    
  
  G4double latestEventID; // Used by the gjs an d gjm programs (cluster mode)
  G4double nbPrimaries;
  
  G4double m_positronKinEnergy;
  G4int    m_recordFlag;

  TFile*             m_hfile; // the file for histograms, tree ...
  GateHitTree*       m_treeHit; // the tree for hit quantities


  GateRootHitBuffer  m_hitBuffer; 
  
  G4int              m_updateROOTmodulo;

  G4bool   m_rootHitFlag;
  G4bool   m_rootSingleDigiFlag;
  G4bool   m_rootCoincDigiFlag;
  G4bool   m_rootNtupleFlag;
  G4bool   m_saveRndmFlag;
  
  G4String m_fileName;

  GateToRootMessenger* m_rootMessenger;

  std::vector<VOutputChannel*>  m_outputChannelList;
  
    /* PY Descourt 08/09/2009 */
    G4double m_positronKinEnergy_copy;
  G4ThreeVector  m_ionDecayPos_copy;
  G4ThreeVector  m_positronGenerationPos_copy;
  G4ThreeVector  m_positronAnnihilPos_copy;
  G4double dxg1_copy,dyg1_copy,dzg1_copy,dxg2_copy,dyg2_copy,dzg2_copy;
  G4int m_RSEventID,m_RSRunID; 
  /// ROOT FILE & DATAS FOR TRACKS INFOS
 G4int  m_currentTracksData; // stores the current index of the Tracks Data when reading Root file in detector mode 
 G4int  m_currentRSData; // stores the current index of the Record Stepping Data when reading Root file in detector mode
 GateTrack* m_currentGTrack;  // in detector mode contains all current Track Data
 G4String fTracksFN;
 TFile* m_TracksFile;
 TTree *tracksTuple;
 TTree *m_RecStepTree;
 G4int fSkipRecStepData;
 Long64_t last_RSEventID;

//
//// data from GateAnalysis::RecordEndOfEvent() stored in  the RecStepData Root File 
//
 ComptonRayleighData theCRData;
 ComptonRayleighData theCRData_copy;

 G4int m_EOF;  // 1 if tracks root file end of file is reached


 G4int TrackID,ParentID,RunID,EventID;
 G4int PDGCode,m_sourceID;
 G4int m_wasKilled;
 G4double posx,posy,posz,MDirectionx,MDirectiony,MDirectionz,Momentumx,Momentumy,Momentumz;
 G4double VertexPositionx,VertexPositiony,VertexPositionz,VtxMomDirx,VtxMomDiry,VtxMomDirz,Polarizationx,Polarizationy,Polarizationz;
 G4double LTime,GTime,PTime,Energy,KinEnergy,Velocity,VertexKineticEnergy,Weight;
 G4double m_EventTime;
 G4String m_particleName;
 Char_t   m_volumeName[40],m_processName[40],m_parentparticleName[40];
    /* PY Descourt 08/09/2009 */
 friend class GateToRootFactory;
};
//--------------------------------------------------------------------------

#endif
#endif
