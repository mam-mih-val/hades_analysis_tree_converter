//
// Created by mikhail on 5/23/20.
//

#include "event_manager.h"

namespace Analysis {
EventManager *EventManager::instance_ = nullptr;

void EventManager::MakeBranch(AnalysisTree::Configuration &config,
                              TTree *tree) {
  fields_int_.clear();
  fields_float_.clear();
  fields_bool_.clear();

  const std::map<int, std::string> triggers_names{
      {Particle::kGoodVertexClust, "good_vertex_cluster"},
      {Particle::kGoodVertexCand, "good_vertex_candidate"},
      {Particle::kGoodSTART, "good_start"},
      {Particle::kNoPileUpSTART, "no_pile_up_start"},
      {Particle::kNoPileUpMETA, "no_pile_up_meta"},
      {Particle::kNoPileUpMDC, "no_pile_up_mdc"},
      {Particle::kNoFlashMDC, "no_flash_mdc"},
      {Particle::kGoodMDCMult, "good_mdc_multiplicity"},
      {Particle::kGoodMDCMIPSMult, "good_mdc_mips_multiplicity"},
      {Particle::kGoodLepMult, "good_lepton_multiplicity"},
      {Particle::kGoodTRIGGER, "good_trigger"},
      {Particle::kGoodSTART2, "good_start2"},
      {Particle::kNoVETO, "no_veto"},
      {Particle::kGoodSTARTVETO, "good_start_veto"},
      {Particle::kGoodSTARTMETA, "good_start_meta"},
      {11, "physical_trigger_1"}, // Physical Trigger 1
      {12, "physical_trigger_2"}, // Physical Trigger 2
      {13, "physical_trigger_3"}  // Physical Trigger 3
  };
  const std::map<int, std::string> centrality_estimators_names{
      {HADES_constants::kTOFtot, "tof_hits"},
      {HADES_constants::kTOF, "selected_tof_hits"},
      {HADES_constants::kRPCtot, "rpc_hits"},
      {HADES_constants::kRPC, "selected_rpc_hits"},
      {HADES_constants::kTOFRPCtot, "tof_rpc_hits"},
      {HADES_constants::kTOFRPC, "selected_tof_rpc_hits"},
      {HADES_constants::kPrimaryParticleCand, "mdc_tracks"},
      {HADES_constants::kSelectedParticleCand, "selected_mdc_tracks"},
      {HADES_constants::kFWSumChargeSpec, "fw_adc"},
      {HADES_constants::kFWSumChargeZ, "fw_charge"}};

  AnalysisTree::BranchConfig event_header_branch(
      "event_header", AnalysisTree::DetType::kEventHeader);

  for (auto centrality_estimator : centrality_estimators_names) {
    event_header_branch.AddField<int>(
        centrality_estimator.second); // centrality estimator
    event_header_branch.AddField<float>(centrality_estimator.second +
                                        "_centrality"); // centrality value
    fields_int_.insert(std::make_pair(
        centrality_estimator.first,
        event_header_branch.GetFieldId(centrality_estimator.second)));
    fields_float_.insert(
        std::make_pair(centrality_estimator.first,
                       event_header_branch.GetFieldId(
                           centrality_estimator.second + "_centrality")));
  }
  for (auto trigger : triggers_names) {
    event_header_branch.AddField<bool>(trigger.second); // trigger name
    fields_bool_.insert(std::make_pair(
        trigger.first, event_header_branch.GetFieldId(trigger.second)));
  }

  event_header_branch.AddField<float>("vtx_chi2");
  event_header_branch.AddField<float>("start_time");
  event_header_branch.AddField<int>("run_id");
  event_header_branch.AddField<int>("event_id");
  event_header_branch.AddField<int>("target_segment");
  event_header_branch.AddField<int>("start_module");
  event_header_branch.AddField<int>("start_strip");
  event_header_branch.AddField<int>("start_multiplicity");
  event_header_branch.AddField<bool>("has_passed_szymon_cut");

  fields_bool_.insert(
      std::make_pair(HAS_PASSED_SZYMON_CUT, event_header_branch.GetFieldId("has_passed_szymon_cut")));

  fields_float_.insert(
      std::make_pair(VTX_CHI2, event_header_branch.GetFieldId("vtx_chi2")));
  fields_float_.insert(
      std::make_pair(START_TIME, event_header_branch.GetFieldId("start_time")));
  fields_int_.insert(
      std::make_pair(RUN_ID, event_header_branch.GetFieldId("run_id")));
  fields_int_.insert(
      std::make_pair(EVENT_ID, event_header_branch.GetFieldId("event_id")));
  fields_int_.insert(
      std::make_pair(TARGER_SEGMENT, event_header_branch.GetFieldId("target_segment")));
  fields_int_.insert(
      std::make_pair(START_MODULE, event_header_branch.GetFieldId("start_module")));
  fields_int_.insert(
      std::make_pair(START_STRIP, event_header_branch.GetFieldId("start_strip")));
  fields_int_.insert(
      std::make_pair(START_MULT, event_header_branch.GetFieldId("start_multiplicity")));

  config.AddBranchConfig(event_header_branch);
  event_header_ = new AnalysisTree::EventHeader(config.GetLastId());
  event_header_->Init(event_header_branch);
  tree->Branch("event_header", "AnalysisTree::EventHeader", &event_header_);
}

} // namespace Analysis