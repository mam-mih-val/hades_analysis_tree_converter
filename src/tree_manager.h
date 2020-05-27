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

    tree_ = new TTree( "hades_analysis_tree", "Analysis Tree, HADES data" );
    event_manager_->MakeBranch(config_, tree_);
    track_manager_->MakeBranch(config_, tree_);
    hit_manager_->MakeBranch(config_, tree_);
    config_.Write("configuration");
  }
  void NewTrack(){
    track_manager_->NewTrack( config_ );
    hit_manager_->NewHit(config_);
  }
  void NewEvent(){
    track_manager_->ClearDetector();
    hit_manager_->ClearDetector();
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
  static TreeManager * instance_;
  AnalysisTree::Configuration config_;
  TreeManager() = default;
  ~TreeManager() = default;

  TFile* file_{nullptr};
  TTree* tree_{nullptr};
  EventManager* event_manager_{nullptr};
  TrackManager* track_manager_{nullptr};
  HitManager* hit_manager_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TREE_MANAGER_H_
