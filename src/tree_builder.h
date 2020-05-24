//
// Created by mikhail on 5/24/20.
//

#ifndef HTREE_TO_AT_SRC_TREE_BUILDER_H_
#define HTREE_TO_AT_SRC_TREE_BUILDER_H_

#include <TTree.h"
#include <TFile.h"

#include "event_manager.h"

namespace Analysis {
class TreeBuilder {
  static TreeBuilder* Instance(){
    if( !instance_ )
      instance_ = new TreeBuilder;
    return instance_;
  }

  void SetFile( std::string file_name ){
    file_=TFile::Open( file_name.data(), "recreate" );
  }

  void CreateTree(){
    auto header = EventManager::Instance()->CreateEventHeader();
    tree_ = new TTree( "hades_analysis_tree", "Analysis Tree, HADES data" );
    tree->Branch( "event_header", &header );
  }
  void Fill(){ tree->Fill(); }

  void Finalize(){
    tree_->Write();
    file_->Write();
    file_->Close();
  }

private:
  static TreeBuilder* instance_;
  TreeBuilder() = default;
  ~TreeBuilder() = default;

  TFile* file_{nullptr};
  TTree* tree_{nullptr};
};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TREE_BUILDER_H_
