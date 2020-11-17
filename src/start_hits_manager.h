//
// Created by mikhail on 11/17/20.
//

#ifndef HTREE_TO_AT_SRC_START_HITS_MANAGER_H_
#define HTREE_TO_AT_SRC_START_HITS_MANAGER_H_

#include "detector_manager.h"

namespace Analysis {
class StartHitsManager : public DetectorManager {
public:
  enum FIELDS_INT {
    MULTIPLICITY = 0,
    MODULE,
    STRIP,
  };
  static StartHitsManager *Instance() {
    if (!instance_)
      instance_ = new StartHitsManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree *tree) override;
  void NewHit(AnalysisTree::Configuration &config) {
    hit_ = hit_detector_->AddChannel();
    hit_->Init(config.GetBranchConfig(hit_detector_->GetId()));
  }
  int GetHitId() { return hit_->GetId(); }
  void ClearDetector() { hit_detector_->ClearChannels(); }
  void ReserveHits(int n_hits) { hit_detector_->Reserve(n_hits); }
  void SetField(const int &value, int idx) override {
    hit_->SetField(value, fields_int_.at(idx));
  }
  void SetField(const float &value, int idx) override {
    hit_->SetField(value, fields_float_.at(idx));
  }
  void SetField(const bool &value, int idx) override {
    hit_->SetField(value, fields_bool_.at(idx));
  }
  void SetTime(float value, int idx) { hit_->SetField(value, idx); }
  void SetWidth(float value, int idx) { hit_->SetField(value, 10 + idx); }

private:
  static StartHitsManager *instance_;
  StartHitsManager() : hit_detector_{nullptr}, hit_{nullptr} {};
  ~StartHitsManager() = default;
  AnalysisTree::HitDetector *hit_detector_;
  AnalysisTree::Hit *hit_;
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_START_HITS_MANAGER_H_
