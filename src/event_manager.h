//
// Created by mikhail on 5/23/20.
//

#ifndef HTREE_TO_AT_SRC_EVENT_MANAGER_H_
#define HTREE_TO_AT_SRC_EVENT_MANAGER_H_

#include "HADES_constants.h"
#include "detector_manager.h"
#include "hparticledef.h"
#include "hparticlestructs.h"
#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hparticleevtinfo.h"

namespace Analysis {

class EventManager : public DetectorManager {
public:
  static EventManager* Instance(){
    if( !instance_ )
      instance_ = new EventManager;
    return instance_;
  }
  enum FIELDS_FLOAT{
    VTX_CHI2=HADES_constants::kFWSumChargeZ+1, // Centrality estimators + corresponding centrality value;
    NUM_FIELDS_FLOAT
  };
  enum FIELDS_INT{
    RUN_ID=HADES_constants::kFWSumChargeZ+1,
    EVENT_ID,
    NUM_FIELDS_INT
  };
  enum FIELDS_BOOL{
    NUM_FIELDS_BOOL=HADES_constants::kPT3+1
  };
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
  static EventManager* instance_;
  EventManager() = default;
  ~EventManager() = default;
  AnalysisTree::EventHeader* event_header_{nullptr};
};

} // namespace Analysis
#endif // HTREE_TO_AT_SRC_EVENT_MANAGER_H_
