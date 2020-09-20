//
// Created by mikhail on 6/8/20.
//

#ifndef HTREE_TO_AT_SRC_SIM_TRACKS_MANAGER_H_
#define HTREE_TO_AT_SRC_SIM_TRACKS_MANAGER_H_
#include "detector_manager.h"
#include <TVector3.h>

namespace Analysis {

class SimTracksManager : public DetectorManager  {
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
    VTX_Y,
    VTX_Z,
  };
  static SimTracksManager * Instance(){
    if(!instance_)
      instance_ = new SimTracksManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override;
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
  static SimTracksManager * instance_;
  SimTracksManager() = default;
  ~SimTracksManager() = default;
  AnalysisTree::Particles* sim_tracks_{nullptr};
  AnalysisTree::Particle* track_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_SIM_TRACKS_MANAGER_H_
