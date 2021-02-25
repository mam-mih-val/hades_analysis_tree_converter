//
// Created by mikhail on 2/11/21.
//

#ifndef HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_CONVERTING_MANAGER_H_
#define HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_CONVERTING_MANAGER_H_

#include <TFile.h>
#include <TTree.h>

#include "AnalysisTree/Configuration.hpp"
#include "AnalysisTree/DataHeader.hpp"

#include "hades.h"
#include "hcategory.h"
#include "hcategorymanager.h"
#include "henergylosscorrpar.h"
#include "heventheader.h"
#include "hloop.h"
#include "hparticlebooker.h"
#include "hparticlecand.h"
#include "hparticledef.h"
#include "hparticleevtinfo.h"
#include "hparticletool.h"
#include "hparticletracksorter.h"
#include "hphysicsconstants.h"
#include "hrun.h"
#include "hruntimedb.h"
#include "htime.h"
#include "hwallhit.h"
#include "hstart2hit.h"
#include "hstart2cal.h"
#include "walldef.h"
#include "hparticleevtchara.h"
#include "hgeantwall.h"

#include "geant_wall_hits_converter.h"

class ConvertingManager {
public:
  ConvertingManager() : loop_(true) {}
  virtual ~ConvertingManager() = default;
  void SetCollidingSystem(const std::string &colliding_system) {
    colliding_system_ = colliding_system;
  }
  void SetLabEnergy(float lab_energy) { lab_energy_ = lab_energy; }
  void InitInput(std::string files);
  void InitOutput(const std::string& file_name, const std::string& tree_name);
  void Process(long long first_evenet, long long N);
  void Finalize();
private:
  void WriteDataHeader();
  // output
  TFile* file_{nullptr};
  TTree* tree_{nullptr};
  AnalysisTree::Configuration config_;
  AnalysisTree::DataHeader data_header_;
  GeantWallHitsConverter* geant_wall_hits_converter_;

  std::string colliding_system_{"Au+Au"};
  float lab_energy_{1.23}; // AGeV

  // input
  HLoop loop_;
};

#endif // HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_CONVERTING_MANAGER_H_
