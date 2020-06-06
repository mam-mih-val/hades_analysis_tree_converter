//
// Created by mikhail on 5/23/20.
//

#ifndef HTREE_TO_AT_SRC_HADES_EVENT_READER_H_
#define HTREE_TO_AT_SRC_HADES_EVENT_READER_H_

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
#include "walldef.h"

#include "hparticleevtcharaBK.h"

//#include "event_manager.h"
//#include "tree_manager.h"

#include <iostream>

class HadesEventReader {
public:
  HadesEventReader() :
                       loop_(true){};
  ~HadesEventReader() = default;
  void Init(std::string file_list){
    std::stringstream list{file_list};
    std::string file{};
    if( !file_list.empty() ){
      while(std::getline(list,file,',')){
        loop_.addFiles( file.data() );
        std::cout << file << " has been added to sequence" << std::endl;
      }
    }
    if(!loop_.setInput("-*,+HParticleCandSim,+HParticleEvtInfo,+HWallHitSim"))
    {
      cerr << "READBACK: ERROR : cannot read input !" << endl;
      std::abort();
    }
    loop_.printCategories();
    loop_.printChain();
    particle_category_ = (HCategory*)HCategoryManager::getCategory(catParticleCand);
    event_info_category_ = (HCategory*)HCategoryManager::getCategory(catParticleEvtInfo);
    wall_category_ = (HCategory*)HCategoryManager::getCategory(catWallHit);
  }
  void SwitchNextEvent(){
    if( !Eof() )
      read_bytes = loop_.nextEvent( position_ );
    position_++;
  }
  bool Eof(){
    return (position_ >= n_events_ || read_bytes <= 0);
  }
  void ReadEvent(){};
  void ReadParticleCandidates(){};
  void ReadWallHits(){};

private:
  HLoop loop_;
  HCategory* particle_category_{nullptr};
  HCategory* event_info_category_{nullptr};
  HCategory* wall_category_{nullptr};
  int read_bytes{1};
  long long n_events_{0};
  long long position_{0};
};

#endif // HTREE_TO_AT_SRC_HADES_EVENT_READER_H_
