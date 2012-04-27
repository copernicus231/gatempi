/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#include "GateUHadronElasticProcessPB.hh"

#include "GateHadronicStandardProcessMessenger.hh"


//-----------------------------------------------------------------------------
GateUHadronElasticPB::GateUHadronElasticPB():GateVProcess("UHadronElastic")
{   
  SetDefaultParticle("sigma0");  SetDefaultParticle("anti_sigma0");
  SetDefaultParticle("pi+"); SetDefaultParticle("pi-");SetDefaultParticle("pi0");
  SetDefaultParticle("kaon+"); SetDefaultParticle("kaon-");
  SetDefaultParticle("kaon0L");SetDefaultParticle("kaon0S");
  SetDefaultParticle("sigma+"); SetDefaultParticle("sigma-");
  SetDefaultParticle("proton"); SetDefaultParticle("anti_proton");
  SetDefaultParticle("neutron");  SetDefaultParticle("anti_neutron");
  SetDefaultParticle("lambda");SetDefaultParticle("anti_lambda");
  SetDefaultParticle("xi-"); SetDefaultParticle("anti_xi-");
  SetDefaultParticle("anti_sigma+"); SetDefaultParticle("anti_sigma-");
  SetDefaultParticle("omega-"); SetDefaultParticle("anti_omega-");
  SetDefaultParticle("xi0");SetDefaultParticle("anti_xi0");
  SetDefaultParticle("deuteron");
  SetDefaultParticle("triton");
  //SetDefaultParticle("He3");
  SetDefaultParticle("alpha");

  SetProcessInfo("Elastic scattering of all long-lived hadrons and light ions");

  AddToModelList("G4HadronElastic");
  AddToModelList("G4NeutronHPElastic");
  AddToModelList("G4NeutronHPorLElastic");

  AddToDataSetList("G4HadronElasticDataSet");

  //AddToDataSetList("G4QElasticCrossSection"); not yet available
  AddToDataSetList("G4NeutronHPElasticData");
 
  pMessenger = new GateHadronicStandardProcessMessenger(this);  
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
G4VProcess* GateUHadronElasticPB::CreateProcess(G4ParticleDefinition *)
{
  return new G4UHadronElasticProcess(GetG4ProcessName());
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void GateUHadronElasticPB::ConstructProcess( G4ProcessManager * manager)
{
  manager->AddDiscreteProcess(GetProcess());        
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool GateUHadronElasticPB::IsApplicable(G4ParticleDefinition * par)
{
  if(par == G4PionPlus::PionPlus() ||
     par == G4PionZero::PionZero() ||
     par == G4PionMinus::PionMinus() ||
     par == G4KaonPlus::KaonPlus() ||
     par == G4KaonZeroShort::KaonZeroShort() ||
     par == G4KaonZeroLong::KaonZeroLong() ||
     par == G4KaonMinus::KaonMinus() ||
     par == G4Proton::Proton() ||
     par == G4AntiProton::AntiProton() ||
     par == G4Neutron::Neutron() ||
     par == G4AntiNeutron::AntiNeutron() ||
     par == G4Lambda::Lambda() ||
     par == G4AntiLambda::AntiLambda() ||
     par == G4SigmaPlus::SigmaPlus() ||
     par == G4SigmaZero::SigmaZero() ||
     par == G4SigmaMinus::SigmaMinus() ||
     par == G4AntiSigmaPlus::AntiSigmaPlus() ||
     par == G4AntiSigmaZero::AntiSigmaZero() ||
     par == G4AntiSigmaMinus::AntiSigmaMinus() ||
     par == G4XiZero::XiZero() ||
     par == G4XiMinus::XiMinus() ||
     par == G4AntiXiZero::AntiXiZero() ||
     par == G4AntiXiMinus::AntiXiMinus() ||
     par == G4Deuteron::Deuteron() ||
     par == G4Triton::Triton() ||
     par == G4Alpha::Alpha() ||
     par == G4He3::He3() ||
     par == G4OmegaMinus::OmegaMinus() ||
     par == G4AntiOmegaMinus::AntiOmegaMinus()
     )  return true;

  return false;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool GateUHadronElasticPB::IsModelApplicable(G4String model,G4ParticleDefinition * par)
{
  if(model == "G4NeutronHPElastic" && par != G4Neutron::Neutron()) return false;
  if(model == "G4NeutronHPorLElastic" && par != G4Neutron::Neutron()) return false;
  return true;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool GateUHadronElasticPB::IsDatasetApplicable(G4String cs, G4ParticleDefinition * par)
{
  if(cs == "G4NeutronHPElasticData" && par != G4Neutron::Neutron()) return false;
  return true;
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
void GateUHadronElasticPB::AddUserDataSet(G4String ){}
void GateUHadronElasticPB::AddUserModel(GateListOfHadronicModels *){}
//-----------------------------------------------------------------------------



MAKE_PROCESS_AUTO_CREATOR_CC(GateUHadronElasticPB)
