//
// Created by mikhail on 5/26/20.
//

#ifndef HTREE_TO_AT_SRC_HIT_MANAGER_H_
#define HTREE_TO_AT_SRC_HIT_MANAGER_H_

#include "detector_manager.h"

namespace Analysis {
class HitManager : public DetectorManager {
public:
  enum FIELDS_INT{
    CHARGE=0
  };
  enum FIELDS_FLOAT{
    MASS2=0,
    PATH_LENGTH,
    BETA,
    TIME_OF_FLIGHT,
    MATCH_QUALITY,
    MATCH_RADIUS,
    DE_DX
  };
  enum FIELDS_BOOL{
    IS_TOF_HIT,
    IS_RPC_HIT
  };
  static HitManager* Instance(){
    if (!instance_)
      instance_ = new HitManager;
    return instance_;
  }
  AnalysisTree::HitDetector* CreateHitDetector( AnalysisTree::Configuration &config ){
    AnalysisTree::BranchConfig hit_branch("meta_hits", AnalysisTree::DetType::kHit);
    hit_branch.AddField<float>("mass2");
    hit_branch.AddField<float>("path_length");
    hit_branch.AddField<float>("beta");
    hit_branch.AddField<float>("time_of_flight");
    hit_branch.AddField<float>("match_quality");
    hit_branch.AddField<float>("match_radius");
    hit_branch.AddField<float>("dEdx");

    hit_branch.AddField<int>("charge");

    hit_branch.AddField<bool>("is_tof_hit");
    hit_branch.AddField<bool>("is_rpc_hit");

    config.AddBranchConfig(hit_branch);
    hit_detector_ = new AnalysisTree::HitDetector( config.GetLastId() );

    fields_int_.insert( std::make_pair( CHARGE, hit_branch.GetFieldId("charge") ) );

    fields_float_.insert( std::make_pair( MASS2, hit_branch.GetFieldId("mass2") ) );
    fields_float_.insert( std::make_pair( PATH_LENGTH, hit_branch.GetFieldId("path_length") ) );
    fields_float_.insert( std::make_pair( BETA, hit_branch.GetFieldId("beta") ) );
    fields_float_.insert( std::make_pair( TIME_OF_FLIGHT, hit_branch.GetFieldId("time_of_flight") ) );
    fields_float_.insert( std::make_pair( MATCH_QUALITY, hit_branch.GetFieldId("match_quality") ) );
    fields_float_.insert( std::make_pair( MATCH_RADIUS, hit_branch.GetFieldId("match_radius") ) );
    fields_float_.insert( std::make_pair( DE_DX, hit_branch.GetFieldId("dEdx") ) );

    fields_bool_.insert( std::make_pair( IS_TOF_HIT, hit_branch.GetFieldId("is_tof_hit") ) );
    fields_bool_.insert( std::make_pair( IS_RPC_HIT, hit_branch.GetFieldId("is_rpc_hit") ) );

    return hit_detector_;
  }
  void NewHit(AnalysisTree::Configuration& config){
    hit_ = hit_detector_->AddChannel();
    hit_->Init(config.GetBranchConfig(hit_detector_->GetId() ) );
  }
  void ClearDetector(){ hit_detector_->ClearChannels(); }
  void SetField( const int& value, int idx ) override {
    hit_->SetField( value, fields_int_.at(idx) );
  }
  void SetField( const float& value, int idx ) override {
    hit_->SetField( value, fields_float_.at(idx) );
  }
  void SetField( const bool& value, int idx ) override {
    hit_->SetField( value, fields_bool_.at(idx) );
  }

private:
  static HitManager* instance_;
  HitManager() : hit_detector_{nullptr}, hit_{nullptr} {};
  ~HitManager() = default;
  AnalysisTree::HitDetector* hit_detector_;
  AnalysisTree::Hit* hit_;

};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_HIT_MANAGER_H_
