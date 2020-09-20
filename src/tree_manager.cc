//
// Created by mikhail on 5/24/20.
//

#include "tree_manager.h"

namespace Analysis {
TreeManager *TreeManager::instance_ = nullptr;

void TreeManager::CreateTree(const std::string& file_name, bool is_mc){
  file_=TFile::Open( file_name.data(), "recreate" );
  event_manager_ = EventManager::Instance();
  track_manager_ = MdcTracksManager::Instance();
  hit_manager_ = MetaHitsManager::Instance();
  wall_manager_ = WallHitsManager::Instance();
  track_tof_matching_ = MdcMetaMatching::Instance();
  sim_event_manager_ = SimEventManager::Instance();
  sim_track_manager_ = SimTracksManager::Instance();
  sim_reco_matching_ = RecoSimMatching::Instance();

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

void TreeManager::RecordDataHeader(){
  const float T = 1.23;  // AGeV
  const float M = 0.938; // GeV
  const float GAMMA = (T + M) / M;
  const float BETA = sqrt(1 - (M * M) / (M + T) / (M + T));
  const float PZ = M * BETA * GAMMA;

  data_header_.SetSystem("Au+Au");
  data_header_.SetBeamMomentum(PZ);
  data_header_.Write("DataHeader");
}

EventManager *TreeManager::GetEventManager() const { return event_manager_; }
SimEventManager *TreeManager::GetSimEventManager() const {
  return sim_event_manager_;
}
MdcTracksManager *TreeManager::GetMdcTracksManager() const { return track_manager_; }
SimTracksManager *TreeManager::GetSimTracksManager() const {
  return sim_track_manager_;
}
MetaHitsManager *TreeManager::GetMetaHitsManager() const { return hit_manager_; }
WallHitsManager *TreeManager::GetWallHitsManager() const { return wall_manager_; }
MdcMetaMatching *TreeManager::GetMdcMetaMatching() const {
  return track_tof_matching_;
}
RecoSimMatching *TreeManager::GetRecoSimMatching() const {
  return sim_reco_matching_;
}
}