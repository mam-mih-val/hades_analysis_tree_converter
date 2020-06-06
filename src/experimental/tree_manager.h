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
#include "wall_hits_manager.h"
#include "track_tof_match.h"

namespace Analysis {
class TreeManager {
public:
  static TreeManager * Instance(){
    if( !instance_ )
      instance_ = new TreeManager;
    return instance_;
  }
  void CreateTree(const std::string& file_name){
    file_=TFile::Open( file_name.data(), "recreate" );
    config_.SetName("HADES_data");
    event_manager_ = EventManager::Instance();
    track_manager_ = TrackManager::Instance();
    hit_manager_ = HitManager::Instance();
    wall_manager_ = WallHitsManager::Instance();
    matching_ = TrackTofMatch::Instance();

    tree_ = new TTree( "hades_analysis_tree", "Analysis Tree, HADES data" );
    event_manager_->MakeBranch(config_, tree_);
    track_manager_->MakeBranch(config_, tree_);
    hit_manager_->MakeBranch(config_, tree_);
    wall_manager_->MakeBranch(config_, tree_);
    matching_->MakeBranch(config_, tree_);
    config_.Write("Configuration");
    RecordDataHeader();
  }
  void NewTrack(){
    track_manager_->NewTrack( config_ );
    hit_manager_->NewHit(config_);
  }
  void NewWallModule(){
    wall_manager_->NewModule(config_);
  }
  void NewEvent(){
    track_manager_->ClearDetector();
    hit_manager_->ClearDetector();
    wall_manager_->ClearDetector();
    matching_->ClearMatching();
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
    data_header_.Write("data_information");
  }
  static TreeManager * instance_;
  AnalysisTree::Configuration config_;
  AnalysisTree::DataHeader data_header_;
  TreeManager() = default;
  ~TreeManager() = default;

  TFile* file_{nullptr};
  TTree* tree_{nullptr};
  EventManager* event_manager_{nullptr};
  TrackManager* track_manager_{nullptr};
  HitManager* hit_manager_{nullptr};
  WallHitsManager *wall_manager_{nullptr};
  TrackTofMatch* matching_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TREE_MANAGER_H_
