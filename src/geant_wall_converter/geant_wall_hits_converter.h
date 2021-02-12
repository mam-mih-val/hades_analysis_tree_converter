//
// Created by mikhail on 2/11/21.
//

#ifndef HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_GEANT_WALL_HITS_CONVERTER_H_
#define HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_GEANT_WALL_HITS_CONVERTER_H_

#include <TTree.h>

#include <hcategory.h>

#include <AnalysisTree/Configuration.hpp>
#include <AnalysisTree/Detector.hpp>

class GeantWallHitsConverter {
public:
  GeantWallHitsConverter()= default;
  ~GeantWallHitsConverter()= default;

  static std::string GetInput() { return "+HGeantWallHits"; }
  void InitOutput(AnalysisTree::Configuration &config, TTree* tree);
  void InitInput();
  void Convert();

protected:
  void InitWallCellsPositions();
  HCategory* geant_wall_hits_category_;
  AnalysisTree::BranchConfig branch_config_;
  AnalysisTree::HitDetector *wall_hits_;
  float relative_amplitude_resolution{0.14};
  std::array<float, 304> x_wall_modules_positions_{};
  std::array<float, 304> y_wall_modules_positions_{};
  std::array<float, 304> z_wall_modules_positions_{};
};

#endif // HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_GEANT_WALL_HITS_CONVERTER_H_
