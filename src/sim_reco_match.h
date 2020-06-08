//
// Created by mikhail on 6/8/20.
//

#ifndef HTREE_TO_AT_SRC_SIM_RECO_MATCH_H_
#define HTREE_TO_AT_SRC_SIM_RECO_MATCH_H_
#include "detector_manager.h"

namespace Analysis {
class SimRecoMatch : public DetectorManager {
public:
  static SimRecoMatch* Instance(){
    if(!instance_)
      instance_ = new SimRecoMatch;
    return instance_;
  }
  void MakeBranch( AnalysisTree::Configuration &config, TTree* tree ) override {
    matching_ = new AnalysisTree::Matching(
        config.GetBranchConfig("sim_tracks").GetId(),
        config.GetBranchConfig("mdc_vtx_tracks").GetId());
    config.AddMatch("sim_tracks", "mdc_vtx_tracks","sim_reco_tracks");
    tree->Branch("sim_reco_tracks", "AnalysisTree::Matching", &matching_);
  }
  void ClearMatching(){
    matching_->Clear();
  }
  void Match( int track_idx, int hit_idx ){
    matching_->AddMatch(track_idx, hit_idx);
  }
private:
  static SimRecoMatch* instance_;
  SimRecoMatch() = default;
  ~SimRecoMatch() = default;
  void SetField(const int&, int) override {};
  void SetField(const float&, int) override {};
  void SetField(const bool&, int) override {};

  AnalysisTree::Matching* matching_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_SIM_RECO_MATCH_H_