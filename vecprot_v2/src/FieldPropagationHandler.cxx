#include "FieldPropagationHandler.h"

#include "FieldLookup.h"

#include "GUFieldPropagatorPool.h"
#include "GUFieldPropagator.h"
#include "ConstBzFieldHelixStepper.h"
#include "ConstFieldHelixStepper.h"
#include "FieldTrack.h"

#include "GeantTrack.h"

#include "base/SOA3D.h"
// #include "SOA6D.h"
#include "Geant/VectorTypes.h"   // Defines Geant::Double_v etc

#include "FlexIntegrationDriver.h"

#ifdef USE_VECGEOM_NAVIGATOR
#include "navigation/NavigationState.h"
#include "ScalarNavInterfaceVG.h"
#include "ScalarNavInterfaceVGM.h"
#include "VectorNavInterface.h"
#else
#include "ScalarNavInterfaceTGeo.h"
#endif

using Double_v = Geant::Double_v;
          
namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
FieldPropagationHandler::FieldPropagationHandler(int threshold, GeantPropagator *propagator)
               : Handler(threshold, propagator)
{
// Default constructor
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
FieldPropagationHandler::~FieldPropagationHandler()
{
// Destructor
}

        
//______________________________________________________________________________
// Curvature for general field   
VECCORE_ATT_HOST_DEVICE
double FieldPropagationHandler::Curvature(const GeantTrack  & track,
                                          GeantTaskData     * td) const
{
  using ThreeVector_d = vecgeom::Vector3D<double>;
  constexpr double tiny = 1.E-30;
  ThreeVector_d MagFld;
  double bmag= 0.0;

  ThreeVector_d Position(track.fXpos, track.fYpos, track.fZpos);
  FieldLookup::GetFieldValue(Position, MagFld, bmag, td);
  // GetFieldValue(track, MagFld, bmag, td);

  //  Calculate transverse momentum 'Pt' for field 'B'
  // 
  ThreeVector_d Momentum( track.fXdir, track.fYdir, track.fZdir );
  Momentum *= track.fP;
  ThreeVector_d PtransB;  //  Transverse wrt direction of B
  double ratioOverFld = 0.0;
  if( bmag > 0 ) ratioOverFld = Momentum.Dot( MagFld ) / (bmag*bmag);
  PtransB = Momentum - ratioOverFld * MagFld ;
  double Pt_mag = PtransB.Mag();

  return fabs(GeantTrack::kB2C * track.fCharge * bmag / (Pt_mag + tiny));
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void FieldPropagationHandler::DoIt(GeantTrack *track, Basket& output, GeantTaskData *td)
{
// Scalar geometry length computation. The track is moved into the output basket.
  // Step selection
  double step, lmax;
  const double eps = 1.E-2; //

  // We use the track sagitta to estimate the "bending" error,
  // i.e. what is the propagated length for which the track deviation in
  // magnetic field with respect to straight propagation is less than epsilon.
  // Take the maximum between the safety and the "bending" safety
  lmax = SafeLength(*track, td, eps);
  lmax = vecCore::math::Max<double>(lmax, track->fSafety);
  // Select step to propagate as the minimum among the "safe" step and:
  // the straight distance to boundary (if frombdr=1) or the proposed  physics
  // step (frombdr=0)
  step = (track->fBoundary) ?
           vecCore::math::Min<double>(lmax, vecCore::math::Max<double>(track->fSnext, 1.E-4))
         : vecCore::math::Min<double>(lmax, track->fPstep);
  // Propagate in magnetic field
  PropagateInVolume(*track, step, td);
  //Update number of partial steps propagated in field
  td->fNmag++;
  // Update time of flight and number of interaction lengths
//  track->fTime += track->TimeStep(track->fStep);
//  track->fNintLen -= track->fStep/track->fIntLen;

  // Set continuous processes stage as follow-up for tracks that reached the
  // physics process
  if (track->fStatus == kPhysics) {
    // Update number of steps to physics and total number of steps
    td->fNphys++;
    td->fNsteps++;
#ifdef USE_REAL_PHYSICS
//    track->SetStage(kAlongStepActionStage);
    track->SetStage(kPostStepActionStage);
#else
    track->SetStage(kContinuousProcStage);
#endif
    output.AddTrack(track);
    return;
  }

  // Crossing tracks continue to continuous processes, the rest have to
  // query again the geometry
  if (!IsSameLocation(*track, td)) {
    td->fNcross++;
    td->fNsteps++;
  } else {
    track->SetStage(kGeometryStepStage);
  }
  output.AddTrack(track);
}


//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void FieldPropagationHandler::DoIt(Basket &input, Basket& output, GeantTaskData *td)
{
// Vector geometry length computation. The tracks are moved into the output basket.
  TrackVec_t &tracks = input.Tracks();
  double lmax;
  const double eps = 1.E-2; //

  int ntracks = tracks.size();
  double *steps = td->GetDblArray(ntracks);
  for (int itr = 0; itr < ntracks; itr++) {
    // Can this loop be vectorized?
    GeantTrack &track = *tracks[itr];
    lmax = SafeLength(track, td, eps);
    lmax = vecCore::math::Max<double>(lmax, track.fSafety);
    // Select step to propagate as the minimum among the "safe" step and:
    // the straight distance to boundary (if fboundary=1) or the proposed  physics
    // step (fboundary=0)
    steps[itr] = (track.fBoundary) ?
                vecCore::math::Min<double>(lmax, vecCore::math::Max<double>(track.fSnext, 1.E-4))
              : vecCore::math::Min<double>(lmax, track.fPstep);
  }
  // Propagate the vector of tracks
  PropagateInVolume(input.Tracks(), steps, td);

  //Update number of partial steps propagated in field
  td->fNmag += ntracks;

  // Update time of flight and number of interaction lengths.
  // Check also if it makes sense to call the vector interfaces
#if (defined(VECTORIZED_GEOMERY) && defined(VECTORIZED_SAMELOC))
  int nvect = 0;
#endif
  for (auto track : tracks) {
//    track->fTime += track->TimeStep(track->fStep);
//    track->fNintLen -= track->fStep/track->fIntLen;
    if (track->fStatus == kPhysics) {
      // Update number of steps to physics and total number of steps
      td->fNphys++;
      td->fNsteps++;
      output.AddTrack(track);
      continue;
    }
#if (defined(VECTORIZED_GEOMERY) && defined(VECTORIZED_SAMELOC))
    if (track->fSafety < 1.E-10 || track->fSnext < 1.E-10)
      nvect++;
#else
    // Vector treatment was not requested, so proceed with scalar
    if (!IsSameLocation(*track, td)) {
      td->fNcross++;
      td->fNsteps++;
    } else {
      track->SetStage(kGeometryStepStage);
    }
    output.AddTrack(track);
    continue;
#endif
  }
  // If vectorized treatment was requested and the remaining population is
  // large enough, continue with vectorized treatment
#if (defined(VECTORIZED_GEOMERY) && defined(VECTORIZED_SAMELOC))
  constexpr int kMinVecSize = 8; // this should be retrieved from elsewhere
  if (nvect < kMinVecSize) {
    for (auto track : tracks) {
      if (track->fStatus == kPhysics) continue;
      if (!IsSameLocation(*track, td)) {
        td->fNcross++;
#ifdef USE_REAL_PHYSICS
//            track->SetStage(kAlongStepActionStage);
            track->SetStage(kPostStepActionStage);
#else
            track->SetStage(kContinuousProcStage);
#endif
      } else {
        track->SetStage(kGeometryStepStage);
      }
      output.AddTrack(track);
      continue;
    }
    return;
  }
  // This part deals with vectorized treatment
  // Copy data to SOA and dispatch for vector mode
  GeantTrackGeo_v &track_geo = *td.fGeoTrack;
  for (auto track : tracks) {
    if (track.fStatus != kPhysics &&
        (track.fSafety < 1.E-10 || track.fSnext < 1.E-10))
      track_geo.AddTrack(*track);
  }
  bool *same = td->GetBoolArray(nvect);
  NavigationState *tmpstate = td->GetPath();
  VectorNavInterface::NavIsSameLocation(nvect,
                   track_geo.fXposV, track_geo.fYposV, track_geo.fZposV,
                   track_geo.fXdirV, track_geo.fYdirV, track_geo.fZdirV,
                   (const VolumePath_t**)fPathV, fNextpathV, same, tmpstate);
  track_geo.UpdateOriginalTracks();
  for (itr = 0; itr < nsel; itr++) {
    GeantTrack *track = track_geo.fOriginalV[itr];
    if (!same[itr]) {
      td->fNcross++;
      td->fNsteps++;
      track->fBoundary = true;
      track->fStatus = kBoundary;
      if (track->fNextpath->IsOutside())
        track->fStatus = kExitingSetup;
      if (track->fStep < 1.E-8) td->fNsmall++;
    } else {
      track->fBoundary = false;
      track->SetStage(kGeometryStepStage);
    }
    output.AddTrack(track);
  }
#endif
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void FieldPropagationHandler::PropagateInVolume(GeantTrack &track, double crtstep, GeantTaskData * td)
{
// Single track propagation in a volume. The method is to be called
// only with  charged tracks in magnetic field.The method decreases the fPstepV
// fSafetyV and fSnextV with the propagated values while increasing the fStepV.
// The status and boundary flags are set according to which gets hit first:
// - physics step (bdr=0)
// - safety step (bdr=0)
// - snext step (bdr=1)
   using ThreeVector = vecgeom::Vector3D<double>;  
   bool useRungeKutta;
   useRungeKutta = td->fPropagator->fConfig->fUseRungeKutta;
   // double bmag = td->fPropagator->fConfig->fBmag;
   double bmag;
   ThreeVector BfieldInitial;      // double BfieldInitial[3]
   ThreeVector Position(track.fXpos, track.fYpos, track.fZpos);
   FieldLookup::GetFieldValue(Position, BfieldInitial, bmag, td);

#ifndef VECCORE_CUDA_DEVICE_COMPILATION
   auto fieldPropagator = GetFieldPropagator(td);   
#endif

  // Reset relevant variables
  track.fStatus = kInFlight;
  track.fPstep -= crtstep;
  if (track.fPstep < 1.E-10) {
    track.fPstep = 0;
    track.fStatus = kPhysics;
  }
  // track.fSafety -= crtstep;     // Overestimate --> moved below for better estimate
  // if (track.fSafety < 1.E-10)
  //   track.fSafety = 0;
  track.fSnext -= crtstep;
  if (track.fSnext < 1.E-10) {
    track.fSnext = 0;
    if (track.fBoundary) {
      track.fStatus = kBoundary;
    }
  }
  track.fStep += crtstep;
#ifdef USE_VECGEOM_NAVIGATOR
//  CheckLocationPathConsistency(i);
#endif
  double curvaturePlus= fabs(GeantTrack::kB2C * track.fCharge * bmag) / (track.fP + 1.0e-30);  // norm for step
  
  constexpr double numRadiansMax= 10.0; // Too large an angle - many RK steps.  Potential change -> 2.0*PI;
  constexpr double numRadiansMin= 0.05; // Very small an angle - helix is adequate.  TBC: Use average B-field value?
      //  A track turning more than 10 radians will be treated approximately
  const double angle= crtstep * curvaturePlus;
  bool mediumAngle = ( numRadiansMin < angle ) && ( angle < numRadiansMax );
  useRungeKutta = useRungeKutta && (mediumAngle);

  bool dominantBz =  std::fabs( std::fabs(BfieldInitial[2]) )
     > 1.e3 * std::max( std::fabs( BfieldInitial[0]), std::fabs(BfieldInitial[1]) );

#ifdef DEBUG_FIELD
  printf("--PropagateInVolume(Single): \n");
  printf("Curvature= %8.4g   CurvPlus= %8.4g  step= %f   Bmag=%8.4g   momentum mag=%f  angle= %g\n"
         Curvature(td, i), curvaturePlus, crtstep, bmag, track.fP, angle );
#endif

  ThreeVector Direction(track.fXdir, track.fYdir, track.fZdir);
  ThreeVector PositionNew(0.,0.,0.);
  ThreeVector DirectionNew(0.,0.,0.);

#ifndef VECCORE_CUDA
  if( useRungeKutta ) {
     fieldPropagator->DoStep(Position,    Direction,    track.fCharge, track.fP, crtstep,
                             PositionNew, DirectionNew);
  }
  else
#endif
  {
     constexpr double toKiloGauss= 1.0e+14; // Converts to kilogauss -- i.e. 1 / Unit::kilogauss
                                            // Must agree with values in magneticfield/inc/Units.h
     double Bz = BfieldInitial[2] * toKiloGauss;
     if ( dominantBz ) {
        // Constant field in Z-direction
        ConstBzFieldHelixStepper stepper( Bz ); //
        stepper.DoStep<ThreeVector,double,int>(Position,    Direction,    track.fCharge, track.fP, crtstep,
                                               PositionNew, DirectionNew);
     } else {
        // Geant::
        double BfieldArr[3] = { BfieldInitial.x(), BfieldInitial.y(), BfieldInitial.z() };
        ConstFieldHelixStepper stepper( BfieldArr );
        stepper.DoStep<ThreeVector,double,int>(Position,    Direction,  track.fCharge, track.fP, crtstep,                                           
                                               PositionNew, DirectionNew);        
     }
  }

  //  may normalize direction here  // vecCore::math::Normalize(dirnew);
  ThreeVector DirectionUnit = DirectionNew.Unit();
  double posShift = (PositionNew - Position).Mag();  
  
  track.fXpos = PositionNew.x();
  track.fYpos = PositionNew.y();
  track.fZpos = PositionNew.z();

  track.fXdir = DirectionUnit.x();
  track.fYdir = DirectionUnit.y();
  track.fZdir = DirectionUnit.z();

  track.fSafety -= posShift; //  Was crtstep;
  if (track.fSafety < 1.E-10)
    track.fSafety = 0;
  
#ifdef REPORT_AND_CHECK
  double origMag= Direction.Mag();
  double oldMag= DirectionNew.Mag();
  double newMag= DirectionUnit.Mag();  
  Printf(" -- State after propagation in field:  Position= %f, %f, %f   Direction= %f, %f, %f  - mag original, integrated, integr-1.0, normed-1 = %10.8f %10.8f %7.2g %10.8f %7.2g",
         track.fXpos,  track.fYpos,  track.fZpos, track.fXdir,  track.fYdir,  track.fZdir,
         origMag, oldMag, oldMag-1.0, newMag, newMag-1.0 );

  const char* Msg[4]= { "After propagation in field - type Unknown(ERROR) ",
                        "After propagation in field - with RK           ",
                        "After propagation in field - with Helix-Bz     ",
                        "After propagation in field - with Helix-General" };
  CheckTrack(i, Msg[propagationType] );
#endif
  
#if 0
  ThreeVector SimplePosition = Position + crtstep * Direction;
  // double diffpos2 = (PositionNew - Position).Mag2();
  double diffpos2 = (PositionNew - SimplePosition).Mag2();
  //   -- if (vecCore::math::Sqrt(diffpos)>0.01*crtstep) {
  const double drift= 0.01*crtstep;
  if ( diffpos2>drift*drift ){
      double diffpos= vecCore::math::Sqrt(diffpos2);
      // Geant::Print("PropagateInVolumeSingle","relative difference in pos = %g", diffpos/crtstep);
      Geant::Print("PropagateInVolumeSingle","difference in pos = %g (abs) %g (relative) , step= %g",
                   diffpos, diffpos/crtstep, crtstep);
  }
#endif
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
void FieldPropagationHandler::PropagateInVolume(TrackVec_t &tracks,
                                                const double *crtstep,
                                                GeantTaskData *td)
{
// THIS IS THE VECTORIZED IMPLEMENTATION PLACEHOLDER FOR MAGNETIC FIELD PROPAGATION.
// Now implemented just as a loop
  int nTracks = tracks.size();
#if 1 // VECTOR_FIELD_PROPAGATION
  using vecgeom::SOA3D;
  const int Npm= 6;
  
  // double yInput[8*nTracks], yOutput[8*nTracks];
  bool       succeeded[nTracks];
  int        intCharge[nTracks];
  
  // Choice 1.   SOA3D
  SOA3D<double> position3D(nTracks);   // To-Do: Move into TaskData: 
  SOA3D<double> direction3D(nTracks); // Alternative to momentum
  double        momentumMag[nTracks];

  // SOA3D<double> momentum3D(nTracks);   

  //    ==> CheckSize(position3D, nTracks);
  SOA3D<double> PositionOut(nTracks);   // To-Do: Move into TaskData: 
  SOA3D<double> DirectionOut(nTracks);
  
  // Choice 2.   SOA6D
  // SOA6D<double> PositMom6D( nTracks );

  for (int itr=0; itr<nTracks; ++itr)
  {
     GeantTrack* pTrack= tracks[itr];

     intCharge[itr]= pTrack->fCharge;
     momentumMag[itr] = pTrack->fP;

     // PositMom6D.push_back( pTrack->fXpos, pTrack->fYpos, pTrack->fZpos, px, py, pz );
     position3D.push_back( pTrack->fXpos, pTrack->fYpos, pTrack->fZpos );
     direction3D.push_back( pTrack->fXdir, pTrack->fYdir, pTrack->fZdir );
  }

  // Trial Helix step -- now assumes a constant uniform field
  if( td->fBfieldIsConst ) {
     vecgeom::Vector3D<double> BfieldUniform= td->fConstFieldValue;
     ConstFieldHelixStepper stepper( BfieldUniform );
     // stepper.DoStep<ThreeVector,double,int>(Position,    Direction,  track.fCharge, track.fP, crtstep,
     //                                        PositionNew, DirectionNew);
     
     stepper.DoStepArr</*Geant::*/Double_v>( position3D.x(),  position3D.y(),  position3D.z(),
                                         direction3D.x(), direction3D.y(), direction3D.z(),
                                         intCharge,
                                         momentumMag,
                                         crtstep,
                                         PositionOut.x(),  PositionOut.y(),  PositionOut.z(),
                                         DirectionOut.x(), DirectionOut.y(), DirectionOut.z(),
                                         nTracks
        );
  }
  else
  {
     // Prepare for Runge Kutta stepping
     
     // Choice 3.   Array of FieldTrack 
     FieldTrack fldTracksIn[nTracks], fldTracksOut[nTracks];
     
     for (int itr=0; itr<nTracks; ++itr)
     {
        GeantTrack* pTrack= tracks[itr];        
        // Alternative - directly momentum vector
        double pmag= pTrack->fP, px=pTrack->fXdir, py=pTrack->fYdir, pz=pTrack->fZdir;
        px *= pmag;
        py *= pmag;
        pz *= pmag;
        // Momentum3D.push( px, py, pz );
        
        // Choice 3. --- Load
        // yInput[itr].LoadFromTrack(*tracks[itr]);
        double trackVals[Npm] = { pTrack->fXpos, pTrack->fYpos, pTrack->fZpos, px, py, pz };
        fldTracksIn[itr].LoadFromArray( trackVals, Npm );
     }

     auto fieldPropagator = GetFieldPropagator(td);
     auto vectorDriver = fieldPropagator ? fieldPropagator->GetFlexibleIntegrationDriver() : nullptr;

     if( vectorDriver ) {
        // Integrate using Runge Kutta method
        vectorDriver
           ->AccurateAdvance( fldTracksIn, hstep, charge, epsTol,
                              fldTracksOut, nTracks, succeeded );
     } else {
        Geant::Error("FieldPropagationHandler: no Flexible/Vector Integration Driver found.");
     }
  }
#else   

  for (int itr=0; itr<nTracks; ++itr)
    PropagateInVolume(*tracks[itr], crtstep[itr], td);
#endif  
}

//______________________________________________________________________________
VECCORE_ATT_HOST_DEVICE
bool FieldPropagationHandler::IsSameLocation(GeantTrack &track, GeantTaskData *td) {
// Query geometry if the location has changed for a track
// Returns number of tracks crossing the boundary (0 or 1)

  if (track.fSafety > 1.E-10 && track.fSnext > 1.E-10) {
    // Track stays in the same volume
    track.fBoundary = false;
    return true;
  }

  // Track may have crossed, check it
  bool same;
#ifdef USE_VECGEOM_NAVIGATOR
  vecgeom::NavigationState *tmpstate = td->GetPath();
  ScalarNavInterfaceVGM::NavIsSameLocation(track, same, tmpstate);
#else
// ROOT navigation
  ScalarNavInterfaceTGeo::NavIsSameLocation(track, same);
#endif // USE_VECGEOM_NAVIGATOR
  if (same) {
    track.fBoundary = false;
    return true;
  }

  track.fBoundary = true;
  track.fStatus = kBoundary;
  if (track.NextPath()->IsOutside())
    track.fStatus = kExitingSetup;
  if (track.fStep < 1.E-8) td->fNsmall++;
  return false;
}

} // GEANT_IMPL_NAMESPACE
} // Geant
