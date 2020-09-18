//
// Created by mikhail on 6/8/20.
//

#ifndef HTREE_TO_AT_SRC_SIM_TRACK_MANAGER_H_
#define HTREE_TO_AT_SRC_SIM_TRACK_MANAGER_H_
#include "detector_manager.h"

namespace Analysis {
class SimTrackManager : public DetectorManager  {
public:
  enum sim_track_fields_int{
    GEANT_PID=0,
    GEANT_TRACK_ID,
    GEANT_PARENT_ID,
    GEANT_MEDIUM_NUMBER,
    GEANT_PROCESS_ID,
  };
  enum sim_track_fields_bool{
    IS_PRIMARY=0,
  };
  enum sim_track_fields_float{
    VTX_X=0,
    VTX_Y=0,
    VTX_Z=0,
  };
  static SimTrackManager * Instance(){
    if(!instance_)
      instance_ = new SimTrackManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override {
    AnalysisTree::BranchConfig sim_tracks_branch("sim_tracks", AnalysisTree::DetType::kParticle);
    sim_tracks_branch.AddField<int>("geant_pid");
    sim_tracks_branch.AddField<int>("geant_track_id");
    sim_tracks_branch.AddField<int>("geant_parent_id");
    sim_tracks_branch.AddField<int>("geant_medium_number");
    sim_tracks_branch.AddField<int>("geant_process_id");

    sim_tracks_branch.AddField<bool>("is_primary");

    sim_tracks_branch.AddField<float>("vtx_x");
    sim_tracks_branch.AddField<float>("vtx_y");
    sim_tracks_branch.AddField<float>("vtx_z");

    fields_int_.insert( std::make_pair(GEANT_PID, sim_tracks_branch.GetFieldId("geant_pid")) );
    fields_int_.insert( std::make_pair(GEANT_TRACK_ID, sim_tracks_branch.GetFieldId("geant_track_id")) );
    fields_int_.insert( std::make_pair(GEANT_PARENT_ID, sim_tracks_branch.GetFieldId("geant_parent_id")) );
    fields_int_.insert( std::make_pair(GEANT_MEDIUM_NUMBER, sim_tracks_branch.GetFieldId("geant_medium_number")) );
    fields_int_.insert( std::make_pair(GEANT_PROCESS_ID, sim_tracks_branch.GetFieldId("geant_process_id")) );

    fields_bool_.insert( std::make_pair(IS_PRIMARY, sim_tracks_branch.GetFieldId("is_primary")) );

    fields_float_.insert( std::make_pair(VTX_X, sim_tracks_branch.GetFieldId("vtx_x")) );
    fields_float_.insert( std::make_pair(VTX_Y, sim_tracks_branch.GetFieldId("vtx_y")) );
    fields_float_.insert( std::make_pair(VTX_Z, sim_tracks_branch.GetFieldId("vtx_z")) );

    config.AddBranchConfig(sim_tracks_branch);
    sim_tracks_ = new AnalysisTree::Particles(config.GetLastId());
    tree->Branch("sim_tracks", "AnalysisTree::Particles", &sim_tracks_);
  }
  void NewTrack(AnalysisTree::Configuration &config){
    track_ = sim_tracks_->AddChannel();
    track_->Init(config.GetBranchConfig(sim_tracks_->GetId() ) );
  }
  int GetTrackId(){ return track_->GetId(); }
  void ClearDetector(){ sim_tracks_->ClearChannels(); }
  void SetField(const int& value, int idx) override{
    track_->SetField( value, fields_int_.at(idx) );
  }
  void SetField(const float& value, int idx) override{
    track_->SetField( value, fields_float_.at(idx) );
  }
  void SetField(const bool& value, int idx) override{
    track_->SetField( value, fields_bool_.at(idx) );
  }
  void SetMomentum(const TVector3& p){
    track_->SetMomentum3(p);
  }
  void SetMass(const float& mass){
    track_->SetMass(mass);
  }
  void SetPdgCode( long long pdg_code ){
    track_->SetPid(pdg_code);
  }
private:
  static SimTrackManager * instance_;
  SimTrackManager() = default;
  ~SimTrackManager() = default;
  AnalysisTree::Particles* sim_tracks_{nullptr};
  AnalysisTree::Particle* track_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_SIM_TRACK_MANAGER_H_
