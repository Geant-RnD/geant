#ifndef Workspace_For_Field_Propagation_h
#define Workspace_For_Field_Propagation_h

#include "base/SOA3D.h"

namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {

struct WorkspaceForFieldPropagation
{
  public:
    WorkspaceForFieldPropagation(size_t bufferSize);
    ~WorkspaceForFieldPropagation();

    size_t capacity(){ return fPositionInp->capacity(); } 
      
    /** @brief  Throw away old content */
    void Clear();
    
    /** @brief  Enlarge the buffers, throwing away old content */
    void ClearAndResize( size_t numEntries );

    // Invariant: 
    //    All SOA3D containers must have the same size.
    //
    // Consequence:
    //  -> the size of fPositionInpForFieldProp with represent others
  public:
    vecgeom::SOA3D<double> * fPositionInp= nullptr;
    vecgeom::SOA3D<double> * fDirectionInp= nullptr;
    vecgeom::SOA3D<double> * fPositionOutp= nullptr;
    vecgeom::SOA3D<double> * fDirectionOutp= nullptr;   
};

inline
WorkspaceForFieldPropagation::WorkspaceForFieldPropagation(size_t bufferSize)
{
   // using SoADd = vecgeom::SOA3D<type>;
   
   fPositionInp=   new vecgeom::SOA3D<double>(bufferSize);
   fDirectionInp=  new vecgeom::SOA3D<double>(bufferSize);
   fPositionOutp=  new vecgeom::SOA3D<double>(bufferSize);
   fDirectionOutp= new vecgeom::SOA3D<double>(bufferSize);   
}

inline
WorkspaceForFieldPropagation::~WorkspaceForFieldPropagation()
{
   delete fPositionInp;
   delete fDirectionInp;
   delete fPositionOutp;
   delete fDirectionOutp;
}

inline
void WorkspaceForFieldPropagation::Clear()
{
   fPositionInp->clear();
   fPositionOutp->clear();
   fDirectionInp->clear();
   fDirectionOutp->clear();
}

inline
void WorkspaceForFieldPropagation::ClearAndResize( size_t numEntries )
{
   size_t currentCap = fPositionInp->capacity();
   size_t nextCap = std::min( currentCap + (currentCap >> 1), ( (numEntries+1) / 16 + 1) * 16 );

   Clear();

   fPositionInp->reserve(nextCap);
   fPositionOutp->reserve(nextCap);
   fDirectionInp->reserve(nextCap);
   fDirectionOutp->reserve(nextCap);
}

} // GEANT_IMPL_NAMESPACE
} // Geant
#endif
