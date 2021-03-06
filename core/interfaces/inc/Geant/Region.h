

#ifndef REGION_H
#define REGION_H

// for inline namespace VECGEOM_IMPL_NAMESPACE
#include "base/TypeMap.h"

#include <vector>
#include <string>

namespace vecgeom {
inline namespace VECGEOM_IMPL_NAMESPACE {
/**
 * @brief   Class for describin regions.
 *
 * Implementation of vecgeom::Region.
 *
 * @class   Region.
 * @author  M Novak, A Ribon
 * @date    june 2016
 */
class Region {
public:
  Region(const std::string &name, bool iscutinlength = true, double gcut = -1., double emcut = -1, double epcut = -1,
         double pcut = -1);

  ~Region() {}

  // get number of regions created so far
  static int GetNumberOfRegions() { return gTheRegionTable.size(); }

  int GetIndex() const { return fIndex; }
  const std::string &GetName() const { return fName; }

  bool IsProductionCutInLength() const { return fIsProductionCutInLength; }
  double GetGammaCut() const { return fGammaCutValue; }
  double GetElectronCut() const { return fElectronCutValue; }
  double GetPositronCut() const { return fPositronCutValue; }
  double GetProtonCut() const { return fProtonCutValue; }

  // get the global Region table
  static const std::vector<Region *> &GetTheRegionTable() { return gTheRegionTable; }

private:
  std::string fName;
  int fIndex; // index of this region
  bool fIsProductionCutInLength;
  double fGammaCutValue;
  double fElectronCutValue;
  double fPositronCutValue;
  double fProtonCutValue;

  // the global Region table
  static std::vector<Region *> gTheRegionTable;
};

} // namespace vecgeom
} // namespace VECGEOM_IMPL_NAMESPACE

#endif // REGION_H
