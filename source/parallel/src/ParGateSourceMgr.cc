/*----------------------
 GATE version name: gate_v6

 Copyright (C): OpenGATE Collaboration

 This software is distributed under the terms
 of the GNU Lesser General  Public Licence (LGPL)
 See GATE/LICENSE.txt for further details
 ----------------------*/

/*
 \class  ParGateSourceMgr.cc
 \author copernicus email:copernicus231@gmail.com
 */

#ifdef GATE_PARALLEL_MPI

#include "ParGateSourceMgr.hh"
#include <sstream>

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Navigator.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"
#include "GateSourceMgr.hh"
#include "GateSourceMgrMessenger.hh"
#include "GateSourceVoxellized.hh"
#include "GateSourceLinacBeam.hh"
#include "GateClock.hh"
#include "GateApplicationMgr.hh"
#include "GateRTPhantomMgr.hh"
#include <vector>
#include <cmath>
#include "GateActions.hh"
#include "G4RunManager.hh"
#include "ParGateMPI.hh"
#include <mpi.h>

void ParGateSourceMgr::WaitForTimeRequests() {
	G4double result;
	MPI_Status status;
	MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, GET_CURRENT_TIME,
			ParGateMPI::GetInstance()->GetSourceWorldComm(), &status);
	result = GetCentralTime();
	MPI_Send(&result, 1, MPI_DOUBLE, status.MPI_SOURCE, CURRENT_TIME,
			ParGateMPI::GetInstance()->GetSourceWorldComm());
	MPI_Recv(&result, 1, MPI_DOUBLE, status.MPI_SOURCE, SET_CURRENT_TIME,
			ParGateMPI::GetInstance()->GetSourceWorldComm(), NULL);
	SetCentralTime(result);
}

G4double ParGateSourceMgr::GetCentralTime() {
	G4double result;

	if (ParGateMPI::GetInstance()->GetSourceWorldRank() == 0) {
		result = m_time;
	} else {
		MPI_Send(NULL, 0, MPI_INT, 0, GET_CURRENT_TIME,
				ParGateMPI::GetInstance()->GetSourceWorldComm());
		MPI_Recv(&result, 1, MPI_DOUBLE, 0, CURRENT_TIME,
				ParGateMPI::GetInstance()->GetSourceWorldComm(), NULL);
	}
	return result;
}

void ParGateSourceMgr::SetCentralTime(G4double value) {
	if (ParGateMPI::GetInstance()->GetSourceWorldRank() == 0) {
		if (value > 0.) {
			m_time = value;
		} else {
			throw GateMPIException("Unexcpected Negative value for m_time");
		}
	} else {
		MPI_Send(&value, 1, MPI_DOUBLE, 0, SET_CURRENT_TIME,
				ParGateMPI::GetInstance()->GetSourceWorldComm()); //CANNOT BE NON BLOCKING BECAUSE AFTER USE THE FUCTION IT GOES OUT OF SCOPE MAYBE WE CAN PUT VALUE AS MEMBER
	}

}

ParGateSourceMgr::ParGateSourceMgr() :
		GateSourceMgr() {
	numberOfEvents = ParGateMPI::GetInstance()->GetNumberOfEvents();
}

ParGateSourceMgr::~ParGateSourceMgr() {

}

G4int ParGateSourceMgr::PrepareNextEvent(G4Event *event) {

	GateVSource* source;
	GateSteppingAction* myAction =
			(GateSteppingAction *) (G4RunManager::GetRunManager()->GetUserSteppingAction());
	TrackingMode theMode = myAction->GetMode();
	m_currentSources.clear();

	G4int numVertices = 0;

	if ((theMode == 1) || (theMode == 2)) {

		GateRTPhantomMgr::GetInstance()->UpdatePhantoms(m_time); /* PY Descourt 11/12/2008 */

		if (mVerboseLevel > 1)
			G4cout << "GateSourceMgr::PrepareNextEvent" << G4endl;

		// ask the source for this event
		if (mVerboseLevel > 1)
			G4cout << "GateSourceMgr::PrepareNextEvent : GetNextSource() "
					<< G4endl;
		if (sourceIndex == numberOfEvents) {
			pFirstSource_vector.clear();
			m_firstTime_vector.clear();
			m_currentSourceID_vector.clear();
			source = GetNextSource();
		}
		source = pFirstSource_vector[sourceIndex];
		m_firstTime = m_firstTime_vector[sourceIndex];
		m_currentSourceID = m_currentSourceID_vector[sourceIndex];/*EQUIVALENT CODE TO  PY Descourt 08/09/2009 */
		sourceIndex++;
		if (source) {
			// obsolete: to avoid the initialization phase for the source if it's the same as
			// the previous event (always the same with only 1 source). Not needed now with one gps
			// per source
			if (source != m_previousSource)
				m_needSourceInit = true;
			m_previousSource = source;

			// save the information, that can then be asked during the analysis phase
			m_currentSources.push_back(source);

			// update the internal time
			m_time += m_firstTime;

			GateApplicationMgr* appMgr = GateApplicationMgr::GetInstance();
			G4double timeStop = appMgr->GetTimeStop();
			appMgr->SetCurrentTime(m_time);

			if (mVerboseLevel > 1)
				G4cout << "GateSourceMgr::PrepareNextEvent :  m_time (s) "
						<< m_time / s << "  m_timeLimit (s) " << m_timeLimit / s
						<< G4endl;

			// Warning: the comparison  m_time <= m_timeLimit should be wrong due to decimal floating point problem

			/*  if (((!appMgr->IsAnAmountOfPrimariesPerRunModeEnabled() && ( m_time <= m_timeLimit ))
			 || (appMgr->IsAnAmountOfPrimariesPerRunModeEnabled()
			 && (mNbOfParticleInTheCurrentRun < appMgr->GetNumberOfPrimariesPerRun()) ))
			 && ( m_time <= timeStop ) ) */
			//      if( (  m_timeLimit - m_time >= -0.001 ) && ( m_time <= timeStop ) )
			// G4cout << m_time - m_timeLimit<<"   "<<m_firstTime<<"    "<<m_firstTime*(1-1.E-10) <<"  "<< (m_time - m_timeLimit) - m_firstTime << G4endl;
			if ((!appMgr->IsTotalAmountOfPrimariesModeEnabled()
					&& (m_time <= m_timeLimit) && (m_time <= timeStop))
					|| (appMgr->IsTotalAmountOfPrimariesModeEnabled()
							&& appMgr->IsAnAmountOfPrimariesPerRunModeEnabled()
							&& (mNbOfParticleInTheCurrentRun
									< appMgr->GetNumberOfPrimariesPerRun())
							&& (m_time - timeStop <= m_firstTime))
					|| (appMgr->IsTotalAmountOfPrimariesModeEnabled()
							&& (fabs(m_time - m_timeLimit - m_firstTime)
									> m_firstTime * 0.5)
							&& (m_time - timeStop <= m_firstTime))) {
				if (mVerboseLevel > 1)
					G4cout
							<< "GateSourceMgr::PrepareNextEvent : source selected <"
							<< source->GetName() << ">" << G4endl;

				// transmit the time to the source and ask it to generate the primary vertex
				source->SetTime(m_time);
				source->SetNeedInit(m_needSourceInit);
				SetWeight(appMgr->GetWeight());
				source->SetSourceWeight(GetWeight());
				mNumberOfEventBySource[source->GetSourceID() + 1] += 1;
				numVertices = source->GeneratePrimaries(event);
			} else {
				if (mVerboseLevel > 0)
					G4cout
							<< "GateSourceMgr::PrepareNextEvent : m_time > m_timeLimit. No vertex generated"
							<< G4endl;

				if (m_time <= timeStop) {
					m_time -= m_firstTime;
					appMgr->SetCurrentTime(m_time);
				}
			}
		} else {
			G4cout
					<< "GateSourceMgr::PrepareNextEvent : WARNING : GateSourceMgr::GetNextSource gave no source"
					<< G4endl;
		}

		m_needSourceInit = false;

		mNbOfParticleInTheCurrentRun++;
	} // normal or Tracker Modes

	if (theMode == 3) // detector mode
			{
		m_currentSources.push_back(m_fictiveSource);
		//G4cout << "GateSourceMgr::PrepareNextEvent :   m_fictiveSource = " << m_fictiveSource << G4endl;
		numVertices = m_fictiveSource->GeneratePrimaries(event);
		m_fictiveSource->SetTime(m_time); // time has been set in GeneratePrimaries

		//	G4cout << "GateSourceMgr::PrepareNextEvent :::::::      Time " << m_time/s << " time limit " << m_timeLimit/s << G4endl;

		if (m_time > m_timeLimit) {
			numVertices = 0;
		}

	}

	if (mVerboseLevel > 1)
		G4cout << "GateSourceMgr::PrepareNextEvent : numVertices : "
				<< numVertices << G4endl;
	return numVertices;
}

G4int ParGateSourceMgr::PrepareNextRun(const G4Run *r) {
	//  GateMessage("Acquisition", 0, "PrepareNextRun "  << r->GetRunID() << G4endl);
	if (mVerboseLevel > 1)
		G4cout << "GateSourceMgr::PrepareNextRun" << G4endl;

	// Initialize the internal time to the GATE clock time
	mNbOfParticleInTheCurrentRun = 0;
	GateClock* theClock = GateClock::GetInstance();
	m_timeClock = theClock->GetTime();
	m_time = m_timeClock;
	numberOfEvents = ParGateMPI::GetInstance()->GetNumberOfEvents();
	sourceIndex = numberOfEvents;
	m_currentSourceNumber++;
	//G4cout<<"Time Clock = "<<m_time<<G4endl;
	// Get the next time
	GateApplicationMgr* appMgr = GateApplicationMgr::GetInstance();
	//G4cout<<"test GetTimeSlice"<<G4endl;
	G4double timeSlice = appMgr->GetTimeSlice(r->GetRunID());
	//m_timeLimit = m_time + timeSlice;
	m_timeLimit = appMgr->GetEndTimeSlice(r->GetRunID());
	//mCurrentSliceTotalTime = timeSlice;
	//mCurrentSliceStartTime = m_time;
	//mCurrentSliceStopTime = m_timeLimit;
	//   GateMessage("Acquisition", 0,
	//               "Run from t="  << mCurrentSliceStartTime/s << " sec to "
	//               << mCurrentSliceStopTime/s << " sec." << G4endl);

	if (mVerboseLevel > 1)
		G4cout << "GateSourceMgr::PrepareNextRun : m_time      (s) "
				<< m_time / s << G4endl
				<< "                                  m_timeClock (s) "
				<< m_timeClock / s << G4endl
				<< "                                  timeSlice   (s) "
				<< timeSlice / s << G4endl
				<< "                                  m_timeLimit (s) "
				<< m_timeLimit / s << G4endl;

	//! sending commands to the GateRDM
	G4UImanager* UImanager = G4UImanager::GetUIpointer();
	G4String command;
	UImanager->ApplyCommand("/grdm/analogueMC 1");
	UImanager->ApplyCommand("/grdm/verbose 0");
	UImanager->ApplyCommand("/grdm/allVolumes");

	// set time limit of the GateRDM decay
	char timechar[10];
	sprintf(timechar, "%8g", timeSlice / s);
	command = G4String("/gate/decay/setDecayTimeLimit ") + G4String(timechar)
			+ G4String(" s");
	if (mVerboseLevel > 3)
		G4cout << "GateSourceMgr::PrepareNextEvent: command " << command
				<< G4endl;
	command = G4String("/gate/decay/setDecayTimeLimit ") + G4String(timechar)
			+ G4String(" s");
	if (mVerboseLevel > 3)
		G4cout << "GateSourceMgr::PrepareNextEvent: command " << command
				<< G4endl;
	UImanager->ApplyCommand(command.c_str());
	// tell to the GateRDM to avoid the generation of the sampled decay time for the ions
	// (the time is set by the SourceMgr)
	UImanager->ApplyCommand("/gate/decay/setPrimaryDecayTimeGeneration 0");

	// flag for the initialization of the sources
	m_needSourceInit = true;

	// Update the sources (for example for new positioning according to the geometry movements)
	for (size_t is = 0; is != mSources.size(); ++is)
		(mSources[is])->Update(m_time);

	m_runNumber++;

	// if(m_runNumber==0)
	//     {
	//       DD("TODO !!! autoweight");
	/*
	 double totParticles = 0.;
	 for( size_t i = 0; i != mSources.size(); ++i ) {
	 mSources[i]->SetActivity();
	 totParticles += mListOfTimeSlices[i]*listOfActivity[i];
	 if(mSources[i]->GetSourceWeight()!=1. && m_use_autoweight)
	 GateError("You use the macro 'UseSameNumberOfParticlesPerRun' and you define a manual weight");
	 }
	 if(m_use_autoweight){
	 for( size_t i = 0; i != mSources.size(); ++i ){
	 listOfWeight[i]=mListOfTimeSlices[i]*listOfActivity[i]/totParticles;
	 mSources[i]->SetSourceWeight(listOfWeight[i]);
	 mSources[i]->SetActivity(m_TotNPart/mListOfTimeSlices[i]);
	 }
	 }
	 */
	//  }
	return 0;
}

GateVSource *ParGateSourceMgr::GetNextSource() {
	// GateDebugMessage("Acquisition", 0, "GetNextSource" << G4endl);

	// the method decides which is the source that has to be used for this event
	GateVSource* pFirstSource = 0;

	sourceIndex = 0;
	G4double time_sum;
	time_sum = 0.;
	pFirstSource_vector.reserve(numberOfEvents);
	m_firstTime_vector.reserve(numberOfEvents);
	m_currentSourceID_vector.reserve(numberOfEvents);

	m_firstTime = -1.;

	if (mSources.size() == 0) {
		G4cout << "GateSourceMgr::GetNextSource : WARNING: No source available"
				<< G4endl;
		return NULL; // GateError ???
	}

	G4double aTime;

	if (IsTotalAmountOfPrimariesModeEnabled()) { //TODO IMPLEMENT FOR PARALLEL IS EASY :P
		G4double randNumber = ((double) std::rand() / ((double) RAND_MAX + 1)
				* mTotalIntensity);
		G4double sumIntensity = 0.;
		G4int currentSourceNumber = 0;
		while ((currentSourceNumber < (int) mSources.size())
				&& (sumIntensity <= randNumber)) {
			pFirstSource = mSources[currentSourceNumber];
			sumIntensity += pFirstSource->GetIntensity();
			currentSourceNumber++;
		}

		m_firstTime =
				GateApplicationMgr::GetInstance()->GetTimeStepInTotalAmountOfPrimariesMode();
		// DD(m_firstTime);
	} else {
		m_time = GetCentralTime();

		// if there is at least one source
		// make a competition among all the available sources
		// the source that proposes the shortest interval for the next event wins
		for (int r = 0; r < numberOfEvents; r++) {
			m_firstTime = -1.;
			for (size_t is = 0; is != mSources.size(); ++is) {
				aTime = mSources[is]->GetNextTime(m_time + time_sum); // compute random time for this source
				if (mVerboseLevel > 1)
					G4cout << "GateSourceMgr::GetNextSource : source "
							<< mSources[is]->GetName() << "    Next time (s) : "
							<< aTime / s << "   m_firstTime (s) : "
							<< m_firstTime / s << G4endl;

				if (m_firstTime < 0. || (aTime < m_firstTime)) {
					m_firstTime = aTime;
					pFirstSource = mSources[is];
				}
			}
			if (pFirstSource != 0
					&& m_time + time_sum + m_firstTime < m_timeLimit) { //dont calculate sources that are not needed

				pFirstSource_vector.push_back(pFirstSource);
				m_currentSourceID_vector.push_back(pFirstSource->GetSourceID());
				time_sum = time_sum + m_firstTime;
				m_firstTime_vector.push_back(m_firstTime);
				pFirstSource = 0;
				m_currentSourceID = -1;
			} else if (pFirstSource != 0) {
				pFirstSource_vector.push_back(pFirstSource);
				m_currentSourceID_vector.push_back(pFirstSource->GetSourceID());
				m_firstTime_vector.push_back(m_firstTime);
				pFirstSource = 0;
				m_currentSourceID = -1;
				GateApplicationMgr* appMgr = GateApplicationMgr::GetInstance();
				G4double timeStop = appMgr->GetTimeStop();
				if (timeStop <= m_time + time_sum + m_firstTime) { //tricky condition for end the slice or all the simulation
					time_sum = time_sum + m_firstTime;
				}
				break;
			}
		}
	}

	SetCentralTime(m_time + time_sum);
	return pFirstSource;
}

#endif
