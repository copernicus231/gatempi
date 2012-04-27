/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#ifdef GATE_USE_LMF

//#include <iostream.h>
//#include <fstream.h>
#include "globals.hh"
#include <stdio.h>
#include <stdlib.h>
#include "G4ThreeVector.hh"
#include "G4UserEventAction.hh"
#include "G4DigiManager.hh"
#include "globals.hh"
#include "GateToLMF.hh"
#include "GateToLMFMessenger.hh"
#include "GateDigitizer.hh"
#include "GateSingleDigi.hh"
#include "GateCoincidenceDigi.hh"
#include "GateVSystem.hh"
#include "GateApplicationMgr.hh"
#include "G4UnitsTable.hh"
#include "lmf_gate.h"
#include "GateOutputMgr.hh"
#include "GateRotationMove.hh"
#include "GateOrbitingMove.hh"
#include "GateEccentRotMove.hh"
#include "GatePulse.hh"


//! Constructor
/*!
  The first part is the mandatory part, because this class derivates
  from GateVOutputModule. The second is the specific part of this constructor.
*/
GateToLMF::GateToLMF(const G4String& name,GateOutputMgr* outputMgr,GateVSystem *psystem,DigiMode digiMode)      
  : GateVOutputModule(name,outputMgr,digiMode), 
    m_azimuthalStep(0),m_axialStep(0),
    m_shiftX(0),m_shiftY(0),m_shiftZ(0),
    m_inputDataChannel("Singles"),
    m_inputDataChannelID(-1)
{
  for(int i = 0; i < 2; i++) {
    m_LMFEnergy[i] = 0;
    m_LMFLayerID[i] = 0;
    m_LMFCrystalID[i] = 0;
    m_LMFSubmoduleID[i] = 0;
    m_LMFModuleID[i] = 0;
    m_LMFRsectorID[i] = 0;

    for (int iTime = 0;iTime<8;iTime++)
      m_pLMFTime[iTime][i] = 0;     
  }

  m_isEnabled = false; // Keep this flag false: all output are disabled by default

  m_pSystem = psystem ;
  nVerboseLevel = 0;
  m_LMFMessenger = new GateToLMFMessenger(this);

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...
  m_pfile = NULL;
  m_pASCIIfile = NULL;
/*
  m_nameOfFile = name + ".ccs";      //LMF binary file is .ccs 
  m_nameOfASCIIfile = name + ".cch"; //LMF ascii file is .cch
*/
  m_nameOfFile = " ";      // All default output file from all output modules are set to " ".
  m_nameOfASCIIfile = " "; // They are then checked in GateApplicationMgr::StartDAQ, using
                           // the VOutputModule pure virtual method GiveNameOfFile()

  m_name = name;                     // name without extension
 


     
  //              dynamic allocations for LMF Record carriers structures                  //|
  //------------------------------------------------------------------------------------------
  // pEncoH                                                                                //|
  // tested MALLOC for Encoding Header structure                                           //|
  pEncoH = (ENCODING_HEADER *) malloc(sizeof(ENCODING_HEADER)); //|
  if(pEncoH == NULL)                                                                       //|
    G4cout <<  "\n*** ERROR : in GateToLMF.cc : impossible to do : malloc()" << "\n" ;     //|
  
  // pcC                                                                                   //|
  // tested MALLOC for Current Content structure                                           //|
  pcC=(CURRENT_CONTENT *) malloc(sizeof(CURRENT_CONTENT));     //|
  if(pcC == NULL)                                                                          //|
    G4cout <<  "\n*** ERROR : in GateToLMF.cc : impossible to do : malloc()" << "\n";      //|
  // pEH                                                                                   //|      
  // tested MALLOC for Event Header structure                                              //|
  pEH=(EVENT_HEADER *)malloc(sizeof(EVENT_HEADER));            //|
  if(pEH == NULL)                                                                          //|
    G4cout <<  "\n***ERROR : in GateToLMF.cc : imposible to do : malloc()" << "\n";        //|
  
  pEH = fillEHforGATE(pEH); // standard filling of event header structure                  //|
  
  // pGDH                                                                                  //|      
  // tested MALLOC for GateDigi Header structure                                           //|
  pGDH=(GATE_DIGI_HEADER *)malloc(sizeof(GATE_DIGI_HEADER));                                 //|
  if(pGDH == NULL)                                                                         //|
    G4cout <<  "\n***ERROR : in GateToLMF.cc : imposible to do : malloc()" << "\n";        //|
  
  pGDH = fillGDHforGATE(pGDH); // standard filling of gateDigi header structure (evry bool to 0)  //|

  pcC->typeOfCarrier = pEncoH->scanContent.eventRecordTag;                                 //|
    
  // pER                                                                                   //|
  // tested MALLOCs for Event Record structure                                             //|
  for(int i = 0; i < 2; i++) {
    pER[i] = (EVENT_RECORD*) malloc(sizeof(EVENT_RECORD));          //|
    if(pER[i]==NULL)                                                                            //|
      G4cout <<  "\n***ERROR : in GateToLMF.cc : impossible to do : malloc()" << "\n";       //|
    pER[i]->crystalIDs = (u64 *) malloc(sizeof(u64));
    pER[i]->energy = (u8 *) malloc(sizeof(u8));
    pER[i]->pGDR = (GATE_DIGI_RECORD*)malloc(sizeof(GATE_DIGI_RECORD));
    if((pER[i]->crystalIDs == NULL)||(pER[i]->energy == NULL)||(pER[i]->pGDR == NULL))                //|
      G4cout <<  "*** ERROR : in GATEToLMF.cc : impossible to do : malloc" << "\n ";         //|
  }
  //------------------------------------------------------------------------------------------ 
  
} 


//! Destructor
/*!
  The first part is the mandatory part, because this class derivates
  from GateVOutputModule. The second is the specific part of this destructor.
*/
GateToLMF::~GateToLMF()          
{
  delete m_LMFMessenger;
  
  if (nVerboseLevel > 0) G4cout << "GateToLMF deleting..." << G4endl;
 
  if(pEncoH->scanContent.eventRecordBool == 1)
    {
      if(pGDH)
	free(pGDH);
      for(int i = 0; i < 2; i++) {
	free(pER[i]->crystalIDs);
	free(pER[i]->energy);
	free(pER[i]);
      }
      if(pEH->coincidenceBool)
	freeER(pERC);

      free(pEH);
    }
  if(pEncoH->scanContent.countRateRecordBool == 1)
    {
      free(pCRR->pRsectorRate);
      free(pCRR->pModuleRate);
      free(pCRR->pSubmoduleRate);
      free(pCRH);
      free(pCRR);
    }
  free(pcC);
  free(pEncoH);
}

// .....ooooooOOOOOOoooooo...........ooooooOOOOOOoooooo......

const G4String& GateToLMF::GiveNameOfFile()
{
  return m_nameOfFile;
}

// .....ooooooOOOOOOoooooo...........ooooooOOOOOOoooooo......

void GateToLMF::buildLMFEventRecord()
{
  static G4int nSingles = 0;
  int k;
  u16 flag;

  // static FILE *m_pfile=NULL;
  for(k=0;k<8;k++)                //  TIME
      pER[0]->timeStamp[k] = m_pLMFTime[k][0]; //     

  
  if(pEH->energyBool)
    pER[0]->energy[0]= m_LMFEnergy[0];  // ENERGY
  
  if(pEH->gantryAngularPosBool)
    pER[0]->gantryAngularPos = m_LMFgantryAngularPos;    // gantry's  angular position
  
  if(pEH->gantryAxialPosBool)
    pER[0]->gantryAxialPos = m_LMFgantryAxialPos;      // gantry's  axial position
  
  if(pEH->detectorIDBool) {
    pER[0]->crystalIDs[0] = makeid(m_LMFRsectorID[0],
				   m_LMFModuleID[0],
				   m_LMFSubmoduleID[0],
				   m_LMFCrystalID[0],
				   m_LMFLayerID[0],
				   pEncoH,
				   &flag);//Crystal ID
    if(nVerboseLevel > 7) {
      printf("builded detector ID = %llu\n", pER[0]->crystalIDs[0]);
      printf("%d %d %d %d %d \n\n\n\n",m_LMFRsectorID[0],
	     m_LMFModuleID[0],
	     m_LMFSubmoduleID[0],
	     m_LMFCrystalID[0],
	     m_LMFLayerID[0]);
    }
  }
  
  LMFbuilder(pEncoH,pEH,pCRH,pGDH,pcC,pER[0],pCRR,&m_pfile,m_nameOfFile.c_str());   /* ...write it */

  nSingles++;
  if((nSingles%10000)==0)
    printf("\r%d singles stored in LMF file",nSingles);

}

void GateToLMF::createLMF_ASCIIfile(void)
{ 
  std::ofstream asciiFile(m_nameOfASCIIfile,std::ios::out);  // open a ASCII file in writting mode
 
  // Ask the system to print out its parameters into the stream
  if(!asciiFile)
    G4cerr << "GateToLMF::createLMF_ASCIIfile() : Impossible to open a new file" << G4endl;
  m_pSystem->PrintToStream(asciiFile,false); // cf. GateCylindricalPETSystem class.
 
  G4double m_timeSlice = GateApplicationMgr::GetInstance()->GetTimeSlice();

  // OK, now we have all the system-dependant parameters
  // We now need to write the paremters that are related to the simulation itself

  // Get and print out the time slice ?
  // asciiFile << "time slice: " << G4BestUnit( m_timeSlice, "Time" ) << G4endl;

  
  // Get and print out the time and energy steps
  if(pEH->coincidenceBool)
    asciiFile << "clock time step: " << GATE_LMF_TIME_STEP_COINCI << " ps" << G4endl;
  else
    asciiFile << "clock time step: " << GATE_LMF_TIME_STEP_PICOS << " ps" << G4endl;
  asciiFile << "energy step: " << GATE_LMF_ENERGY_STEP_KEV << " keV" << G4endl;
  // GATE_LMF_TIME_STEP_PICOS and GATE_LMF_ENERGY_STEP_KEV are defined 
  // in LMF library : lmf/includes/constantsLMF_ccs.h


  // Compute, store and print out the azimuthal step
  GateSystemComponent* baseComponent = m_pSystem->GetBaseComponent();
  G4double rotationSpeed = 0;
  if ( baseComponent->FindRotationMove() )
    rotationSpeed = baseComponent->GetRotationVelocity();
  else if ( baseComponent->FindOrbitingMove() )
    rotationSpeed = baseComponent->GetOrbitingVelocity();
  else if ( baseComponent->FindEccentRotMove() )
    {rotationSpeed = baseComponent->GetEccentRotVelocity(); // Store the eccentric shift for EccenRotMove
    m_shiftX= baseComponent->GetEccentRotShift().x();
    m_shiftY= baseComponent->GetEccentRotShift().y();
    asciiFile << "x shift: " << m_shiftX / cm  << " cm" << G4endl;
    asciiFile << "y shift: " << m_shiftY / cm  << " cm" << G4endl;
    }
  m_azimuthalStep = rotationSpeed * m_timeSlice;
  asciiFile << "azimuthal step: " << m_azimuthalStep / degree << " degree" << G4endl;

  // Print out the axial Z shift for modulo M ring repeater

  GateSystemComponent*     rsectorComponent   = m_pSystem->GetMainComponent();
  if ( rsectorComponent->FindAngularRepeater() )
    {
      m_pZshift_vector[0] = rsectorComponent->GetAngularRepeatZShift1();
      m_pZshift_vector[1] = rsectorComponent->GetAngularRepeatZShift2();
      m_pZshift_vector[2] = rsectorComponent->GetAngularRepeatZShift3();
      m_pZshift_vector[3] = rsectorComponent->GetAngularRepeatZShift4();
      m_pZshift_vector[4] = rsectorComponent->GetAngularRepeatZShift5();
      m_pZshift_vector[5] = rsectorComponent->GetAngularRepeatZShift6();
      m_pZshift_vector[6] = rsectorComponent->GetAngularRepeatZShift7();
      m_pZshift_vector[7] = rsectorComponent->GetAngularRepeatZShift8();

      
      G4int m_RingModuloNumber = rsectorComponent->GetAngularModuloNumber();

      // G4cout << "2: "<< m_RingModuloNumber<<m_pZshift_vector << G4endl; 
      for ( int i=0 ; i < m_RingModuloNumber ; i++)
	{
	  if(m_pZshift_vector[i] != 0.) 
	    asciiFile << "z shift sector "<<i<<" mod "<<m_RingModuloNumber<<" : "<<m_pZshift_vector[i] / cm<<" cm"<< G4endl;
	}
    }
  else
    G4cout << "GAteToLMF WARNING : could not find AngularRepeater "<< rsectorComponent << rsectorComponent->FindAngularRepeater() << G4endl;

  // Compute, store and print out the axial step
  G4double axialTranslationSpeed = baseComponent->GetTranslationVelocity().z();
  m_axialStep = axialTranslationSpeed * m_timeSlice;
  asciiFile << "axial step: " << G4BestUnit( m_axialStep, "Length" ) << G4endl;

  // Add the scan file name
  asciiFile << "scan file name: "<< m_name << G4endl;
  asciiFile.close();  // close ASCII file, we just have now to write the last line

}



void GateToLMF::StoreTheDigiInLMF(GateSingleDigi *digi)
{  
  if(pEH->energyBool)
    {
      //    G4cout << "Energy stored into digi=" << digi->GetEnergy() << G4endl;
      SetEnergy(0,digi->GetEnergy());  // energy
    }

  if(pEH->detectorIDBool)
    {
      SetLayerID(0, digi->GetComponentID(LAYER_DEPTH)); // ids
      SetCrystalID(0, digi->GetComponentID(CRYSTAL_DEPTH));
      SetSubmoduleID(0, digi->GetComponentID(SUBMODULE_DEPTH)); 
      SetModuleID(0, digi->GetComponentID(MODULE_DEPTH)); 
      SetRsectorID(0, digi->GetComponentID(RSECTOR_DEPTH));
    }



  /*      positions         */
  if(pEH->gantryAxialPosBool)
    SetGantryAxialPos((G4int)( digi->GetScannerPos().z() / m_axialStep));
  if(pEH->gantryAngularPosBool){
    G4int Nloop = int ((digi->GetScannerRotAngle()/rad)/twopi); // Nloop >0. or <0.
    G4double ModAngle = (digi->GetScannerRotAngle()  - Nloop * twopi);
    SetGantryAngularPos((G4int)(ModAngle / m_azimuthalStep)) ; 
  }
 

  if(pEH->sourcePosBool)
    {
      SetSourceAxialPos(0);
      SetSourceAngularPos(0);
    }


  /*         Time              */
  if(nVerboseLevel > 2)
    printf("\n************* EVENT TIME IS = %f\n",digi->GetTime());
  SetTime(0,digi->GetTime());  //!< time
  
  if(pEH->gateDigiBool)   // advanced storage of digi in LMF
    StoreMoreDigiInLMF_GDR(digi);

  if (nVerboseLevel > 0)
    {
      showOneLMFDigi(); 
      if (nVerboseLevel > 2)
	{
	  G4cout << "One digi store in " << m_nameOfFile <<G4endl;
	  if (nVerboseLevel > 7)
	    {
	      G4cout << "type enter to continue" << G4endl;
	      getchar(); // let's see this beautiful event record
	    }
	} 
    }
    
  buildLMFEventRecord();
  
}

void GateToLMF::StoreMoreDigiInLMF_GDR(GateSingleDigi *digi)
{ 
//   if(digi->GetRunID()) {
//     printf("digi->GetRunID = %d\n", digi->GetRunID());
//     getchar();
//   }
  if(pGDH->runIDBool)
    pER[0]->pGDR->runID = (unsigned long) digi->GetRunID();
  if(pGDH->eventIDBool)
    pER[0]->pGDR->eventID[0] = (unsigned long) digi->GetEventID();
  if(pGDH->sourceIDBool)
    pER[0]->pGDR->sourceID[0] = (unsigned short) digi->GetSourceID();
  if(pGDH->sourceXYZPosBool)
    {
      pER[0]->pGDR->sourcePos[0].X = (short) ((G4int)digi->GetSourcePosition().x()/mm);
      pER[0]->pGDR->sourcePos[0].Y = (short) ((G4int)digi->GetSourcePosition().y()/mm);
      pER[0]->pGDR->sourcePos[0].Z = (short) ((G4int)digi->GetSourcePosition().z()/mm);
    }
  if(pGDH->globalXYZPosBool)
    {
      
      pER[0]->pGDR->globalPos[0].X = (short)((G4int)digi->GetGlobalPos().x()/mm);
      pER[0]->pGDR->globalPos[0].Y = (short)((G4int)digi->GetGlobalPos().y()/mm);
      pER[0]->pGDR->globalPos[0].Z = (short)((G4int)digi->GetGlobalPos().z()/mm);
    }
  if(pGDH->comptonBool)
    pER[0]->pGDR->numberCompton[0] = (unsigned char)digi->GetNPhantomCompton();


  if(pGDH->comptonDetectorBool)
    pER[0]->pGDR->numberDetectorCompton[0] = (unsigned char)digi->GetNCrystalCompton();

  return;
}


//    G4int block1ID = m_system->GetMainComponentID( (*CDC)[iDigi]->GetPulse(0) );

void GateToLMF::StoreTheCoinciDigiInLMF(GateCoincidenceDigi *digi)
{  
  static G4int nCoinci = 0;
  u16 flag;

  GatePulse *aPulse[2];

  for(int i= 0; i<2; i++) {
    aPulse[i] = &(digi->GetPulse(i));

    /*         Time              */
    if(nVerboseLevel > 2)
      printf("\n************* EVENT TIME IS = %f\n",aPulse[i]->GetTime());
    SetTime(i,aPulse[i]->GetTime());
    
    for(int k = 0; k<8; k++)
      pER[i]->timeStamp[k] = m_pLMFTime[k][i];

    /*          Energy           */
    if(pEH->energyBool)
      pER[i]->energy[0] = (u8) ((G4int)(((aPulse[i]->GetEnergy()/GATE_LMF_ENERGY_STEP_KEV)/keV)+0.5));

    /*          DetectorID       */
    if(pEH->detectorIDBool) {
      SetLayerID(i, aPulse[i]->GetComponentID(LAYER_DEPTH));
      SetCrystalID(i, aPulse[i]->GetComponentID(CRYSTAL_DEPTH));
      SetSubmoduleID(i, aPulse[i]->GetComponentID(SUBMODULE_DEPTH)); 
      SetModuleID(i, aPulse[i]->GetComponentID(MODULE_DEPTH)); 
      SetRsectorID(i, aPulse[i]->GetComponentID(RSECTOR_DEPTH));
      
      pER[i]->crystalIDs[0] = makeid(m_LMFRsectorID[i],
				     m_LMFModuleID[i],
				     m_LMFSubmoduleID[i],
				     m_LMFCrystalID[i],
				     m_LMFLayerID[i],
				     pEncoH,
				     &flag);
    }

    /*          Gantry Position       */
    if(pEH->gantryAxialPosBool)
      pER[i]->gantryAxialPos = (u16)((G4int)( aPulse[i]->GetScannerPos().z() / m_axialStep));

    if(pEH->gantryAngularPosBool){
      G4int Nloop = int ((aPulse[i]->GetScannerRotAngle()/rad)/twopi); // Nloop >0. or <0.
      G4double ModAngle = (aPulse[i]->GetScannerRotAngle()  - Nloop * twopi);
      pER[i]->gantryAngularPos = (u16)((G4int)(ModAngle / m_azimuthalStep)) ; 
  }
 
//     /*          Source Position       */
//     if(pEH->sourcePosBool) {
//       pER[i]->sourceAngularPos = (u16) 0;	/* external source's angular position, 16 bits */
//       pER[i]->sourceAxialPos = (u16) 0;	
//     }

    /*     Gate specific information    */
    if(pEH->gateDigiBool) {
      if(pGDH->runIDBool)
	pER[i]->pGDR->runID = (u32) aPulse[i]->GetRunID();

      if(pGDH->eventIDBool)
	pER[i]->pGDR->eventID[0] = (u32) aPulse[i]->GetEventID();

      if(pGDH->sourceIDBool)
	pER[i]->pGDR->sourceID[0] = (u16) aPulse[i]->GetSourceID();

      if(pGDH->sourceXYZPosBool) {
	pER[i]->pGDR->sourcePos[0].X = (i16) ((G4int)aPulse[i]->GetSourcePosition().x()/mm);
	pER[i]->pGDR->sourcePos[0].Y = (i16) ((G4int)aPulse[i]->GetSourcePosition().y()/mm);
	pER[i]->pGDR->sourcePos[0].Z = (i16) ((G4int)aPulse[i]->GetSourcePosition().z()/mm);
      }
      if(pGDH->globalXYZPosBool) {
	pER[i]->pGDR->globalPos[0].X = (i16)((G4int)aPulse[i]->GetGlobalPos().x()/mm);
	pER[i]->pGDR->globalPos[0].Y = (i16)((G4int)aPulse[i]->GetGlobalPos().y()/mm);
	pER[i]->pGDR->globalPos[0].Z = (i16)((G4int)aPulse[i]->GetGlobalPos().z()/mm);
      }
      if(pGDH->comptonBool)
	pER[i]->pGDR->numberCompton[0] = (unsigned char)aPulse[i]->GetNPhantomCompton();

      if(pGDH->comptonDetectorBool)
	pER[i]->pGDR->numberDetectorCompton[0] = (unsigned char)aPulse[i]->GetNCrystalCompton();
    }
  }

  
  fillCoinciRecordForGate(pEncoH, pEH, pGDH, pER[0], pER[1], nVerboseLevel, pERC);

  LMFCbuilder(pEncoH, pEH, pGDH, pcC, pERC, &m_pfile, m_nameOfFile.c_str());

//   LMFbuilder(pEncoH,pEH,pCRH,pGDH,pcC,pER[0],pCRR,&m_pfile,m_nameOfFile.c_str());   /* ...write it */

  nCoinci++;
  if((nCoinci%10000)==0)
    printf("\r%d coinci stored in LMF file",nCoinci);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
void GateToLMF::RecordBeginOfAcquisition()
{
  if (nVerboseLevel > 2)
    G4cout << "GateToLMF::RecordBeginOfAcquisition" << G4endl;

  if(pEH->coincidenceBool) {
    pERC = newER(pEH);
    m_inputDataChannel = "Coincidences";
  }

  m_inputDataChannelID = G4DigiManager::GetDMpointer()->GetDigiCollectionID(m_inputDataChannel);

  static G4int codeForRecords = 1;//5; 
  // 0 no records
  // 1 event records
  // 2 count rate records
  // 4 gate digi records
  // and 5 gate digi + event ...
  if (nVerboseLevel > 5)
    G4cout << "GateToLMF::RecordBeginOfAcquisition" << G4endl;

  if(pEH->gateDigiBool)
    codeForRecords = 5;


  //const GateCoincidenceDigiCollection * CDC = GetOutputMgr()->GetCoincidenceDigiCollection(m_inputDataChannel);

  createLMF_ASCIIfile();      

  G4cout << "\n\n**************" << G4endl;
  G4cout << " LMF INIT" << G4endl;
  G4cout << "**************" << G4endl;
      
  GateSystemComponent*     rsectorComponent   = m_pSystem->GetMainComponent();
  GateArrayComponent*      moduleComponent    = m_pSystem->FindArrayComponent("module");
  GateArrayComponent*      submoduleComponent = m_pSystem->FindArrayComponent("submodule");
  GateArrayComponent*      crystalComponent   = m_pSystem->FindArrayComponent("crystal");


  fillEncoHforGate(rsectorComponent->GetLinearRepeatNumber(),
		   rsectorComponent->GetAngularRepeatNumber(),
		   moduleComponent->GetRepeatNumber(2),
		   moduleComponent->GetRepeatNumber(1),
		   submoduleComponent->GetRepeatNumber(2),
		   submoduleComponent->GetRepeatNumber(1),
		   crystalComponent->GetRepeatNumber(2),
		   crystalComponent->GetRepeatNumber(1),
		   1, // tangential number of layer always 1
		   crystalComponent->GetActiveChildNumber(),
		   pEncoH,codeForRecords);
  

  G4cout << "\n\n**************\n\n\n" << G4endl;
}




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


void GateToLMF::RecordEndOfAcquisition()
{
  if (nVerboseLevel > 2)
    G4cout << "GateToLMF::RecordEndOfAcquisition" << G4endl;
  if((pEncoH->scanContent.nRecord != 0)&&(m_pfile != NULL))
    {
      CloseLMFfile(m_pfile);
    } // these lines works but just for 1 file...
      
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


void GateToLMF::RecordBeginOfRun(const G4Run * r)
{
  if (nVerboseLevel > 5)
    G4cout << "GateToLMF::RecordBeginOfRun" << G4endl;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void GateToLMF::RecordEndOfEvent(const G4Event* event)
{
  if (nVerboseLevel > 5)
    G4cout << "GateToLMF::RecordEndOfEvent" << G4endl;

  size_t iDigi, n_digi;
  if(!pEH->coincidenceBool) {
    const GateSingleDigiCollection * SDC = (GateSingleDigiCollection*) (G4DigiManager::GetDMpointer()->GetDigiCollection( m_inputDataChannelID ));

    if (!SDC) 
      {
	if (nVerboseLevel>0) 
	  G4cout << "GateToLMF::RecordEndOfEvent::GateSingleDigiCollection not found" << G4endl;
	return;
      }
    n_digi =  SDC->entries();
    for (iDigi=0 ; iDigi < n_digi ; iDigi++) 
      StoreTheDigiInLMF((*SDC)[iDigi]);
  }
  else {
    const GateCoincidenceDigiCollection * CDC = GetOutputMgr()->GetCoincidenceDigiCollection(m_inputDataChannel);

    if (!CDC) {
      if (nVerboseLevel>0) 
	G4cout << "GateToLMF::RecordEndOfEvent::GateCoincidenceDigiCollection not found" << G4endl;
      return;
    }
  

    n_digi  =  CDC->entries();
    for (size_t iDigi=0 ; iDigi < n_digi ; iDigi++) 
      StoreTheCoinciDigiInLMF((*CDC)[iDigi]);
  }
}


/*------------- MESSENGER FUNCTIONS -------------------------------*/

//! Set the ouput LMF file name.
void GateToLMF::SetOutputFileName(G4String ofname)
{ 
  m_nameOfFile = ofname + ".ccs";
  m_nameOfASCIIfile =  ofname + ".cch";
  m_name = ofname;
  if (nVerboseLevel > 0)
    G4cout << "GateToLMF::SetOutputFileName: file name set to " << m_nameOfFile << G4endl;
}

/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetCoincidenceBool(G4bool value)
{
  pEH->coincidenceBool = value;
}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetDetectorIDBool(G4bool value)
{ 
  pEH->detectorIDBool = value;

}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetEnergyBool(G4bool value)
{ 
  pEH->energyBool = value;
}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetNeighbourBool(G4bool value)
{ 
  pEH->neighbourBool = value;
}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetNeighbourhoodOrder(G4int value)
{ 
  pEH->neighbourhoodOrder = value;
  pEH->numberOfNeighbours = findvnn((unsigned short)value);
}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetGantryAxialPosBool(G4bool value)
{ 
  pEH->gantryAxialPosBool = value;
}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetGantryAngularPosBool(G4bool value)
{ 
  pEH->gantryAngularPosBool = value;
}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetSourcePosBool(G4bool value)
{ 
  pEH->sourcePosBool = value;
}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetGateDigiBool(G4bool value)
{ 
  pEH->gateDigiBool = value;
}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetComptonBool(G4bool value)
{ 
  if(pEH->gateDigiBool)
    pGDH->comptonBool= value;
}


/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetComptonDetectorBool(G4bool value)
{ 
  if(pEH->gateDigiBool)
    pGDH->comptonDetectorBool= value;
}

/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetSourceIDBool(G4bool value)
{ 
  if(pEH->gateDigiBool)
    pGDH->sourceIDBool= value;

}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetSourceXYZPosBool(G4bool value)
{ 
  if(pEH->gateDigiBool)
    pGDH->sourceXYZPosBool= value;

}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetGlobalXYZPosBool(G4bool value)
{ 
  if(pEH->gateDigiBool)
    pGDH->globalXYZPosBool= value;

}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetEventIDBool(G4bool value)
{ 
  if(pEH->gateDigiBool)
    pGDH->eventIDBool= value;

}
/*------------- MESSENGER FUNCTIONS -------------------------------*/
void GateToLMF::SetRunIDBool(G4bool value)
{ 
  if(pEH->gateDigiBool)
    pGDH->runIDBool= value;

}
  
// ......................SET TIME
void GateToLMF::SetTime(u8 id, G4double value) 
{
  static u8 *bufCharTime; 
  static G4int allocDone = 0;

  static u64 timeulli;
  
  //  printf("\n\n\nEVENT \n\n\n");
  //  printf("* Time Value = %f picosecond",value/picosecond);

  if(!allocDone) {
    bufCharTime = (u8 *)malloc (8*sizeof(u8));
    allocDone = 1;
  }
   
  G4double timeForLMF=(((value/GATE_LMF_TIME_STEP_PICOS)/picosecond)+0.5);

  timeulli = (u64) timeForLMF;
 
  bufCharTime = u64ToU8(timeulli);
    
  //  memcpy(m_pLMFTime, doubleToChar(timeForLMF) , 8 );
  for(int i =0 ; i<8 ; i++)
    m_pLMFTime[i][id] = bufCharTime[i];
}

// .....ooooooOOOOOOoooooo...........ooooooOOOOOOoooooo......



//.........................GET / SET IDs
void GateToLMF::SetGantryAxialPos(G4int value){  m_LMFgantryAxialPos = (unsigned short)value;}
void GateToLMF::SetGantryAngularPos(G4int value){  m_LMFgantryAngularPos = (unsigned short)value;}
void GateToLMF::SetSourceAxialPos(G4int value){  m_LMFsourceAxialPos = (unsigned short)value;}
void GateToLMF::SetSourceAngularPos(G4int value){  m_LMFsourceAngularPos = (unsigned short)value;}
unsigned short GateToLMF::GetGantryAngularPos(){return(m_LMFgantryAngularPos);}
unsigned short GateToLMF::GetGantryAxialPos(){return(m_LMFgantryAxialPos);}
unsigned short GateToLMF::GetSourceAngularPos(){return(m_LMFsourceAngularPos);}
unsigned short GateToLMF::GetSourceAxialPos(){return(m_LMFsourceAxialPos);}







void GateToLMF::showOneLMFDigi()   // Display a digi in his LMF standard
{
  // ...................SHOW time
  G4cout <<  "\ntime (8 unsigned char)=" << "\n";
  int i=0;
  for(i=0;i<8;i++)
    G4cout <<  "\t" << (G4int) m_pLMFTime[7-i][0]; 
  G4cout <<  " =  in picosec. : " << (G4double) u8ToDouble(m_pLMFTime[0]) << "\n";
  // ....................SHOW ID
  G4cout << "\nCrystal ID : "
	 << "\tRsector = " << m_LMFRsectorID[0]
	 << "\tmodule = " <<  m_LMFModuleID[0] 
	 << "\tsubmodule = " << m_LMFSubmoduleID[0]
	 << "\tcrystal = " << m_LMFCrystalID[0]
	 << "\tlayer = " << m_LMFLayerID[0] << "\n";
  // .................SHOW Energy
  G4cout <<  "\nEnergy in LMFstep = " << (G4int) m_LMFEnergy[0] << "\n";
  
}






#endif