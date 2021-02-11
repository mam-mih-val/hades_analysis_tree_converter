//
// Created by mikhail on 2/11/21.
//

#ifndef HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_ANALYSIS_TREE_MANAGER_H_
#define HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_ANALYSIS_TREE_MANAGER_H_

class AnalysisTreeManager {

private:
  AnalysisTree::Configuration config_;
  AnalysisTree::DataHeader data_header_;
  TFile* file_{nullptr};
  TTree* tree_{nullptr};
  std::string colliding_system_;
  float lab_energy_;
};

#endif // HTREE_TO_AT_SRC_GEANT_WALL_CONVERTER_ANALYSIS_TREE_MANAGER_H_
