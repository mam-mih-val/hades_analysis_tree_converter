//
// Created by mikhail on 6/8/20.
//

#ifndef HTREE_TO_AT_SRC_SIM_EVENT_MANAGER_H_
#define HTREE_TO_AT_SRC_SIM_EVENT_MANAGER_H_

#include "detector_manager.h"
namespace Analysis {
class SimEventManager : public DetectorManager{
public:
  enum sim_header_fields_float{
    BEAM_ENERGY,
    IMPACT_PARAMETER,
    REACTION_PLANE
  };
  enum sim_header_fields_int{
    GEANT_EVENT_ID,
    REJECTED_ELECTRONS,
  };
  static SimEventManager* Instance(){
    if( !instance_ )
      instance_=new SimEventManager;
    return instance_;
  }
  void MakeBranch(AnalysisTree::Configuration &config, TTree* tree) override;
  AnalysisTree::EventHeader* GetEventHeader(){ return event_header_; }
  void SetVertex(float x, float y, float z){
    event_header_->SetVertexPosition3({x, y, z});
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
  static SimEventManager* instance_;
  SimEventManager() = default;
  ~SimEventManager() = default;
  AnalysisTree::EventHeader* event_header_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_SIM_EVENT_MANAGER_H_
