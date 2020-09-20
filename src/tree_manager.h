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

namespace Analysis {
class TreeManager {
public:
  static TreeManager * Instance(){
    if( !instance_ )
      instance_ = new TreeManager;
    return instance_;
  }
  void CreateTree(const std::string& file_name, bool is_mc=false);
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
    track_manager_->ClearDetector();
    hit_manager_->ClearDetector();
    wall_manager_->ClearDetector();
    track_tof_matching_->ClearMatching();
  }
  void ReserveTracks(int n_tracks){
    track_manager_->ReserveTracks(n_tracks);
    hit_manager_->ReserveHits(n_tracks);
  }
  inline EventManager *GetEventManager() const;
  inline SimEventManager *GetSimEventManager() const;
  inline MdcTracksManager *GetMdcTracksManager() const;
  inline SimTracksManager *GetSimTracksManager() const;
  inline MetaHitsManager *GetMetaHitsManager() const;
  inline WallHitsManager *GetWallHitsManager() const;
  inline MdcMetaMatching *GetMdcMetaMatching() const;
  inline RecoSimMatching *GetRecoSimMatching() const;
  bool WriteEvent(){
    try {
      tree_->Fill();
      return true;
    } catch (std::exception &e) {
      return false;
    }
  }
  void Finalize(){
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
  EventManager* event_manager_{nullptr};
  SimEventManager* sim_event_manager_{nullptr};
  MdcTracksManager * track_manager_{nullptr};
  SimTracksManager * sim_track_manager_{nullptr};
  MetaHitsManager * hit_manager_{nullptr};
  WallHitsManager* wall_manager_{nullptr};
  MdcMetaMatching * track_tof_matching_{nullptr};
  RecoSimMatching * sim_reco_matching_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TREE_MANAGER_H_
