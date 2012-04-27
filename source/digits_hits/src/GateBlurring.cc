/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#include "GateBlurring.hh"

#include "GateBlurringMessenger.hh"
#include "GateTools.hh"

#include "Randomize.hh"

#include "G4UnitsTable.hh"


GateBlurring::GateBlurring(GatePulseProcessorChain* itsChain,
      	      	      	      	 const G4String& itsName,
      	      	      	      	 G4double itsResolution,
			         G4double itsEnergyRef) 
  : GateVPulseProcessor(itsChain,itsName),
    m_resolution(itsResolution),
    m_eref(itsEnergyRef)
{
  m_messenger = new GateBlurringMessenger(this);
}  




GateBlurring::~GateBlurring() 
{
  delete m_messenger;
}  



void GateBlurring::ProcessOnePulse(const GatePulse* inputPulse,GatePulseList& outputPulseList)
{
  if(m_resolution < 0 ) {
    G4cerr << 	G4endl << "[GateBlurring::ProcessOnePulse]:" << G4endl
      	   <<   "Sorry, but the resolution (" << GetResolution() << ") is invalid" << G4endl;
    G4Exception("You must set the energy of reference AND the resolution:\n"
		"\t/gate/digitizer/blurring/setResolution NUMBER\n"
		"or disable the blurring using:\n"
		"\t/gate/digitizer/blurring/disable\n");
  }
     else {
       if(m_eref < 0) {
	 G4cerr <<   G4endl << "[GateBlurring::ProcessOnePulse]:" << G4endl
		<<   "Sorry, but the energy of reference (" << G4BestUnit(GetEnergyRef(),"Energy") << ") is invalid" << G4endl;
	 G4Exception("You must set the resolution AND the energy of reference:\n"
		     "\t/gate/digitizer/blurring/setEnergyOfReference ENERGY\n"
		     "or disable the blurring using:\n"
		     "\t/gate/digitizer/blurring/disable\n");
       }
       else {
	 m_coeff = m_resolution * sqrt(m_eref);
	 GatePulse* outputPulse = new GatePulse(*inputPulse);
	 outputPulse->SetEnergy(G4RandGauss::shoot(inputPulse->GetEnergy(),m_coeff*sqrt(inputPulse->GetEnergy())/2.35));
	 outputPulseList.push_back(outputPulse);
       }
     }
}

void GateBlurring::DescribeMyself(size_t indent)
{
  G4cout << GateTools::Indent(indent) << "Resolution of " << m_resolution  << " for " <<  G4BestUnit(m_eref,"Energy") << G4endl;
}
