
#ifndef PHYSICSDATA_H
#define PHYSICSDATA_H

#include <vector>

namespace geantphysics {

class LightTrack;

class PhysicsData {
public:
  PhysicsData();
  ~PhysicsData() {}

  /** @brief: Insert secondary and return handle to it **/
  LightTrack &InsertSecondary()
  {
    ResizeIfSmall();
    return fListOfSecondaries[fNumUsedSecondaries++];
  }

  /** @brief: Clear list of used secondaries **/
  void ClearSecondaries() { fNumUsedSecondaries = 0; }

  /** @brief: Array of secondaries, Use GetNumOfSecondaries() to get its size **/
  LightTrack *GetListOfSecondaries() { return fListOfSecondaries.data(); }

  /** @brief: Number of used elements in array returned by GetListOfSecondaries **/
  int GetNumOfSecondaries() const { return fNumUsedSecondaries; }

  std::vector<LightTrack> &GetPrimaryTracks() { return fPrimaryTracks; }

  std::vector<int> &GetSecondaryFillVector() { return fSecondaryFillVector; }

  static void ClearAll();
  static std::vector<PhysicsData *> gThePhysicsDataTable;

private:
  void ResizeIfSmall()
  {
    if ((int)fListOfSecondaries.size() == fNumUsedSecondaries) fListOfSecondaries.emplace_back();
  }
  int fNumUsedSecondaries; // number of secondary tracks currently used from fListOfSecondaries
  std::vector<LightTrack> fListOfSecondaries;
  std::vector<LightTrack> fPrimaryTracks;
  std::vector<int> fSecondaryFillVector;
};

} // namespace geantphysics

#endif
