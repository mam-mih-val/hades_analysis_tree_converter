//
// Created by mikhail on 5/24/20.
//

#ifndef HTREE_TO_AT_SRC_TREE_MANAGER_H_
#define HTREE_TO_AT_SRC_TREE_MANAGER_H_

#include <TFile.h>
#include <TTree.h>

#include "event_manager.h"
#include "mdc_meta_matching.h"
#include "mdc_tracks_manager.h"
#include "meta_hits_manager.h"
#include "reco_sim_matching.h"
#include "sim_event_manager.h"
#include "sim_tracks_manager.h"
#include "wall_hits_manager.h"
#include "start_hits_manager.h"

namespace Analysis {
class TreeManager {
public:
  static TreeManager * Instance(){
    if( !instance_ )
      instance_ = new TreeManager;
    return instance_;
  }
  void CreateTree(const std::string& file_name, const std::string& system, float energy, bool is_mc=false);
  void NewTrack(){
    track_manager_->NewTrack( config_ );
    hit_manager_->NewHit(config_);
  }
  void NewSimTrack(){
    sim_track_manager_->NewTrack(config_);
  }
  void NewWallModule(){
    wall_manager_->NewModule(config_);
  }
  void NewStartHit(){
    start_hits_manager_->NewHit(config_);
  }
  void CheckIfNewFile(){
    auto* file = tree_->GetCurrentFile();
    if( file != file_ ) {
      file_ = file;
      file_->cd();
      config_.Write("Configuration");
      data_header_.Write("DataHeader");
    }
  }
  void NewSimEvent(){
    sim_track_manager_->ClearDetector();
    sim_reco_matching_->ClearMatching();
  }
  void NewEvent(){
    start_hits_manager_->ClearDetector();
    track_manager_->ClearDetector();
    hit_manager_->ClearDetector();
    wall_manager_->ClearDetector();
    track_tof_matching_->ClearMatching();
  }
  void ReserveTracks(int n_tracks){
    track_manager_->ReserveTracks(n_tracks);
    hit_manager_->ReserveHits(n_tracks);
  }
  EventManager *GetEventManager() const { return event_manager_; }
  SimEventManager *GetSimEventManager() const { return sim_event_manager_; }
  MdcTracksManager *GetMdcTracksManager() const { return track_manager_; }
  SimTracksManager *GetSimTracksManager() const { return sim_track_manager_; }
  MetaHitsManager *GetMetaHitsManager() const { return hit_manager_; }
  WallHitsManager *GetWallHitsManager() const { return wall_manager_; }
  MdcMetaMatching *GetMdcMetaMatching() const  { return track_tof_matching_; }
  RecoSimMatching *GetRecoSimMatching() const { return sim_reco_matching_; }
  StartHitsManager *GetStartHitsManager() const;
  bool WriteEvent(){
    try {
      tree_->Fill();
      return true;
    } catch (std::exception &e) {
      return false;
    }
  }
  void Finalize(){
    file_->cd();
    tree_->Write();
    file_->Write();
    file_->Close();
  }

private:
  void RecordDataHeader();
  static TreeManager * instance_;
  TreeManager() = default;
  ~TreeManager() = default;

  AnalysisTree::Configuration config_;
  AnalysisTree::DataHeader data_header_;
  TFile* file_{nullptr};
  TTree* tree_{nullptr};
  std::string colliding_system_;
  float lab_energy_;
  EventManager* event_manager_{nullptr};
  SimEventManager* sim_event_manager_{nullptr};
  MdcTracksManager * track_manager_{nullptr};
  SimTracksManager * sim_track_manager_{nullptr};
  MetaHitsManager * hit_manager_{nullptr};
  WallHitsManager* wall_manager_{nullptr};
  StartHitsManager* start_hits_manager_{nullptr};
  MdcMetaMatching * track_tof_matching_{nullptr};
  RecoSimMatching * sim_reco_matching_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TREE_MANAGER_H_
