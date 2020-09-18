//
// Created by mikhail on 6/8/20.
//

#ifndef HTREE_TO_AT_SRC_SIM_EVENT_MANAGER_H_
#define HTREE_TO_AT_SRC_SIM_EVENT_MANAGER_H_

#include "detector_manager.h"
namespace Analysis {
class SimEventManager : public DetectorManager{
public:
  enum sim_header_fields_float{
    BEAM_ENERGY,
    IMPACT_PARAMETER,
    REACTION_PLANE
  };
  enum sim_header_fields_int{
    GEANT_EVENT_ID,
    REJECTED_ELECTRONS,
  };
  static SimEventManager* Instance(){
    if( !instance_ )
      instance_=new SimEventManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override {

    AnalysisTree::BranchConfig sim_header_branch(
        "sim_header", AnalysisTree::DetType::kEventHeader);

    sim_header_branch.AddField<float>("beam_energy");
    sim_header_branch.AddField<float>("impact_parameter");
    sim_header_branch.AddField<float>("reaction_plane");
    sim_header_branch.AddField<int>("geant_event_id");
    sim_header_branch.AddField<int>("rejected_electrons");

    fields_float_.insert( std::make_pair( BEAM_ENERGY, sim_header_branch.GetFieldId( "beam_energy" )  ) );
    fields_float_.insert( std::make_pair( IMPACT_PARAMETER, sim_header_branch.GetFieldId( "impact_parameter" )  ) );
    fields_float_.insert( std::make_pair( REACTION_PLANE, sim_header_branch.GetFieldId( "reaction_plane" )  ) );
    fields_int_.insert( std::make_pair( GEANT_EVENT_ID, sim_header_branch.GetFieldId( "geant_event_id" )  ) );
    fields_int_.insert( std::make_pair( REJECTED_ELECTRONS, sim_header_branch.GetFieldId( "rejected_electrons" )  ) );

    config.AddBranchConfig(sim_header_branch);
    event_header_ = new AnalysisTree::EventHeader(config.GetLastId());
    event_header_->Init(sim_header_branch);
    tree->Branch("sim_header", "AnalysisTree::EventHeader", &event_header_);
  }
  AnalysisTree::EventHeader* GetEventHeader(){ return event_header_; }
  void SetVertex(float x, float y, float z){
    event_header_->SetVertexPosition3({x, y, z});
  }
  void SetField(const int& value, int idx) override {
    event_header_->SetField( value, fields_int_.at(idx) );
  }
  void SetField(const float& value, int idx) override {
    event_header_->SetField( value, fields_float_.at(idx) );
  }
  void SetField(const bool& value, int idx) override {
    event_header_->SetField( value, fields_bool_.at(idx) );
  }
private:
  static SimEventManager* instance_;
  SimEventManager() = default;
  ~SimEventManager() = default;
  AnalysisTree::EventHeader* event_header_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_SIM_EVENT_MANAGER_H_
