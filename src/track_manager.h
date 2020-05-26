//
// Created by mikhail on 5/23/20.
//

#ifndef HTREE_TO_AT_SRC_TRACK_READER_H_
#define HTREE_TO_AT_SRC_TRACK_READER_H_

#include "detector_manager.h"

namespace Analysis {
class TrackManager : public DetectorManager {
public:
  enum track_fields_float{
    CHI2=0,
    VTX_CHI2,
    DE_DX,
    DCA_XY,
    DCA_Z,
    NUMBER_FIELDS_FLOAT
  };
  enum track_fields_int{
    CHARGE=0,
    N_HITS_0,
    N_HITS_1,
    N_HITS_2,
    GEANT_PID,
    NUMBER_FIELDS_INT
  };
  static TrackManager* Instance(){
    if( !instance_ )
      instance_ = new TrackManager;
    return instance_;
  }
  AnalysisTree::Particles*
  CreateTrackDetector(AnalysisTree::Configuration &config) {
    AnalysisTree::BranchConfig vtx_tracks_branch("mdc_vtx_tracks", AnalysisTree::DetType::kParticle);
    vtx_tracks_branch.AddField<float>("chi2");
    vtx_tracks_branch.AddField<float>("vtx_chi2");
    vtx_tracks_branch.AddField<float>("dEdx");
    vtx_tracks_branch.AddField<float>("dca_xy");
    vtx_tracks_branch.AddField<float>("dca_z");
    vtx_tracks_branch.AddField<int>("charge");
    vtx_tracks_branch.AddField<int>("nhits_0");
    vtx_tracks_branch.AddField<int>("nhits_1");
    vtx_tracks_branch.AddField<int>("nhits_2");
    vtx_tracks_branch.AddField<int>("geant_pid");

    vtx_tracks_ = new AnalysisTree::Particles(config.GetLastId());

    fields_float_.insert( std::make_pair(CHI2, vtx_tracks_branch.GetFieldId("chi2")) );
    fields_float_.insert( std::make_pair(VTX_CHI2, vtx_tracks_branch.GetFieldId("vtx_chi2")) );
    fields_float_.insert( std::make_pair(DE_DX, vtx_tracks_branch.GetFieldId("dEdx")) );
    fields_float_.insert( std::make_pair(DCA_XY, vtx_tracks_branch.GetFieldId("dca_xy")) );
    fields_float_.insert( std::make_pair(DCA_Z, vtx_tracks_branch.GetFieldId("dca_z")) );

    fields_int_.insert( std::make_pair(CHI2, vtx_tracks_branch.GetFieldId("chi2")) );
    fields_int_.insert( std::make_pair(N_HITS_0, vtx_tracks_branch.GetFieldId("nhits_0")) );
    fields_int_.insert( std::make_pair(N_HITS_1, vtx_tracks_branch.GetFieldId("nhits_1")) );
    fields_int_.insert( std::make_pair(N_HITS_2, vtx_tracks_branch.GetFieldId("nhits_2")) );
    fields_int_.insert( std::make_pair(GEANT_PID, vtx_tracks_branch.GetFieldId("geant_pid")) );

    config.AddBranchConfig(vtx_tracks_branch);
    return vtx_tracks_;
  }
  void NewTrack(AnalysisTree::Configuration &config){
    track_ = vtx_tracks_->AddChannel();
    track_->Init(config.GetBranchConfig(vtx_tracks_->GetId() ) );
  }
  void ClearDetector(){ vtx_tracks_->ClearChannels(); }
  void SetField(int value, int idx) override{
    track_->SetField( value, fields_int_.at(idx) );
  }
  void SetField(float value, int idx) override{
    track_->SetField( value, fields_float_.at(idx) );
  }
  void SetField(bool value, int idx) override{
    track_->SetField( value, fields_bool_.at(idx) );
  }
  void SetMomentum(const TVector3& p){
    track_->SetMomentum3(p);
  }
  void SetMass(const float& mass){
    track_->SetMass(mass);
  }

private:
  static TrackManager* instance_;
  TrackManager() = default;
  ~TrackManager() = default;
  AnalysisTree::Particles * vtx_tracks_{nullptr};
  AnalysisTree::Particle* track_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TRACK_READER_H_
