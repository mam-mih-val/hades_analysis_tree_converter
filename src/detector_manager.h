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

namespace Analysis {
class DetectorManager {
public:
  template<typename T>
  virtual void FillDetectorFields(){}
  virtual void CleanDetector(){}
protected:
  DetectorManager() = default;
  ~DetectorManager() = default;

  std::map<int, int> fields_int_;
  std::map<int, int> fields_float_;
  std::map<int, int> fields_bool_;
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_DETECTOR_MANAGER_H_
