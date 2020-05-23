//
// Created by mikhail on 5/23/20.
//

#ifndef HTREE_TO_AT_SRC_EVENT_MANAGER_H_
#define HTREE_TO_AT_SRC_EVENT_MANAGER_H_

#include "HADES_constants.h"

namespace Analysis {
class EventManager {
  EventManager() = default;
  ~EventManager() = default;
  AnalysisTree::EventHeader *
  CreateEventHeader(AnalysisTree::Configuration &config,
                       std::string branchName = "hades_event") {
    AnalysisTree::BranchConfig event_header_branch(
        branchName, AnalysisTree::DetType::kEventHeader);

    event_header_branch.AddField<float>("vtx_chi2");
    event_header_branch.AddField<int>("run_id");
    event_header_branch.AddField<int>("event_id");
    for (auto centEst : fCEmapNames) {
      event_header_branch.AddField<int>(centrality_estimators_.second); // centrality estimator
      event_header_branch.AddField<float>("centrality_" +
                                      centEst.second); // centrality class
    }
    for (auto trigger : triggers_)
      event_header_branch.AddField<bool>(trigger.second); // trigger name
    config.AddBranchConfig(event_header_branch);
    auto event_header = new AnalysisTree::EventHeader(config.GetLastId());
    event_header->Init(event_header_branch);
    return event_header;
  }
private:
  const std::map<int, std::string> centrality_estimators_ = {
      {HADES_constants::kNhitsTOF, "tof_hits"},
      {HADES_constants::kNhitsTOF_cut, "selected_tof_hits"},
      {HADES_constants::kNhitsRPC, "rpc_hits"},
      {HADES_constants::kNhitsRPC_cut, "selected_rpc_hits"},
      {HADES_constants::kNhitsTOF_RPC, "tof_rpc_hits"},
      {HADES_constants::kNhitsTOF_RPC_cut, "selected_tof_rpc_hits"},
      {HADES_constants::kNtracks, "mdc_tracks"},
      {HADES_constants::kNselectedTracks, "selected_mdc_tracks"},
      {HADES_constants::kFWSumChargeSpec, "fw_adc"},
      {HADES_constants::kFWSumChargeZ, "fw_charge"},
  };
  const map<int, std::string> triggers_ = {
      {HADES_constants::kGoodVertexClust, "kGoodVertexClust"},
      {HADES_constants::kGoodVertexCand, "kGoodVertexCand"},
      {HADES_constants::kGoodSTART, "kGoodSTART"},
      {HADES_constants::kNoPileUpSTART, "kNoPileUpSTART"},
      {HADES_constants::kNoPileUpMETA, "kNoPileUpMETA"},
      {HADES_constants::kNoPileUpMDC, "kNoPileUpMDC"},
      {HADES_constants::kNoFlashMDC, "kNoFlashMDC"},
      {HADES_constants::kGoodMDCMult, "kGoodMDCMult"},
      {HADES_constants::kGoodMDCMIPSMult, "kGoodMDCMIPSMult"},
      {HADES_constants::kGoodLepMult, "kGoodLepMult"},
      {HADES_constants::kGoodTRIGGER, "kGoodTRIGGER"},
      {HADES_constants::kGoodSTART2, "kGoodSTART2"},
      {HADES_constants::kNoVETO, "kNoVETO"},
      {HADES_constants::kGoodSTARTVETO, "kGoodSTARTVETO"},
      {HADES_constants::kGoodSTARTMETA, "kGoodSTARTMETA"},
      {HADES_constants::kPT1, "kPT1"},
      {HADES_constants::kPT2, "kPT2"},
      {HADES_constants::kPT3, "kPT3"}};
};

} // namespace Analysis
#endif // HTREE_TO_AT_SRC_EVENT_MANAGER_H_
