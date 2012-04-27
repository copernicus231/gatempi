/*----------------------
  GATE version name: gate_v6

  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/

#include "GateToInterfile.hh"
#include "GateToInterfileMessenger.hh"

#include "globals.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

#include "GateOutputMgr.hh"
#include "GateTools.hh"
#include "GateSPECTHeadSystem.hh"
#include "GateToProjectionSet.hh"
#include "GateProjectionSet.hh"

//#include "GateVObjectCreator.hh"

//#include "GatePlacementMove.hh"

/*
 *  GateToInterfile is used to write as an output file the result of the GateToProjectionSet module.
 *  This 2 classes are strickly working together.
 *  All macro commands (inherited from GateVOutputModule) of the GateToInterfileMessenger are overloaded to
 *  have no action at all. The enable and disable command, verbose and setFileName are now managed by
 *  GateToProjectionSet. The describe command of GateToInterfile will do nothing. The enable and disable
 *  commands of the GateToProjectionSet class will also enable or disable the GateToProjectionSet module
 *  (the same for the verbose level).
 */


GateToInterfile::GateToInterfile(const G4String& name, GateOutputMgr* outputMgr,GateSPECTHeadSystem* itsSystem,DigiMode digiMode) 
  : GateVOutputModule(name,outputMgr,digiMode)
  , m_system(itsSystem)
  , m_fileName(" ") // All default output file from all output modules are set to " ".
                    // They are then checked in GateApplicationMgr::StartDAQ, using
                    // the VOutputModule pure virtual method GiveNameOfFile()
{
/*
  if (digiMode==kofflineMode)
    m_fileName="digigate";
*/

  m_isEnabled = false; // Keep this flag false: all output are disabled by default
  m_asciiMessenger = new GateToInterfileMessenger(this);

  nVerboseLevel =0;
}




GateToInterfile::~GateToInterfile() 
{
  delete m_asciiMessenger;

  if (nVerboseLevel > 0) G4cout << "GateToInterfile deleting..." << G4endl;

  if (m_headerFile.is_open())
    m_headerFile.close();
  if (m_dataFile.is_open())
    m_dataFile.close();
}


const G4String& GateToInterfile::GiveNameOfFile()
{
  return m_fileName;
}

void GateToInterfile::RecordBeginOfAcquisition()
{
  if (!(m_system->GetProjectionSetMaker()->IsEnabled())) return;

  // Open the header file
  m_headerFile.open((m_fileName+".hdr").c_str(),std::ios::out | std::ios::trunc);
  if (!(m_headerFile.is_open()))
      	G4Exception("[GateToInterfile::RecordBeginOfAcquisition]:\n"
      	      	    "Could not open the header file '" + m_fileName + ".hdr'!");

  // Pre-write the header file
  WriteGeneralInfo();
  WriteGateScannerInfo();
  m_headerFile  << "!END OF INTERFILE :="    	      	    << G4endl;

  // Open the data file
  m_dataFile.open((m_fileName+".sin").c_str(),std::ios::out | std::ios::trunc | std::ios::binary);
  if (!(m_dataFile.is_open()))
      	G4Exception("[GateToInterfile::RecordBeginOfAcquisition]:\n"
      	      	    "Could not open the data file '" + m_fileName + ".sin'!");

}


void GateToInterfile::RecordEndOfAcquisition()
{
  if (!(m_system->GetProjectionSetMaker()->IsEnabled())) return;

  // Close the data file
  m_dataFile.close();

  // Fully rewrite the header, so as to store the maximum counts
  m_headerFile.seekp(0,std::ios::beg);
  if ( m_headerFile.bad() ) G4Exception( "\n[GateToInterfile]:\n"
      	      	              	      	 "Could not go to back to the beginning of the header file (file missing?)!\n"); 
  WriteGeneralInfo();
  WriteGateScannerInfo();
  WriteGateRunInfo( m_system->GetProjectionSetMaker()->GetProjectionSet()->GetCurrentProjectionID()+1 );

  m_headerFile  << "!END OF INTERFILE :="    	      	    << G4endl;

  m_headerFile.close();

}






void GateToInterfile::RecordBeginOfRun(const G4Run* )
{
  if (!(m_system->GetProjectionSetMaker()->IsEnabled())) return;
}





void GateToInterfile::RecordEndOfRun(const G4Run* )
{
  if (!(m_system->GetProjectionSetMaker()->IsEnabled())) return;

  // Write the projection sets
  for (size_t headID=0 ; headID < m_system->GetProjectionSetMaker()->GetHeadNb(); headID++)
    if (m_system->GetProjectionSetMaker()->GetProjectionSet()->GetData() != 0) {
    		m_system->GetProjectionSetMaker()->GetProjectionSet()->StreamOut( m_dataFile , headID );
    } else if (m_system->GetProjectionSetMaker()->GetProjectionSet()->GetARFData() != 0) {
    		m_system->GetProjectionSetMaker()->GetProjectionSet()->StreamOutARFProjection( m_dataFile , headID );
    } else {
    		G4cerr << "[GateToInterfile::RecordEndOfRun]:" << G4endl
    		<< "No data available to write to projection set." << G4endl;
    }
}

/* Overload of the base-class' virtual method to print-out a description of the module

	indent: the print-out indentation (cosmetic parameter)
*/    
void GateToInterfile::Describe(size_t indent)
{
  GateVOutputModule::Describe(indent);
  G4cout << GateTools::Indent(indent) << "Job:                   write a set of SPECT projections into an Interfile output file" << G4endl;
  G4cout << GateTools::Indent(indent) << "Is enabled?            " << ( IsEnabled() ? "Yes" : "No") << G4endl;
  G4cout << GateTools::Indent(indent) << "File name:             '" << m_fileName << "'" << G4endl;
  G4cout << GateTools::Indent(indent) << "Attached to system:    '" << m_system->GetObjectName() << "'" << G4endl;
}








// Write the general INTERFILE information into the header
void GateToInterfile::WriteGeneralInfo()
{
  m_headerFile  << "!INTERFILE :="    	      	      	    << G4endl
		<< "!imaging modality := "       	    << "nucmed" << G4endl
		<< "!version of keys := "       	    << "3.3" << G4endl
		<< "date of keys := " 	         	    << "1992:01:01" << G4endl
      	      	<< ";" << G4endl;
		
  m_headerFile  << "!GENERAL DATA :=" 	      	      	    << G4endl
		<< "data description := "     	      	    << "GATE simulation" << G4endl
		<< "!data starting block := "         	    << 0 << G4endl
 		<< "!name of data file := "    	      	    << m_fileName+".sin" << G4endl
    	      	<< ";" << G4endl;
		
  time_t aTimer;
  time(&aTimer);
  struct tm * currentTime = localtime(&aTimer);
  GateToProjectionSet* setMaker = m_system->GetProjectionSetMaker();

  m_headerFile  << "!GENERAL IMAGE DATA :="   	      	    << G4endl
		<< "!type of data := "     	      	    << "TOMOGRAPHIC" << G4endl
 		<< "!total number of images := "      	    << setMaker->GetTotalImageNb() << G4endl
 		<< "study date := "   	      	      	    << std::setfill('0')
		      	      	      	      	      	    << std::setw(4) << 1900+currentTime->tm_year << ":" 
		      	      	      	      	      	    << std::setw(2) << currentTime->tm_mon << ":" 
				      	      	      	    << std::setw(2) << currentTime->tm_mday << G4endl
 		<< "study time := "   	      	      	    << std::setw(2) << currentTime->tm_hour << ":" 
		      	      	      	      	      	    << std::setw(2) << currentTime->tm_min << ":" 
				      	      	      	    << std::setw(2) << currentTime->tm_sec << G4endl 
							    << std::setfill(' ')
  		<< "imagedata byte order := " 	      	    << ( (BYTE_ORDER == LITTLE_ENDIAN) ? "LITTLEENDIAN" : "BIGENDIAN" )  << G4endl
    		<< "number of energy windows := "    	    << 1 << G4endl
 	      	<< ";" << G4endl;

  m_headerFile  << "!SPECT STUDY (general) :="        	    << G4endl
      	      	<< "number of detector heads := "     	    << setMaker->GetHeadNb() << G4endl
 	      	<< ";" << G4endl;
  for (size_t headID=0; headID<setMaker->GetHeadNb() ; headID++) {

    m_headerFile  << "!number of images/energy window := "    << setMaker->GetTotalImageNb() << G4endl
		  << "!process status := "     	      	      << "ACQUIRED" << G4endl
		  << "!number of projections := "             << setMaker->GetProjectionNb() << G4endl
		  << "!matrix size [1] := "     	      << setMaker->GetPixelNbX() << G4endl
		  << "!matrix size [2] := "     	      << setMaker->GetPixelNbY() << G4endl
		  << "!number format := "       	      << "UNSIGNED INTEGER" << G4endl
		  << "!number of bytes per pixel := "  	      << setMaker->BytesPerPixel() << G4endl
		  << "!scaling factor (mm/pixel) [1] := "     << setMaker->GetPixelSizeX()/mm << G4endl
		  << "!scaling factor (mm/pixel) [2] := "     << setMaker->GetPixelSizeY()/mm << G4endl
		  << "!extent of rotation := "       	      << setMaker->GetAngularSpan()/deg << G4endl
		  << "!time per projection (sec) := "         << setMaker->GetTimePerProjection() / second << G4endl
		  << "study duration (elapsed) sec : = "      << setMaker->GetStudyDuration() / second << G4endl
		  << "!maximum pixel count : = " 	      << setMaker->GetProjectionSet()->GetMaxCounts(headID) << G4endl
		  << ";" << G4endl;

    G4double rotationDirection = ( ( m_system->GetBaseComponent()->GetOrbitingVelocity()>=0) ? +1. : -1 );

    m_headerFile  << "!SPECT STUDY (acquired data) :="        << G4endl
      	      	  << "!direction of rotation := "       	    << ( ( m_system->GetBaseComponent()->GetOrbitingVelocity()>=0) ? "CW" : "CCW" )  << G4endl
		  << "start angle := "	      	      	      	    << (headID * setMaker->GetHeadAngularPitch() / rotationDirection) / degree << G4endl
		  << "first projection angle in data set := "	    << (headID * setMaker->GetHeadAngularPitch() / rotationDirection) / degree << G4endl
		  << "acquisition mode := "      	      	    << "stepped" << G4endl
		  << "orbit := "	      	      	      	    << "circular" << G4endl
		  << "camera zoom factor := 1"  	      	    << G4endl
		  << ";" << G4endl;
  }
  

		
}







// Write the GATE specific scanner information into the header
void GateToInterfile::WriteGateScannerInfo()
{
  m_headerFile  << "GATE GEOMETRY :="               	  << G4endl;

  GateVVolume *baseInserter  = m_system->GetBaseComponent()->GetCreator();
  m_headerFile  << "head x dimension (cm) := "            <<  2.* baseInserter->GetCreator()->GetHalfDimension(0)/cm << G4endl
      	      	<< "head y dimension (cm) := "            <<  2.* baseInserter->GetCreator()->GetHalfDimension(1)/cm << G4endl
      	      	<< "head z dimension (cm) := "            <<  2.* baseInserter->GetCreator()->GetHalfDimension(2)/cm << G4endl
      	      	<< "head material := "                	  <<  baseInserter->GetCreator()->GetMaterialName() << G4endl
		<< "head x translation (cm) := "      	  <<  baseInserter->GetVolumePlacement()->GetTranslation().x()/cm << G4endl
		<< "head y translation (cm) := "      	  <<  baseInserter->GetVolumePlacement()->GetTranslation().y()/cm << G4endl
		<< "head z translation (cm) := "      	  <<  baseInserter->GetVolumePlacement()->GetTranslation().z()/cm << G4endl;

  GateVVolume *crystalInserter  = m_system->GetCrystalComponent()->GetCreator();
  if ( crystalInserter )
    m_headerFile  << "crystal x dimension (cm) := "         <<  2.* crystalInserter->GetCreator()->GetHalfDimension(0)/cm << G4endl
      	      	  << "crystal y dimension (cm) := "         <<  2.* crystalInserter->GetCreator()->GetHalfDimension(1)/cm << G4endl
      	      	  << "crystal z dimension (cm) := "         <<  2.* crystalInserter->GetCreator()->GetHalfDimension(2)/cm << G4endl
      	      	  << "crystal material := "                 <<  crystalInserter->GetCreator()->GetMaterialName() << G4endl;

  GateVVolume *pixelInserter  = m_system->GetPixelComponent()->GetCreator();
  if ( pixelInserter )
    m_headerFile  << "crystal x dimension (cm) := "         <<  2.* pixelInserter->GetCreator()->GetHalfDimension(0)/cm << G4endl
      	      	  << "crystal y dimension (cm) := "         <<  2.* pixelInserter->GetCreator()->GetHalfDimension(1)/cm << G4endl
      	      	  << "crystal z dimension (cm) := "         <<  2.* pixelInserter->GetCreator()->GetHalfDimension(2)/cm << G4endl
      	      	  << "crystal material := "                 <<  pixelInserter->GetCreator()->GetMaterialName() << G4endl;

  m_headerFile  << ";" << G4endl;
}


// Write the GATE specific run information into the header
void GateToInterfile::WriteGateRunInfo(G4int runNb)
{
  m_headerFile  << "GATE SIMULATION :="               	  << G4endl
      	      	<< "number of runs := "               	  << runNb << G4endl
		<< ";" << G4endl;
}


