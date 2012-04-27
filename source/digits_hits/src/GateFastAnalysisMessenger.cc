/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#ifdef GATE_USE_OPTICAL

#include "GateFastAnalysisMessenger.hh"
#include "GateFastAnalysis.hh"

GateFastAnalysisMessenger::GateFastAnalysisMessenger(GateFastAnalysis* gateFastAnalysis)
  : GateOutputModuleMessenger(gateFastAnalysis), m_gateFastAnalysis(gateFastAnalysis)
{}

GateFastAnalysisMessenger::~GateFastAnalysisMessenger()
{}

void GateFastAnalysisMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ GateOutputModuleMessenger::SetNewValue(command,newValue);}

#endif
