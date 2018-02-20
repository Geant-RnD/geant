//===--- LinkDef.h - Geant-V ------------------------------------*- C++ -*-===//
//
//                     Geant-V Prototype
//
//===----------------------------------------------------------------------===//
/**
 * @file LinkDef.h
 * @brief Linkage for classes
 * @author Andrei Gheata
 */
//===----------------------------------------------------------------------===//

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class Geant::cxx::GeantTaskData+;
#pragma link C++ class Basket+;
#pragma link C++ class GeantPropagator+;
#pragma link C++ class PhysicsProcessOld+;
#pragma link C++ class StdApplication+;
#pragma link C++ class MyHit+;
#pragma link C++ class GeantBlock<MyHit>+;
#pragma link C++ class Geant::TThreadMergingFile+;
#pragma link C++ class Geant::ThreadFileMerger+;
#pragma link C++ class MCTruthMgr+;
#pragma link C++ class Geant::GeantConfig+;

#endif
