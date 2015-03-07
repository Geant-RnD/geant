#ifndef GUBenchmarker_H
#define GUBenchmarker_H 1

#include "base/Global.h"

namespace vecphys {

class GUTrackHandler;

class GUBenchmarker {

public:

  GUBenchmarker();
  ~GUBenchmarker();

  int RunBenchmark();

  void SetNTracks(const int ntracks) { fNtracks = ntracks; }
  void SetRepetitions(const unsigned repetitions) { 
    fRepetitions = repetitions; 
  }

private:
    
  int  RunBenchmarkInteract();
  void RunGeant4();
  void RunScalar();
  void RunVector();

#ifdef VECPHYS_CUDA
  void RunCuda();
#endif

private:
  GUTrackHandler *fTrackHandler;

  int fNtracks;
  unsigned fRepetitions;
  int fVerbosity;

};

} // end namespace vecphys

#endif
