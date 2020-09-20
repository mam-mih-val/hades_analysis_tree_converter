//
// Created by mikhail on 5/23/20.
//

#ifndef HTREE_TO_AT_SRC_TRACK_READER_H_
#define HTREE_TO_AT_SRC_TRACK_READER_H_

#include "detector_manager.h"
#include <TVector3.h>

namespace Analysis {
class MdcTracksManager : public DetectorManager {
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
    LAYERS_0,
    LAYERS_1,
    LAYERS_2,
    LAYERS_3,
    LAYERS_TOTAL,
    LAYERS_BITS,
    GEANT_PID,
    NUMBER_FIELDS_INT
  };
  static MdcTracksManager * Instance(){
    if( !instance_ )
      instance_ = new MdcTracksManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override;

  void NewTrack(AnalysisTree::Configuration &config){
    track_ = vtx_tracks_->AddChannel();
    track_->Init(config.GetBranchConfig(vtx_tracks_->GetId() ) );
  }
  int GetTrackId(){ return track_->GetId(); }
  void ClearDetector(){ vtx_tracks_->ClearChannels(); }
  void ReserveTracks(int n_tracks){ vtx_tracks_->Reserve(n_tracks); }
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
  static MdcTracksManager * instance_;
  MdcTracksManager() : vtx_tracks_{nullptr}, track_{nullptr} {};
  ~MdcTracksManager() = default;
  AnalysisTree::Particles* vtx_tracks_;
  AnalysisTree::Particle* track_;
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TRACK_READER_H_
