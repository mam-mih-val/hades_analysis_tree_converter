//
// Created by mikhail on 5/24/20.
//

#ifndef HTREE_TO_AT_SRC_TREE_MANAGER_H_
#define HTREE_TO_AT_SRC_TREE_MANAGER_H_

#include <TTree.h>
#include <TFile.h>

#include "event_manager.h"
#include "hit_manager.h"
#include "track_manager.h"
#include "sim_track_manager.h"
#include "wall_hits_manager.h"
#include "sim_event_manager.h"
#include "track_tof_match.h"
#include "sim_reco_match.h"

namespace Analysis {
class TreeManager {
public:
  static TreeManager * Instance(){
    if( !instance_ )
      instance_ = new TreeManager;
    return instance_;
  }
  void CreateTree(const std::string& file_name, bool is_mc=false){
    file_=TFile::Open( file_name.data(), "recreate" );
    event_manager_ = EventManager::Instance();
    track_manager_ = TrackManager::Instance();
    hit_manager_ = HitManager::Instance();
    wall_manager_ = WallHitsManager::Instance();
    track_tof_matching_ = TrackTofMatch::Instance();
    sim_event_manager_ = SimEventManager::Instance();
    sim_track_manager_ = SimTrackManager::Instance();
    sim_reco_matching_ = SimRecoMatch::Instance();

    tree_ = new TTree( "hades_analysis_tree", "Analysis Tree, HADES data" );
    event_manager_->MakeBranch(config_, tree_);
    track_manager_->MakeBranch(config_, tree_);
    hit_manager_->MakeBranch(config_, tree_);
    wall_manager_->MakeBranch(config_, tree_);
    track_tof_matching_->MakeBranch(config_, tree_);
    if( is_mc ) {
      sim_event_manager_->MakeBranch(config_, tree_);
      sim_track_manager_->MakeBranch(config_, tree_);
      sim_reco_matching_->MakeBranch(config_, tree_);
    }
    config_.Write("Configuration");
    RecordDataHeader();
    std::cout << "Analysis Tree Configuration:" << std::endl;
    config_.Print();
  }
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
  void RecordDataHeader(){
    const float T = 1.23;  // AGeV
    const float M = 0.938; // GeV
    const float GAMMA = (T + M) / M;
    const float BETA = sqrt(1 - (M * M) / (M + T) / (M + T));
    const float PZ = M * BETA * GAMMA;

    data_header_.SetSystem("Au+Au");
    data_header_.SetBeamMomentum(PZ);
    data_header_.Write("DataHeader");
  }
  static TreeManager * instance_;
  TreeManager() = default;
  ~TreeManager() = default;

  AnalysisTree::Configuration config_;
  AnalysisTree::DataHeader data_header_;
  TFile* file_{nullptr};
  TTree* tree_{nullptr};
  EventManager* event_manager_{nullptr};
  SimEventManager* sim_event_manager_{nullptr};
  TrackManager* track_manager_{nullptr};
  SimTrackManager* sim_track_manager_{nullptr};
  HitManager* hit_manager_{nullptr};
  WallHitsManager *wall_manager_{nullptr};
  TrackTofMatch*track_tof_matching_{nullptr};
  SimRecoMatch* sim_reco_matching_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TREE_MANAGER_H_
