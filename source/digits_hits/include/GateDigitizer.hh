/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#ifndef GateDigitizer_h
#define GateDigitizer_h 1

#include "globals.hh"
#include <vector>
#include "G4VDigitizerModule.hh"

#include "GateClockDependent.hh"
#include "GatePulseProcessorChain.hh"
#include "GateCoincidencePulseProcessorChain.hh"
#include "GateCoincidencePulse.hh"
#include "GateCoincidenceSorter.hh"
#include "GateVDigiMakerModule.hh"

class GateDigitizerMessenger;
class GateHitConvertor;
class GateVDigiMakerModule;
class GateVSystem;

class GateDigitizer : public GateClockDependent,public G4VDigitizerModule
{
  public:
    static GateDigitizer* GetInstance();

  protected:
    GateDigitizer();

  public:
    virtual ~GateDigitizer();

    virtual void InsertChain(GatePulseProcessorChain* newChain);
    virtual void InsertCoincidenceChain(GateCoincidencePulseProcessorChain* newChain);

    virtual void DescribeChains(size_t indent);
    virtual void DescribeSorters(size_t indent);
    virtual void ListElements(size_t indent=0);
    virtual GatePulseProcessorChain* GetChain(size_t i)
      	  {return (i<m_singleChainList.size()) ? m_singleChainList[i] : 0; }
     virtual inline size_t size() const
      	  {return m_singleChainList.size(); }
    virtual size_t GetChainNumber() const
      	  { return size();}
     virtual GateNamedObject* FindElement(const G4String& name);
     virtual inline GateNamedObject* FindElementByBaseName(const G4String& baseName)
       { return FindElement( MakeElementName(baseName) ) ; }
     inline const G4String&  GetElementTypeName()
      	  { return m_elementTypeName;}

    virtual inline G4String MakeElementName(const G4String& newBaseName)
      { return GetObjectName() + "/" + newBaseName; }

  virtual inline GateVSystem* GetSystem() const 
    { return m_system;}
  virtual void SetSystem(GateVSystem* aSystem);


  // Print-out a description of the object
  void Describe(size_t indent);

  //! Store a new pulse-list into the array of pulse-list 
  void StorePulseList(GatePulseList* newPulseList);

  //! Store a new alias for a pulse-list 
  void StorePulseListAlias(const G4String& aliasName,GatePulseList* aPulseList);

  //! Store a new alias for a pulse-list 
  void StoreCoincidencePulseAlias(const G4String& aliasName,GateCoincidencePulse* aPulse);

  //! Store a new coincidence pulse into the array of coincidence pulses 
  void StoreCoincidencePulse(GateCoincidencePulse* newCoincidencePulse);

  //! Store a new coincidence pulse into the array of coincidence pulses 
  void StoreCoincidenceChain(GateCoincidencePulseProcessorChain* newCoincidenceChain);

  //! Find a pulse-list from the array of pulse-list 
  GatePulseList* FindPulseList(const G4String& pulseListName);

  //! Find a pulse-list from the array of pulse-list 
  std::vector<GateCoincidencePulse*> FindCoincidencePulse(const G4String& pulseName);

  //! Clear the array of pulse-lists
  void ErasePulseListVector();

  //! Integrates a new pulse-processor chain 
  void StoreNewPulseProcessorChain(GatePulseProcessorChain* processorChain);
  //! Integrates a new coincidence-processor chain 
  void StoreNewCoincidenceProcessorChain(GateCoincidencePulseProcessorChain* processorChain);

  //! Integrates a new coincidence sorter 
  void StoreNewCoincidenceSorter(GateCoincidenceSorter* coincidenceSorter);

  void MakeCoincidencePulse(G4int i)
    { m_coincidenceSorterList[i]->ProcessSinglePulseList();}

  virtual void Digitize();

  //! Return the hit convertor attached to the digitizer
  inline GateHitConvertor* GetHitConvertor() 
    { return m_hitConvertor;}

  inline void StoreDigiCollection(G4VDigiCollection* aDC)
    { G4VDigitizerModule::StoreDigiCollection(aDC); }

  inline void StoreCollectionName(const G4String& aCollectionName)
    { collectionName.push_back(aCollectionName.c_str()); }

  virtual void InsertDigiMakerModule(GateVDigiMakerModule* newDigiMakerModule);

  protected:
  G4String 					m_elementTypeName;	 //!< Type-name for all digitizer modules 
    
  GateVSystem*					m_system;		 //<! System to which the digitizer is attached 

  GateHitConvertor*    				m_hitConvertor;	      	 //!< Hit convertor
  std::vector<GatePulseProcessorChain*>    	m_singleChainList;	 //!< Vector of pulse-processor chains
  std::vector<GateCoincidencePulseProcessorChain*>   m_coincidenceChainList;	 //!< Vector of pulse-processor chains
  std::vector<GateCoincidenceSorter*>  		m_coincidenceSorterList; //!< Vector of coincidence sorters

  std::vector<GateVDigiMakerModule*>  		m_digiMakerList;       	 //!< Vector of digi-maker modules
  
  GateDigitizerMessenger*    			m_messenger;


  typedef std::pair<G4String,GatePulseList*> 	GatePulseListAlias;
  typedef std::pair<G4String,GateCoincidencePulse*> GateCoincidencePulseListAlias;

  std::vector<GatePulseList*>            	m_pulseListVector;
  std::vector<GateCoincidencePulse*>     	m_coincidencePulseVector;
  std::vector<GatePulseListAlias>      		m_pulseListAliasVector;
  std::vector<GateCoincidencePulseListAlias>    m_coincidencePulseListAliasVector;

  static GateDigitizer*      			theDigitizer;
};

#endif

