/*----------------------
  GATE version name: gate_v6

  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/

#include "GateDigitizer.hh"

#include "GateTools.hh"
#include "GateDigitizerMessenger.hh"
#include "GateSingleDigiMaker.hh"
#include "GateHitConvertor.hh"
#include "GateOutputMgr.hh"


GateDigitizer* GateDigitizer::theDigitizer=0;

//-----------------------------------------------------------------
GateDigitizer* GateDigitizer::GetInstance()
{
  if (!theDigitizer)
    theDigitizer = new GateDigitizer;
  return theDigitizer;
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
GateDigitizer::GateDigitizer()
  : GateClockDependent("digitizer"),
    G4VDigitizerModule("digitizer"),
    m_elementTypeName("digitizer module"),
    m_system(0)
{
  m_messenger = new GateDigitizerMessenger(this);

  m_hitConvertor = GateHitConvertor::GetInstance();
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
GateDigitizer::~GateDigitizer()
{  
  delete m_hitConvertor;
  while (m_singleChainList.size())
  {
      delete m_singleChainList.back();
      m_singleChainList.erase(m_singleChainList.end()-1);
  }
  while ( m_coincidenceSorterList.size() ) {
    delete m_coincidenceSorterList.back();
    m_coincidenceSorterList.erase(m_coincidenceSorterList.end()-1);
  }
  while ( m_digiMakerList.size() ) {
    delete m_digiMakerList.back();
    m_digiMakerList.erase(m_digiMakerList.end()-1);
  }
  delete m_messenger;
  theDigitizer = 0;
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
GateNamedObject* GateDigitizer::FindElement(const G4String& name)
{
  size_t i;
  for (i=0; i<m_singleChainList.size() ; i++)
    if (m_singleChainList[i]->GetObjectName() == name)
      return m_singleChainList[i];
  for (i=0; i<m_coincidenceSorterList.size() ; i++)
    if (m_coincidenceSorterList[i]->GetObjectName() == name)
      return m_coincidenceSorterList[i];
  return 0;
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void GateDigitizer::ListElements(size_t indent)
{
  DescribeChains(indent);
  DescribeSorters(indent);
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void GateDigitizer::InsertChain(GatePulseProcessorChain* newChain)
{
  m_singleChainList.push_back(newChain);
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void GateDigitizer::InsertCoincidenceChain(GateCoincidencePulseProcessorChain* newChain)
{
  m_coincidenceChainList.push_back(newChain);
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void GateDigitizer::DescribeChains(size_t indent)
{
  G4cout << GateTools::Indent(indent) << "Nb of chains:       " << m_singleChainList.size() << "\n";
  for (size_t i=0; i<m_singleChainList.size(); i++)
      G4cout << GateTools::Indent(indent+1) << GetChain(i)->GetObjectName() << G4endl;
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void GateDigitizer::DescribeSorters(size_t indent)
{
  G4cout << GateTools::Indent(indent) << "Nb of coinc. units: " << m_coincidenceSorterList.size() << "\n";
  for (size_t i=0; i<m_coincidenceSorterList.size(); i++)
      G4cout << GateTools::Indent(indent+1) << m_coincidenceSorterList[i]->GetObjectName() << G4endl;
}

// Clear the array of pulse-lists
void GateDigitizer::ErasePulseListVector()
{
  while (m_pulseListVector.size()) {
        if (nVerboseLevel>1)
          G4cout << "[GateDigitizer::ErasePulseListVector]: Erasing pulse-list '" << m_pulseListVector.back()->GetListName() << "'" << G4endl;
        delete m_pulseListVector.back();
        m_pulseListVector.erase(m_pulseListVector.end()-1);
  }
  while (m_coincidencePulseVector.size()) {
        if (nVerboseLevel>1)
          G4cout << "[GateDigitizer::ErasePulseListVector]: Erasing coincidence pulse" << G4endl;
        delete m_coincidencePulseVector.back();
        m_coincidencePulseVector.erase(m_coincidencePulseVector.end()-1);
  }
  while (m_pulseListAliasVector.size()) {
        if (nVerboseLevel>1)
          G4cout << "[GateDigitizer::ErasePulseListVector]: Erasing alias '" << m_pulseListAliasVector.back().first << "'" << G4endl;
        m_pulseListAliasVector.erase(m_pulseListAliasVector.end()-1);
  }
  while (m_coincidencePulseListAliasVector.size()) {
        if (nVerboseLevel>1)
          G4cout << "[GateDigitizer::ErasePulseListVector]: Erasing alias '" << m_coincidencePulseListAliasVector.back().first << "'" << G4endl;
        m_coincidencePulseListAliasVector.erase(m_coincidencePulseListAliasVector.end()-1);
  }
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Store a new pulse-list into the array of pulse-list 
void GateDigitizer::StorePulseList(GatePulseList* newPulseList)
{
  if (newPulseList) {
    if (nVerboseLevel>1)
      G4cout << "[GateDigitizer::StorePulseList]: Storing new pulse-list '" << newPulseList->GetListName() << "'" << G4endl;
    m_pulseListVector.push_back(newPulseList);
  }
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Store a new coincidence pulse into the array of coincidence pulses 
void GateDigitizer::StoreCoincidencePulse(GateCoincidencePulse* newCoincidencePulse)
{
  if (newCoincidencePulse) {
    if (nVerboseLevel>1)
      G4cout << "[GateDigitizer::StoreCoincidencePulse]: Storing new coincidence pulse" << G4endl;
    m_coincidencePulseVector.push_back(newCoincidencePulse);
  }
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Store a new alias for a pulse-list 
void GateDigitizer::StorePulseListAlias(const G4String& aliasName,GatePulseList* aPulseList)
{
  if (aPulseList) {
    if (nVerboseLevel>1)
      G4cout << "[GateDigitizer::StorePulseListAlias]: Storing new alias '" << aliasName
             << "' for list '" << aPulseList->GetListName() << "'"<< G4endl;
    m_pulseListAliasVector.push_back(GatePulseListAlias(aliasName,aPulseList));
  }
}
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Store a new alias for a coincidence pulse
void GateDigitizer::StoreCoincidencePulseAlias(const G4String& aliasName,GateCoincidencePulse* aPulse)
{
  if (aPulse) {
    if (nVerboseLevel>1)
      G4cout << "[GateDigitizer::StoreCoincidencePulseListAlias]: Storing new alias '" << aliasName
             << "' for coincidence '" << aPulse->GetListName() << "'"<< G4endl;
    m_coincidencePulseListAliasVector.push_back(GateCoincidencePulseListAlias(aliasName,aPulse));
  }
}


// Find a pulse-list from the array of pulse-list 
GatePulseList* GateDigitizer::FindPulseList(const G4String& pulseListName)
{
  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::FindPulseList]: Looking for pulse-list '" << pulseListName << "'"<< G4endl;

  size_t i;
  for (i=0; i<m_pulseListAliasVector.size(); ++i)
    if ( m_pulseListAliasVector[i].first == pulseListName) {
      if (nVerboseLevel>1)
        G4cout << "[GateDigitizer::FindPulseList]: Found pulse-list '" << pulseListName << "'"<< G4endl;
      return m_pulseListAliasVector[i].second;
    }
  for (i=0; i<m_pulseListVector.size(); ++i)
    if ( m_pulseListVector[i]->GetListName() == pulseListName) {
      if (nVerboseLevel>1)
        G4cout << "[GateDigitizer::FindPulseList]: Found pulse-list alias '" << pulseListName << "'"<< G4endl;
      return m_pulseListVector[i];
    }
  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::FindPulseList]: Cound not find pulse-list '" << pulseListName << "'"<< G4endl;
  return 0;    
}



// Find a pulse-list from the array of pulse-list 
std::vector<GateCoincidencePulse*> GateDigitizer::FindCoincidencePulse(const G4String& pulseName)

{
  std::vector<GateCoincidencePulse*> ans;
  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::FindCoincidencePulse]: Looking for coincidence pulse '" << pulseName << "'"<< G4endl;

  size_t i;
  for (i=0; i<m_coincidencePulseListAliasVector.size(); ++i)
    if ( m_coincidencePulseListAliasVector[i].first == pulseName) {
      if (nVerboseLevel>1)
        G4cout << "[GateDigitizer::FindCoincidencePulse]: Found pulse-list '" << pulseName << "'"<< G4endl;
      ans.push_back(m_coincidencePulseListAliasVector[i].second);
    }
  for (i=0; i<m_coincidencePulseVector.size(); ++i){
    if ( m_coincidencePulseVector[i]->GetListName() == pulseName) {
      if (nVerboseLevel>1)
        G4cout << "[GateDigitizer::FindCoincidencePulse]: Found coincidence pulse '" << pulseName << "'"<< G4endl;
      ans.push_back(m_coincidencePulseVector[i]);
    }
  }
  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::FindCoincidencePulse]: Cound not find coincidence pulse '" << pulseName << "'"<< G4endl;

  return ans;
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Integrates a new pulse-processor chain 
void GateDigitizer::StoreNewPulseProcessorChain(GatePulseProcessorChain* processorChain)
{
  G4String outputName = processorChain->GetOutputName() ; 
  if (nVerboseLevel)
    G4cout << "[GateDigitizer::StoreNewPulseProcessorChain]: Storing new processor chain '" << processorChain->GetObjectName() << "'" 
           << " with output pulse-list name '" << outputName << "'" << G4endl;

  InsertChain ( processorChain );
}
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Integrates a new coincidence-processor chain 
void GateDigitizer::StoreNewCoincidenceProcessorChain(GateCoincidencePulseProcessorChain* processorChain)
{
  G4String outputName = processorChain->GetOutputName() ; 
  if (nVerboseLevel)
    G4cout << "[GateDigitizer::StoreNewPulseProcessorChain]: Storing new processor chain '" << processorChain->GetObjectName() << "'" 
           << " with output pulse-list name '" << outputName << "'" << G4endl;

  InsertCoincidenceChain ( processorChain );
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Integrates a new coincidence sorter 
void GateDigitizer::StoreNewCoincidenceSorter(GateCoincidenceSorter* coincidenceSorter)
{
  G4String outputName = coincidenceSorter->GetOutputName() ; 
  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::StoreNewCoincidenceSorter]: Storing new coincidence sorter '" << coincidenceSorter->GetObjectName() << "'" 
           << " with output coincidence-pulse name '" << outputName << "'" << G4endl;

  m_coincidenceSorterList.push_back ( coincidenceSorter );
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Print-out a description of the object
void GateDigitizer::Describe(size_t indent)
{
  GateClockDependent::Describe(indent);
  ListElements(indent);
  G4cout << GateTools::Indent(indent) << "Hit convertor:      '" << m_hitConvertor->GetObjectName() << "'" <<G4endl;
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void GateDigitizer::Digitize()
{
  if ( !IsEnabled() )
    return;

  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::Digitize]: starting" << G4endl;

  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::Digitize]: erasing pulse-lists" << G4endl;
  ErasePulseListVector();

  // Convert the hits into pulses
  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::Digitize]: launching hit conversion" << G4endl;
  GateCrystalHitsCollection* CHC = GateOutputMgr::GetInstance()->GetCrystalHitCollection();
  m_hitConvertor->ProcessHits(CHC);

  // Have the hits processed by the pulse-processor chains
  size_t i;
  for (i=0; i<GetChainNumber() ; ++i) {
    if (nVerboseLevel>1)
      G4cout << "[GateDigitizer::Digitize]: launching processor chain '" << GetChain(i)->GetObjectName() << "'" << G4endl;
    GetChain(i)->ProcessPulseList();
  }
  
  // Have the pulses processed by the coincidence sorters
  for (i=0; i<m_coincidenceSorterList.size() ; ++i) {
    if (nVerboseLevel>1)
      G4cout << "[GateDigitizer::Digitize]: launching coincidence sorter '" << m_coincidenceChainList[i]->GetObjectName() << "'" << G4endl;
    m_coincidenceSorterList[i]->ProcessSinglePulseList();
  }
 
  // Have the coincidences processed by the coincidence-processor chains
  for (i=0; i<m_coincidenceChainList.size() ; ++i) {
    if (nVerboseLevel>1)
      G4cout << "[GateDigitizer::Digitize]: launching coincidence-processor '" << m_coincidenceSorterList[i]->GetObjectName() << "'" << G4endl;
    m_coincidenceChainList[i]->ProcessCoincidencePulses();
  }

  for (i=0; i<m_digiMakerList.size() ; ++i) {
    if (nVerboseLevel>1)
      G4cout << "[GateDigitizer::Digitize]: launching digitizer module '" << m_digiMakerList[i]->GetObjectName() << "'" << G4endl;
    m_digiMakerList[i]->Digitize();
  }

  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::Digitize]: completed" << G4endl;
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void GateDigitizer::InsertDigiMakerModule(GateVDigiMakerModule* newDigiMakerModule)
{
  if (nVerboseLevel>1)
    G4cout << "[GateDigitizer::InsertDigiMakerModule]: storing new digi-maker module '" << newDigiMakerModule->GetObjectName() << "'" 
    	   << " with collection name " << newDigiMakerModule->GetCollectionName() << "'" << G4endl;
  m_digiMakerList.push_back(newDigiMakerModule);
  StoreCollectionName(newDigiMakerModule->GetCollectionName());
}
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void GateDigitizer::SetSystem(GateVSystem* aSystem) 
{
  m_system = aSystem;
  size_t i;
  for (i=0; i<GetChainNumber() ; ++i)
    GetChain(i)->SetSystem(aSystem);
  for (i=0; i<m_coincidenceSorterList.size() ; ++i)
    m_coincidenceSorterList[i]->SetSystem(aSystem);
}
//-----------------------------------------------------------------

