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
  };
  enum sim_track_fields_bool{
    IS_PRIMARY=0,
  };
  static SimTrackManager * Instance(){
    if(!instance_)
      instance_ = new SimTrackManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override {
    AnalysisTree::BranchConfig sim_tracks_branch("sim_tracks", AnalysisTree::DetType::kParticle);
    sim_tracks_branch.AddField<int>("geant_pid");
    sim_tracks_branch.AddField<bool>("is_primary");

    fields_int_.insert( std::make_pair(GEANT_PID, sim_tracks_branch.GetFieldId("geant_pid")) );
    fields_bool_.insert( std::make_pair(IS_PRIMARY, sim_tracks_branch.GetFieldId("is_primary")) );

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
  void SetPdgCode( long long x pdg_code ){
    track_->SetPid(pdg);
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
