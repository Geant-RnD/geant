#ifndef JOINING_PROXY_VEC_MRG32k3A_H
#define JOINING_PROXY_VEC_MRG32k3A_H

#include <VecMath/Rng/RngDefs.h>
#include <VecMath/Rng/MRG32k3a.h>

namespace geant {
inline namespace GEANT_IMPL_NAMESPACE {

using BaseVecRNGType=    vecRng::MRG32k3a<vecRng::VectorBackend>; 
using BaseScalarRNGType= vecRng::MRG32k3a<vecRng::ScalarBackend>;

using BaseRNGState_scalar= typename vecRng::RNG_traits<BaseScalarRNGType> /*<ScalarBackend>>*/ ::State_t;
using BaseRNGState_vector= typename vecRng::RNG_traits<BaseVecRNGType> /*<VectorBackend>>*/ ::State_t;

using BaseRNGStream_t= size_t;

template <typename BackendT>
class JoiningProxyVecMRG32k3a : public vecRng::MRG32k3a<BackendT>
{
public:
   using State_t= BaseRNGState_vector;

   /** @brief Constructor: 'create' empty instance - not associated with set of scalar states */
   JoiningProxyVecMRG32k3a();
   
   /** @brief Constructor: 'create' vector instance from a set of scalar states */
   //   JoiningProxyVecMRG32k3a( BaseScalarRNGType* trackPrng[], int numStates );

   /** @brief Split apart this class - ensure that the state of its parts is fed back */
   //          - i.e. the state gets copied back to each 'track' scalar RNG state
   ~JoiningProxyVecMRG32k3a();

   /** @brief Create 'vector' state by copying the array of per-track states */
   //   void Join( BaseScalarRNGType* trackRngState[], int numStates );
   void Join( BaseRNGState_scalar** trackRngState, int start );

   /** @brief De-couple 'vector' state - copy back the per-track states into original arrays */
   //   void Split();
   void Split( BaseRNGState_scalar** trackRngState, int start );

   VECCORE_ATT_HOST_DEVICE   
   typename BackendT::Double_v Uniform();

   void PrintComponents() const;

   //@@@syjun---for temporary tests
   VECCORE_ATT_HOST_DEVICE  
   BaseRNGState_scalar* GenerateState(size_t streamId); 

   VECCORE_ATT_HOST_DEVICE  
   int GetSizeOfVector() { return vecSize; }

   VECCORE_ATT_HOST_DEVICE  
   size_t GetStreamIndex(BaseRNGState_scalar* state) { return fScalarPrng->template UniformIndex<vecRng::ScalarBackend>(state); }
   
protected:
   static typename BackendT::Double_v  dbVec;
   static constexpr int vecSize= vecRng::VectorSize<decltype(dbVec)>();
   
private:
   bool fJoinState= false;
   BaseScalarRNGType*  fScalarPrng;
};

// Implementation
template <typename BackendT>
BaseRNGState_scalar* 
JoiningProxyVecMRG32k3a<BackendT>::GenerateState(size_t streamId)
{
  fScalarPrng->Initialize(streamId);
  return fScalarPrng->GetState();
}

template <typename BackendT>
void
JoiningProxyVecMRG32k3a<BackendT>::Join(BaseRNGState_scalar** trackPrngState, int start)
{
   for( int i= 0; i < vecSize ; ++i ) {
     assert( trackPrngState[start+i] != nullptr );
     this->SetStateAt(i, trackPrngState[start+i]);
   }
   fJoinState= true;
}

//  ---------------------------================---------------------------
template <typename BackendT>
void 
JoiningProxyVecMRG32k3a<BackendT>::Split(BaseRNGState_scalar** trackPrngState, int start)
{
   for( int i= 0; i < vecSize; ++i ) {
     assert( trackPrngState[start+i] != nullptr );
     *trackPrngState[start+i] = this->GetStateAt(i);
   }
   fJoinState= false;
}

//  ---------------------------================---------------------------

template <typename BackendT>
void JoiningProxyVecMRG32k3a<BackendT>::PrintComponents() const
{
   const char* typeNameStr = "JoiningProxyVecMRG32k3a< >";
   std::cout << std::endl;
   std::cout << "== Printing of JoiningProxVecMRG32k3a PrintComponents():  " << std::endl;
   std::cout << "===================================================================" << std::endl;
   std::cout << "Dump of " << typeNameStr << " < Backend with vecSize= " << vecSize
             << " >  called for object " << this << " State addr = " << this->fState
             << std::endl;
   std::cout << " JoinState = " << fJoinState << std::endl;
   std::cout << " Contents of scalar RNG addresses: " << std::endl;
   for( int i= 0; i < vecSize; ++i ) {
     //     std::cout << " Addr [ " << i << " ] : " << fScalarTrackPrng[i] << std::endl;
   }
   std::cout << "===================================================================" << std::endl;   
}

//  ---------------------------================---------------------------
template <typename BackendT>
   JoiningProxyVecMRG32k3a<BackendT>::JoiningProxyVecMRG32k3a()
{
   fJoinState= false; 
   this->Initialize();
   
   //helper to generate the stream index
   fScalarPrng= new BaseScalarRNGType;
   fScalarPrng->Initialize();
}

//  ---------------------------================---------------------------
template <typename BackendT>
JoiningProxyVecMRG32k3a<BackendT>::~JoiningProxyVecMRG32k3a()
{
   delete fScalarPrng;
}

//  ---------------------------================---------------------------
VECCORE_ATT_HOST_DEVICE
template <typename BackendT>
  typename BackendT::Double_v JoiningProxyVecMRG32k3a<BackendT>::Uniform()
{
   return this->vecRng::MRG32k3a<BackendT>::template Uniform<BackendT>();
}

}
}
#endif
