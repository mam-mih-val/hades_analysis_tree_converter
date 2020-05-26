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
  void SetFile( std::string file_name ){
    file_=TFile::Open( file_name.data(), "recreate" );
  }
  void CreateTree(){
    config_.SetName("HADES_data");
    event_manager_ = EventManager::Instance();
    track_manager_ = TrackManager::Instance();
    hit_manager_ = HitManager::Instance();
    auto header = event_manager_->CreateEventHeader(config_);
    auto track_detector = track_manager_->CreateTrackDetector(config_);
    auto hit_detector = hit_manager_->CreateHitDetector(config_);

    tree_ = new TTree( "hades_analysis_tree", "Analysis Tree, HADES data" );
    tree_->Branch("event_header", &header);
    tree_->Branch("mdc_vtx_tracks", &track_detector);
    tree_->Branch("meta_hits", &hit_detector);
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
  void WriteEvent(){ tree_->Fill(); }
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
