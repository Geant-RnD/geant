//===--- NumaBlockMgr.h - Geant-V ---------------------------------*- C++ -*-===//
//
//                     Geant-V Prototype
//
//===----------------------------------------------------------------------===//
/**
 * @file NumaBlockMgr.h
 * @brief Memory manager for NUMA-aware data blocks
 * @author Andrei Gheata
 */
//===----------------------------------------------------------------------===//
#ifndef GEANT_NUMA_BLOCK_ALLOCATOR
#define GEANT_NUMA_BLOCK_ALLOCATOR

#include <atomic>
#include <vector>
#include <cassert>
#include <iostream>
#include <type_traits>
#include "Geant/Config.h"
#include "mpmc_bounded_queue.h"
#include "NumaBlock.h"

/**
 * @brief Class NumaBlockMgr
 * @detailed The class is managing allocation of blocks of POD data for a given 
 *           NUMA node. The allocation for single blocks is using numa_aligned_malloc.
 */
namespace Geant {
inline namespace GEANT_IMPL_NAMESPACE {

template <typename T, bool D=false> class NumaBlockMgr {
  using size_t = std::size_t;
  using numa_block_ptr = NumaBlock<T,D>*;
  using queue_t = mpmc_bounded_queue<numa_block_ptr>;

  static size_t const cacheline_size = 64;
  static size_t const queue_buff_size = 4096;
  typedef char cacheline_pad_t[cacheline_size];

private:
  std::atomic<numa_block_ptr> fCurrent; // Current block being distributed

  cacheline_pad_t pad0_;   //! Padding to protect the other data from the hot cache line above
  
  int             fNode;  // Numa node id
  int             fMaxdepth; // Max depth used as extra parameter in allocations
  std::atomic_int fNblocks; // Number of blocks in flight
  size_t          fBlockSize;  // Numa block size
  queue_t         fBlocks;  // Queue of free blocks

public:

  /** @brief Constructor providing number of blocks to be initially created */
  NumaBlockMgr(size_t nblocks, int numa_node, int maxdepth, size_t bsize) 
    : fCurrent(nullptr), fNode(numa_node), fMaxdepth(maxdepth), fNblocks(nblocks), fBlockSize(bsize),
      fBlocks(queue_buff_size)
  {
    // Constructor creating nblocks
    numa_block_ptr block;
    for (size_t i=0; i<nblocks; ++i) {
      if (D)
        block = NumaBlock<T,D>::MakeInstance(fBlockSize, fNode, fMaxdepth);
      else
        block = NumaBlock<T,D>::MakeInstance(fBlockSize, fNode);
      if (i == 0)
        fCurrent.store(block);
      else
        fBlocks.enqueue(block);
    }
  }
  
  /** @brief Add a free block */
  numa_block_ptr AddBlock()
  {
    fNblocks++;
    if (D) return ( NumaBlock<T,D>::MakeInstance(fBlockSize, fNode, fMaxdepth) );
    return ( NumaBlock<T,D>::MakeInstance(fBlockSize, fNode) );
  }

  /** @brief Get number of queued blocks */
  int GetNqueued() const { return fBlocks.size(); }

  /** @brief Get number of blocks in flight */
  int GetNblocks() const { return fNblocks.load(); }

  /** @brief Destructor*/
  ~NumaBlockMgr() {
    numa_block_ptr block;
//    std::cout << "deleting block manager: " << this << std::endl;
    while (fBlocks.dequeue(block)) {
      if (block == fCurrent.load()) continue;
      NumaBlock<T,D>::ReleaseInstance(block);
    }
//    std::cout << "  delete fCurrent\n";
    NumaBlock<T,D>::ReleaseInstance(fCurrent.load());
  }

  /** @brief Get a free block */
  numa_block_ptr GetBlock()
  {
    numa_block_ptr block;
    if (!fBlocks.dequeue(block))
      block = AddBlock();
    return block;
  }

  
  /** @brief Get a free object from the pool 
      @return A valid object reference */
  T &GetObject(size_t &index) {
    // Get the next object from the current active block
    numa_block_ptr block = CurrentBlock();
    // Hold the object
    T* obj = block->GetObject(index);
    // If the block is not yet fully distributed, return
    if (obj && !block->IsDistributed()) return (*obj);
    // Replace distributed block
    numa_block_ptr next_free = GetBlock();
//    std::cout << "current= " << block << " distributed, next_free= " << next_free << std::endl;
    while (!fCurrent.compare_exchange_weak(block, next_free, std::memory_order_relaxed)) {
      // Retry if block is the same
      if (CurrentBlock() == block) continue;
      // Someone else replaced the block, recycle our free block
      assert(fBlocks.enqueue(next_free));
      break;
    }
    // Return the held object if valid, or try again with the new block
//    std::cout << "   current is now: " << CurrentBlock() << std::endl;
    if (obj) return (*obj);

    block = CurrentBlock();
    // Blocks are large, unlikely to be emptied righ away, but you never know...
    obj = block->GetObject(index);
    if (!obj) obj = &GetObject(index);
    // There is no link anymore to the replaced block but once released by all users
    // it will go back in the block list
    return (*obj);
  }

  /** @brief Recycle an object from a block 
      @param block Block from which the object is released
      @return Block may have been recycled */
  GEANT_FORCE_INLINE bool ReleaseObject(numa_block_ptr block) {
    if (block->ReleaseObject()) {
//      std::cout << "Recycling block " << block << std::endl;
      block->Clear();
      fBlocks.enqueue(block);
      return true;
    }
    return false;
  }
  
  numa_block_ptr CurrentBlock() const { return ( fCurrent.load() ); }
  
};  
} // GEANT_IMPL_NAMESPACE
} // Geant

#endif
