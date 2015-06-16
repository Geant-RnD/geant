#include "GUBenchmarker.h"
#include "GUBenchmarker_gpu.h"
#include "GUPhysicsModelName.h"

#include "base/Stopwatch.h"
#include "backend/cuda/Backend.h"

#include "GUAliasSampler.h"
#include "GUComptonKleinNishina.h"
#include "GUSeltzerBerger.h"
#include "Physics2DVector.h"
#include "GUTrackHandler.h"

#ifdef VECPHYS_ROOT
#include "GUHistogram.h"
#endif

#include "GUCurand.h"

namespace vecphys {

void GUBenchmarker::RunCuda()
{
  cudaDeviceReset();

  //set 1024 megabytes on the heap (global mememory) 
  //cudaThreadSetLimit(cudaLimitMallocHeapSize, 1024*1024*1024);

#ifdef VECPHYS_ROOT
  GUHistogram* histogram = new GUHistogram("cuda.root", fMaxP);
#endif

  int *targetElements = new int [fNtracks];
  int *targetElements_d;

  //prepare table - To do: for each model
  GUSeltzerBerger *model = new GUSeltzerBerger(0,-1);

  GUAliasTableManager* tableM_h = model->GetSampler()->GetAliasTableManager();
  GUAliasTableManager* tableM_d;
  cudaMalloc((void**)&tableM_d, tableM_h->SizeOfManager());

  tableM_h->Relocate(tableM_d);
  
  //SeltzerBerger data
  Physics2DVector* sbData = model->GetSBData();
  Physics2DVector* sbData_d;
  cudaMalloc((void**)&sbData_d,maximumZ*sizeof(Physics2DVector));
  cudaMemcpy(sbData_d, sbData, maximumZ*sizeof(Physics2DVector),
             cudaMemcpyHostToDevice);

  GUTrack* otrack_aos = (GUTrack*) malloc(fNtracks*sizeof(GUTrack));

  //allocate memory for input/output tracks
  GUTrack *itrack_d;
  GUTrack *otrack_d;
  cudaMalloc((void**)&itrack_d, fNtracks*sizeof(GUTrack));
  cudaMalloc((void**)&otrack_d, fNtracks*sizeof(GUTrack));

  //set the default number of threads and thread blocks - should be setable
  int theNBlocks  =  26;
  int theNThreads = 192;

  //prepare random engines on the device
  Random_t* randomStates = 0;
  cudaMalloc(&randomStates, theNBlocks*theNThreads* sizeof(curandState));
  GUCurand_Init(randomStates, time(NULL), theNBlocks, theNThreads);

  Precision* incomingEn = new Precision[fNtracks];

  Precision elapsedTotal[kNumberPhysicsModel];
  Precision elapsedT[kNumberPhysicsModel];

  for(int k = 0 ; k < kNumberPhysicsModel ; ++k) elapsedTotal[k] = 0.; 

  for (unsigned r = 0; r < fRepetitions; ++r) {

    PrepareTargetElements(targetElements, fNtracks);

    //H2D
    cudaMalloc((void**)&targetElements_d, fNtracks*sizeof(int));
    cudaMemcpy(targetElements_d, targetElements, fNtracks*sizeof(int), 
               cudaMemcpyHostToDevice);

    fTrackHandler->GenerateRandomTracks(fNtracks,fMinP, fMaxP);

    bool first = true;
    for(unsigned int k = 0 ; k < kNumberPhysicsModel ; ++k) {

      GUTrack* itrack_aos = fTrackHandler->GetAoSTracks();
      cudaMemcpy(itrack_d, itrack_aos, fNtracks*sizeof(GUTrack), 
                   cudaMemcpyHostToDevice);

      if (first) {
        for(int i = 0 ; i < fNtracks ; ++i) {
          incomingEn[i] = itrack_aos[i].E;
        }
        first = false;
      }

      elapsedT[k] = 0.0;
      elapsedT[k] = CudaKernelFunc[k](theNBlocks, theNThreads, randomStates,
                                      tableM_d,sbData_d,fNtracks, itrack_d, 
                                      targetElements_d,otrack_d);
      elapsedTotal[k] += elapsedT[k];

      cudaMemcpy(itrack_aos, itrack_d, fNtracks*sizeof(GUTrack), 
                 cudaMemcpyDeviceToHost);
      cudaMemcpy(otrack_aos, otrack_d, fNtracks*sizeof(GUTrack), 
                 cudaMemcpyDeviceToHost);

#ifdef VECPHYS_ROOT
      histogram->RecordTime(k,elapsedT[k]);
      for(int i = 0 ; i < fNtracks ; ++i) {
        histogram->RecordHistos(k,incomingEn[k],
	  		        itrack_aos[i].E,
			        itrack_aos[i].pz/itrack_aos[i].E,
			        otrack_aos[i].E,
			        otrack_aos[i].pz/otrack_aos[i].E);
      } 
#endif    
    }
  }

  for(int k = 0 ; k < kNumberPhysicsModel ; ++k) {
    printf("%s  CUDA   Total time of %3d reps = %6.3f sec\n",
           GUPhysicsModelName[k], fRepetitions, elapsedTotal[k]);
  }

  cudaFree(randomStates);
  cudaFree(itrack_d);
  cudaFree(otrack_d);
  cudaFree(tableM_d);
  cudaFree(targetElements_d);

  free(tableM_h);
  free(targetElements);
  free(otrack_aos);

  //  delete model;
  delete[] incomingEn; 
#ifdef VECPHYS_ROOT
  delete histogram;
#endif
}

} // end of vecphys namespace

