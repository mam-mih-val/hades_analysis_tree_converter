//
// Created by mikhail on 5/27/20.
//

#ifndef HTREE_TO_AT_SRC_MDC_META_MATCHING_H_
#define HTREE_TO_AT_SRC_MDC_META_MATCHING_H_

#include "detector_manager.h"
#include <AnalysisTree/Matching.hpp>

namespace Analysis {
class MdcMetaMatching : public DetectorManager {
public:
  static MdcMetaMatching * Instance(){
    if(!instance_)
      instance_ = new MdcMetaMatching;
    return instance_;
  }
  void MakeBranch( AnalysisTree::Configuration &config, TTree* tree ) override {
    matching_ = new AnalysisTree::Matching( config.GetBranchConfig("mdc_vtx_tracks").GetId(),
                                           config.GetBranchConfig("meta_hits").GetId() );
    config.AddMatch(matching_);
    tree->Branch("mdc_vtx_tracks2meta_hits", "AnalysisTree::Matching", &matching_);
  }
  void ClearMatching(){
    matching_->Clear();
  }
  void Match( int track_idx, int hit_idx ){
    matching_->AddMatch(track_idx, hit_idx);
  }

private:
  static MdcMetaMatching * instance_;
  MdcMetaMatching() = default;
  ~MdcMetaMatching() = default;
  void SetField(const int&, int) override {};
  void SetField(const float&, int) override {};
  void SetField(const bool&, int) override {};

  AnalysisTree::Matching* matching_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_MDC_META_MATCHING_H_
