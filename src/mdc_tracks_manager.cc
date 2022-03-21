//
// Created by mikhail on 5/23/20.
//

#include "mdc_tracks_manager.h"

namespace Analysis{
MdcTracksManager *MdcTracksManager::instance_=nullptr;

void MdcTracksManager::MakeBranch(AnalysisTree::Configuration &config, TTree* tree) {
AnalysisTree::BranchConfig vtx_tracks_branch("mdc_vtx_tracks", AnalysisTree::DetType::kParticle);
vtx_tracks_branch.AddField<float>("chi2");
vtx_tracks_branch.AddField<float>("vtx_chi2");
vtx_tracks_branch.AddField<float>("dEdx");
vtx_tracks_branch.AddField<float>("dca_xy");
vtx_tracks_branch.AddField<float>("dca_z");
vtx_tracks_branch.AddField<float>("phi_out");
vtx_tracks_branch.AddField<float>("theta_out");
vtx_tracks_branch.AddField<float>("chi2_in");
vtx_tracks_branch.AddField<float>("chi2_out");
vtx_tracks_branch.AddField<int>("charge");
vtx_tracks_branch.AddField<int>("nhits_0");
vtx_tracks_branch.AddField<int>("nhits_1");
vtx_tracks_branch.AddField<int>("nhits_2");
vtx_tracks_branch.AddField<int>("layers_0"); // fired layers in station 0
vtx_tracks_branch.AddField<int>("layers_1"); // fired layers in station 1
vtx_tracks_branch.AddField<int>("layers_2"); // fired layers in station 2
vtx_tracks_branch.AddField<int>("layers_3"); // fired layers in station 3
vtx_tracks_branch.AddField<int>("layers_total"); // sum over all layers
vtx_tracks_branch.AddField<int>("layers_bits");
vtx_tracks_branch.AddField<int>("geant_pid");

fields_float_.insert( std::make_pair(CHI2, vtx_tracks_branch.GetFieldId("chi2")) );
fields_float_.insert( std::make_pair(VTX_CHI2, vtx_tracks_branch.GetFieldId("vtx_chi2")) );
fields_float_.insert( std::make_pair(DE_DX, vtx_tracks_branch.GetFieldId("dEdx")) );
fields_float_.insert( std::make_pair(DCA_XY, vtx_tracks_branch.GetFieldId("dca_xy")) );
fields_float_.insert( std::make_pair(DCA_Z, vtx_tracks_branch.GetFieldId("dca_z")) );
fields_float_.insert( std::make_pair(PHI_OUT, vtx_tracks_branch.GetFieldId("phi_out")) );
fields_float_.insert( std::make_pair(THETA_OUT, vtx_tracks_branch.GetFieldId("theta_out")) );
fields_float_.insert( std::make_pair(CHI2_IN, vtx_tracks_branch.GetFieldId("chi2_in")) );
fields_float_.insert( std::make_pair(CHI2_OUT, vtx_tracks_branch.GetFieldId("chi2_out")) );

fields_int_.insert( std::make_pair(CHI2, vtx_tracks_branch.GetFieldId("chi2")) );
fields_int_.insert( std::make_pair(N_HITS_0, vtx_tracks_branch.GetFieldId("nhits_0")) );
fields_int_.insert( std::make_pair(N_HITS_1, vtx_tracks_branch.GetFieldId("nhits_1")) );
fields_int_.insert( std::make_pair(N_HITS_2, vtx_tracks_branch.GetFieldId("nhits_2")) );
fields_int_.insert( std::make_pair(GEANT_PID, vtx_tracks_branch.GetFieldId("geant_pid")) );

fields_int_.insert( std::make_pair(LAYERS_0, vtx_tracks_branch.GetFieldId("layers_0")) );
fields_int_.insert( std::make_pair(LAYERS_1, vtx_tracks_branch.GetFieldId("layers_1")) );
fields_int_.insert( std::make_pair(LAYERS_2, vtx_tracks_branch.GetFieldId("layers_2")) );
fields_int_.insert( std::make_pair(LAYERS_3, vtx_tracks_branch.GetFieldId("layers_3")) );
fields_int_.insert( std::make_pair(LAYERS_TOTAL, vtx_tracks_branch.GetFieldId("layers_total")) );
fields_int_.insert( std::make_pair(LAYERS_BITS, vtx_tracks_branch.GetFieldId("layers_bits")) );

config.AddBranchConfig(vtx_tracks_branch);
vtx_tracks_ = new AnalysisTree::Particles(config.GetNumberOfBranches());
tree->Branch("mdc_vtx_tracks", "AnalysisTree::Particles", &vtx_tracks_);
}
}