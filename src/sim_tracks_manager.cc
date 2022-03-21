//
// Created by mikhail on 6/8/20.
//

#include "sim_tracks_manager.h"

namespace Analysis{
SimTracksManager *SimTracksManager::instance_= nullptr;

void SimTracksManager::MakeBranch(AnalysisTree::Configuration &config, TTree* tree) {
AnalysisTree::BranchConfig sim_tracks_branch("sim_tracks", AnalysisTree::DetType::kParticle);
sim_tracks_branch.AddField<int>("geant_pid");
sim_tracks_branch.AddField<int>("geant_track_id");
sim_tracks_branch.AddField<int>("geant_parent_id");
sim_tracks_branch.AddField<int>("geant_medium_number");
sim_tracks_branch.AddField<int>("geant_process_id");

sim_tracks_branch.AddField<bool>("is_primary");

sim_tracks_branch.AddField<float>("vtx_x");
sim_tracks_branch.AddField<float>("vtx_y");
sim_tracks_branch.AddField<float>("vtx_z");

fields_int_.insert( std::make_pair(GEANT_PID, sim_tracks_branch.GetFieldId("geant_pid")) );
fields_int_.insert( std::make_pair(GEANT_TRACK_ID, sim_tracks_branch.GetFieldId("geant_track_id")) );
fields_int_.insert( std::make_pair(GEANT_PARENT_ID, sim_tracks_branch.GetFieldId("geant_parent_id")) );
fields_int_.insert( std::make_pair(GEANT_MEDIUM_NUMBER, sim_tracks_branch.GetFieldId("geant_medium_number")) );
fields_int_.insert( std::make_pair(GEANT_PROCESS_ID, sim_tracks_branch.GetFieldId("geant_process_id")) );

fields_bool_.insert( std::make_pair(IS_PRIMARY, sim_tracks_branch.GetFieldId("is_primary")) );

fields_float_.insert( std::make_pair(VTX_X, sim_tracks_branch.GetFieldId("vtx_x")) );
fields_float_.insert( std::make_pair(VTX_Y, sim_tracks_branch.GetFieldId("vtx_y")) );
fields_float_.insert( std::make_pair(VTX_Z, sim_tracks_branch.GetFieldId("vtx_z")) );

config.AddBranchConfig(sim_tracks_branch);
sim_tracks_ = new AnalysisTree::Particles(config.GetNumberOfBranches());
tree->Branch("sim_tracks", "AnalysisTree::Particles", &sim_tracks_);
}
}