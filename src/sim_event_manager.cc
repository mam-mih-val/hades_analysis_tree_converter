//
// Created by mikhail on 6/8/20.
//

#include "sim_event_manager.h"
namespace Analysis{
SimEventManager* SimEventManager::instance_= nullptr;

void SimEventManager::MakeBranch(AnalysisTree::Configuration &config, TTree* tree) {

AnalysisTree::BranchConfig sim_header_branch(
    "sim_header", AnalysisTree::DetType::kEventHeader);

sim_header_branch.AddField<float>("beam_energy");
sim_header_branch.AddField<float>("impact_parameter");
sim_header_branch.AddField<float>("reaction_plane");
sim_header_branch.AddField<int>("geant_event_id");
sim_header_branch.AddField<int>("rejected_electrons");

fields_float_.insert( std::make_pair( BEAM_ENERGY, sim_header_branch.GetFieldId( "beam_energy" )  ) );
fields_float_.insert( std::make_pair( IMPACT_PARAMETER, sim_header_branch.GetFieldId( "impact_parameter" )  ) );
fields_float_.insert( std::make_pair( REACTION_PLANE, sim_header_branch.GetFieldId( "reaction_plane" )  ) );
fields_int_.insert( std::make_pair( GEANT_EVENT_ID, sim_header_branch.GetFieldId( "geant_event_id" )  ) );
fields_int_.insert( std::make_pair( REJECTED_ELECTRONS, sim_header_branch.GetFieldId( "rejected_electrons" )  ) );

config.AddBranchConfig(sim_header_branch);
event_header_ = new AnalysisTree::EventHeader(config.GetNumberOfBranches() - 1);
event_header_->Init(sim_header_branch);
tree->Branch("sim_header", "AnalysisTree::EventHeader", &event_header_);
}
}