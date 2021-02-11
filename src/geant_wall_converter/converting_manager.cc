//
// Created by mikhail on 2/11/21.
//
#include "converting_manager.h"

void ConvertingManager::WriteDataHeader() {
  const float T = lab_energy_;  // AGeV
  const float M = 0.938; // GeV
  const float GAMMA = (T + M) / M;
  const float BETA = sqrt(1 - (M * M) / (M + T) / (M + T));
  const float PZ = M * BETA * GAMMA;

  data_header_.SetSystem(colliding_system_);
  data_header_.SetBeamMomentum(PZ);
  data_header_.Write("DataHeader");
}
void ConvertingManager::InitInput(std::string files) {
  std::stringstream list{files};
  std::string file{};
  if( !files.empty() ){
    while(std::getline(list,file,',')){
      if( file.empty() )
        continue;
      loop_.addFile( file.data() );
      std::cout << file << " has been added to sequence" << std::endl;
    }
  }
  loop_.setInput("+*");
  loop_.printCategories();
  loop_.printChain();
//  HPhysicsConstants::loadGeantIons();
}
void ConvertingManager::InitOutput(const std::string& file_name, const std::string& tree_name) {
  file_=TFile::Open( file_name.c_str(), "recreate" );
  tree_=new TTree(tree_name.c_str(), "Analysis Tree, HADES data");

  geant_wall_hits_converter_ = new GeantWallHitsConverter;
  geant_wall_hits_converter_->InitInput();
  geant_wall_hits_converter_->InitOutput(config_, tree_);

  WriteDataHeader();
  config_.Write("Configuration");
}
void ConvertingManager::Process(long long N) {
  auto n_events = std::min(loop_.getEntries(),N);
  if( n_events < 0 )
    n_events=loop_.getEntries();
  for( long long int i=0; i<n_events; ++i ){
    loop_.nextEvent(i);
    geant_wall_hits_converter_->Convert();
    tree_->Fill();
    auto* file = tree_->GetCurrentFile();
    if( file != file_ ) {
      file_ = file;
      file_->cd();
      config_.Write("Configuration");
      data_header_.Write("DataHeader");
    }
  }
  std::cout << n_events << " were converted to AnalysisTree" << std::endl;
}
void ConvertingManager::Finalize() {
  file_->cd();
  tree_->Write();
  file_->Write();
  file_->Close();
}
