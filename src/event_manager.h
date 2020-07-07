//
// Created by mikhail on 5/23/20.
//

#ifndef HTREE_TO_AT_SRC_EVENT_MANAGER_H_
#define HTREE_TO_AT_SRC_EVENT_MANAGER_H_

#include "HADES_constants.h"
#include "hparticleevtcharaBK.h"
#include "detector_manager.h"

namespace Analysis {

class EventManager : public DetectorManager {
public:
  static EventManager* Instance(){
    if( !instance_ )
      instance_ = new EventManager;
    return instance_;
  }
  enum FIELDS_FLOAT{
    VTX_CHI2=HParticleEvtCharaBK::kFWSumChargeZ+1, // Centrality estimators + corresponding centrality value;
    NUM_FIELDS_FLOAT
  };
  enum FIELDS_INT{
    RUN_ID=HParticleEvtCharaBK::kFWSumChargeZ+1,
    EVENT_ID,
    NUM_FIELDS_INT
  };
  enum FIELDS_BOOL{
    NUM_FIELDS_BOOL=HADES_constants::kPT3+1
  };
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override {
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
      {HParticleEvtCharaBK::kTOFtot, "tof_hits"},
      {HParticleEvtCharaBK::kTOF, "selected_tof_hits"},
      {HParticleEvtCharaBK::kRPCtot, "rpc_hits"},
      {HParticleEvtCharaBK::kRPC, "selected_rpc_hits"},
      {HParticleEvtCharaBK::kTOFRPCtot, "tof_rpc_hits"},
      {HParticleEvtCharaBK::kTOFRPC, "selected_tof_rpc_hits"},
      {HParticleEvtCharaBK::kPrimaryParticleCand, "mdc_tracks"},
      {HParticleEvtCharaBK::kSelectedParticleCand, "selected_mdc_tracks"},
      {HParticleEvtCharaBK::kFWSumChargeSpec, "fw_adc"},
      {HParticleEvtCharaBK::kFWSumChargeZ, "fw_charge"}
    };

    AnalysisTree::BranchConfig event_header_branch(
        "event_header", AnalysisTree::DetType::kEventHeader);

    for (auto centrality_estimator : centrality_estimators_names) {
      event_header_branch.AddField<int>(centrality_estimator.second); // centrality estimator
//      event_header_branch.AddField<float>(centrality_estimator.second+"_centrality"); // centrality value
      fields_int_.insert( std::make_pair( centrality_estimator.first,
                                          event_header_branch.GetFieldId( centrality_estimator.second ) ) );
//      fields_float_.insert( std::make_pair( centrality_estimator.first,
//                                          event_header_branch.GetFieldId( centrality_estimator.second+"_centrality" ) ) );
    }
    for (auto trigger : triggers_names) {
      event_header_branch.AddField<bool>(trigger.second); // trigger name
      fields_bool_.insert( std::make_pair( trigger.first,
                                            event_header_branch.GetFieldId( trigger.second ) ) );
     }

    event_header_branch.AddField<float>("vtx_chi2");
    event_header_branch.AddField<int>("run_id");
    event_header_branch.AddField<int>("event_id");

    fields_float_.insert( std::make_pair( VTX_CHI2, event_header_branch.GetFieldId( "vtx_chi2" )  ) );
    fields_int_.insert( std::make_pair( RUN_ID, event_header_branch.GetFieldId( "run_id" )  ) );
    fields_int_.insert( std::make_pair( EVENT_ID, event_header_branch.GetFieldId( "event_id" )  ) );

    config.AddBranchConfig(event_header_branch);
    event_header_ = new AnalysisTree::EventHeader(config.GetLastId());
    event_header_->Init(event_header_branch);
    tree->Branch("event_header", "AnalysisTree::EventHeader", &event_header_);
  }
  AnalysisTree::EventHeader* GetEventHeader(){ return event_header_; }
  void SetVertex(float x, float y, float z){
    event_header_->SetVertexX(x);
    event_header_->SetVertexY(y);
    event_header_->SetVertexZ(z);
  }
  void SetField(const int& value, int idx) override {
    event_header_->SetField( value, fields_int_.at(idx) );
  }
  void SetField(const float& value, int idx) override {
    event_header_->SetField( value, fields_float_.at(idx) );
  }
  void SetField(const bool& value, int idx) override {
    event_header_->SetField( value, fields_bool_.at(idx) );
  }

private:
  static EventManager* instance_;
  EventManager() = default;
  ~EventManager() = default;
  AnalysisTree::EventHeader* event_header_{nullptr};
};

} // namespace Analysis
#endif // HTREE_TO_AT_SRC_EVENT_MANAGER_H_
