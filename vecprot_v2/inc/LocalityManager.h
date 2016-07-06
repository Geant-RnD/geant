//===--- LocalityManager.h - Geant-V ---------------------------------*- C++ -*-===//
//
//                     Geant-V Prototype
//
//===----------------------------------------------------------------------===//
/**
 * @file LocalityManager.h
 * @brief Manager class for handling locality.
 * @author Andrei Gheata
 */
//===----------------------------------------------------------------------===//
#ifndef GEANT_LOCALITY_MANAGER
#define GEANT_LOCALITY_MANAGER

#include "Geant/Config.h"
#include "TrackManager.h"
#include "GeantTrack.h"

/**
 * @brief Class TrackManager
 * @detailed The class is managing allocation of blocks of tracks for a given NUMA
 *           node. The allocation for single blocks is using numa_aligned_malloc.
 */
namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {
/**
 * @brief Class TrackManager
 * @detailed The track manager handles allocation and cleaning of track blocks.
 */
class LocalityManager {
  using size_t = std::size_t;
  
private:
  static LocalityManager *fgInstance; /** Singleton instance */
  bool fInitialized;               /** Flag for initialization */
  NumaPolicy fPolicy;              /** Numa policy to be used */
  int fNnodes;                     /** Number of nodes */
  TrackManager **fTrackMgr;        /** [fNnodes] Array of track managers */
  size_t fNblocks;                 /** Number of initial track blocks */
  size_t fBlockSize;               /** Number of tracks stored by each block */

private:
  /** @brief Constructor */
  LocalityManager();    
public:
 /** @brief Function that creates locality manager instance **/
  static
  LocalityManager* Instance();

  /** @brief Destructor */
  ~LocalityManager();
  
  /** @brief Setter for the number of locality nodes.*/
  GEANT_INLINE
  void SetNnodes(int nnodes) { fNnodes = nnodes; }

  /** @brief Getter for the number of locality nodes.*/
  GEANT_INLINE
  int GetNnodes() const { return fNnodes; }

  /** @brief Setter for the locality policy.*/
  GEANT_INLINE
  void SetPolicy(NumaPolicy::EPolicyType policy) { fPolicy.SetPolicy(policy); }

  /** @brief Getter for the locality policy.*/
  GEANT_INLINE
  NumaPolicy const &GetPolicy() const { return fPolicy; }
  
  /** @brief Setter for the number of blocks to allocate.*/
  GEANT_INLINE
  void SetNblocks(size_t nblocks) { fNblocks = nblocks; }

  /** @brief Getter for the number of blocks to allocate.*/
  GEANT_INLINE
  int GetNblocks() const { return fNblocks; }
  
  /** @brief Setter for the block size.*/
  GEANT_INLINE
  void SetBlockSize(size_t size) { fBlockSize = size; }

  /** @brief Getter for the block size.*/
  GEANT_INLINE
  int GetBlockSize() const { return fBlockSize; }
  
  /** @brief Initialize locality manager and allocate data.*/
  void Init();
  
  /** @brief Getter for the initialization flag.*/
  GEANT_INLINE
  bool IsInitialized() const { return fInitialized; }

  /** @brief Getter for track managers per locality node.*/
  GEANT_INLINE
  TrackManager &GetTrackManager(int inode) const { return *fTrackMgr[inode]; }
      
  /** @brief Service to recycle tracks */
  GEANT_INLINE
  bool ReleaseTrack(GeantTrack const &track) {
    int node = TrackManager::GetNode(track);
    return ( fTrackMgr[node]->ReleaseTrack(track) );
  }

};
} // GEANT_IMPL_NAMESPACE
} // Geant

#endif
