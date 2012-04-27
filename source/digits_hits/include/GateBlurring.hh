/*----------------------
   GATE version name: gate_v6

   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#ifndef GateBlurring_h
#define GateBlurring_h 1

#include "globals.hh"
#include <iostream>
#include <vector>
#include "G4ThreeVector.hh"

#include "GateVPulseProcessor.hh"

class GateBlurringMessenger;

/*! \class  GateBlurring
    \brief  Pulse-processor for simulating a Gaussian blurring on the energy spectrum. 

    - GateBlurring - by Martin.Rey@epfl.ch
    
      \sa GateVPulseProcessor
*/      
class GateBlurring : public GateVPulseProcessor
{
  public:

    //! \name constructors and destructors
    //@{

    //! Constructs a new blurring attached to a GateDigitizer
    GateBlurring(GatePulseProcessorChain* itsChain,
		 const G4String& itsName,
		 G4double itsBlurring=0,
		 G4double itsEnergyRef=-1) ;

    //! Destructor
    virtual ~GateBlurring() ; 
    //@}
    
     //! \name getters and setters
    //@{
    //! This function returns the resolution in use.
    G4double GetResolution()   	       { return m_resolution; }

    //! This function returns the energy of reference in use.  
    G4double GetEnergyRef()            { return m_eref; }
    
    //! This function sets the resolution of a gaussian blurring.
    /*! 
      If you want a resolution of 15% to 511 keV, SetResolution(0.15), SetRefEnergy(511. keV) 
      \param val is a real number
    */
    void SetResolution(G4double val)   { m_resolution = val;  }
    //@}
    
    void SetRefEnergy(G4double eval)   { m_eref = eval; }

    //! Implementation of the pure virtual method declared by the base class GateClockDependent
    //! print-out the attributes specific of the blurring
    virtual void DescribeMyself(size_t indent);

  protected:
    //! Implementation of the pure virtual method declared by the base class GateVPulseProcessor
    //! This methods processes one input-pulse
    //! It is is called by ProcessPulseList() for each of the input pulses
    //! The result of the pulse-processing is incorporated into the output pulse-list
    void ProcessOnePulse(const GatePulse* inputPulse,GatePulseList& outputPulseList);
    
  private:
    G4double m_resolution;    	          //!< Simulated energy resolution
    G4double m_eref;                      //!< Simulated energy of reference
    G4double m_coeff;                     //!< Coefficient which connects energy to the resolution

    GateBlurringMessenger *m_messenger;   //!< Messenger
    
};


#endif

