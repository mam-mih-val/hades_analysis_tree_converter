//
// Created by mikhail on 5/23/20.
//

#ifndef HTREE_TO_AT_SRC_DETECTOR_MANAGER_H_
#define HTREE_TO_AT_SRC_DETECTOR_MANAGER_H_

#include "AnalysisTree/Detector.h"
#include "AnalysisTree/EventHeader.h"
#include "AnalysisTree/Track.h"
#include "AnalysisTree/DataHeader.h"
#include "AnalysisTree/Configuration.h"
#include <TTree.h>

namespace Analysis {
class DetectorManager {
public:
  virtual void MakeBranch(AnalysisTree::Configuration &config, TTree* tree){}
  virtual void SetField(const int& value, int idx){}
  virtual void SetField(const float& value, int idx){}
  virtual void SetField(const bool& value, int idx){}

protected:
  DetectorManager() = default;
  ~DetectorManager() = default;

  std::map<int, int> fields_int_;
  std::map<int, int> fields_float_;
  std::map<int, int> fields_bool_;
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_DETECTOR_MANAGER_H_
