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
#include "hstart2hit.h"
#include "hstart2cal.h"
#include "walldef.h"
#include "hparticleevtchara.h"

#include "event_manager.h"
#include "tree_manager.h"
#include "start_hits_manager.h"

#include "TDatabasePDG.h"
#include <TChain.h>

#include <iostream>

class HadesEventReader {
public:
  HadesEventReader() = delete;
  explicit HadesEventReader(bool is_mc) : loop_(true), is_mc_(is_mc){};
  ~HadesEventReader() = default;
  void Init(const std::string& file_list){
    std::stringstream list{file_list};
    std::string file{};
    if( !file_list.empty() ){
      while(std::getline(list,file,',')){
        if( file=="" )
          continue;
        loop_.addFiles( file.data() );
        std::cout << file << " has been added to sequence" << std::endl;
      }
    }
    if( !is_mc_ ) {
      if (!loop_.setInput("-*,+HParticleCand,+HParticleEvtInfo,+HWallHit,+HStart2Hit,+HStart2Cal")) {
        cerr << "READBACK: ERROR : cannot read input !" << endl;
        std::abort();
      }
    }
    if( is_mc_ ){
      if (!loop_.setInput("-*,+HParticleCand,+HParticleEvtInfo,+HWallHit,+HGeantKine")) {
        cerr << "READBACK: ERROR : cannot read input !" << endl;
        std::abort();
      }
    }
    HPhysicsConstants::loadGeantIons();
    loop_.printCategories();
    loop_.printChain();
    dE_dx_corr_.setDefaultPar("apr12");
    particle_category_ = (HCategory*)HCategoryManager::getCategory(catParticleCand);
    event_info_category_ = (HCategory*)HCategoryManager::getCategory(catParticleEvtInfo);
    wall_category_ = (HCategory*)HCategoryManager::getCategory(catWallHit);
    if( !is_mc_ ) {
      start2hit_category_ = (HCategory *)HCategoryManager::getCategory(131);
      start2cal_category_ = (HCategory *)HCategoryManager::getCategory(130);
    }
    if( is_mc_ )
      geant_kine_ = (HCategory*)HCategoryManager::getCategory(catGeantKine);
    n_events_=loop_.getEntries();
  }
  void AddGeantFiles( std::string geant_files ){
    if( geant_files.empty() )
      return;
    geant_chain_ = new TChain( "T" );
    std::stringstream list{geant_files};
    std::string file{};
    while(std::getline(list,file,',')){
      if( file=="" )
        continue;
      geant_chain_->Add(file.c_str());
      std::cout << file << " has been added to sequence" << std::endl;
    }
    loop_.getChain()->AddFriend(geant_chain_, "TG");
    loop_.printCategories();
    loop_.printChain();
  }
  void InitEvtChara( const std::string& parameter_file ){
    evt_chara_bk_.setParameterFile(parameter_file.data());
    evt_chara_bk_.init();
  }
  void SwitchNextEvent(){
    if( !Eof() )
      read_bytes = loop_.nextEvent( position_ );
    position_++;
  }
  void SetSzymonFile(const std::string &szymon_file);
  void SetSystem(const std::string &system) { system_ = system; }
  void SetEnergy(float energy) { energy_ = energy; }
  bool Eof(long long events=0) const{
    if( events > 0 )
      return (position_ >= n_events_ || read_bytes <= 0 || position_ > events );
    else
      return (position_ >= n_events_ || read_bytes <= 0 );
  }
  void ReadEvent();
  void ReadParticleCandidates();
  void ReadWallHits();
  void ReadStartCals();
  void ReadSimData();
  int GetPdgOfNuclei( int geant_code );
private:
  HLoop loop_;
  TChain* geant_chain_{nullptr};
  int read_bytes{1};
  long long n_events_{0};
  long long position_{0};
  bool is_mc_{false};
  TFile* szymon_file_;
  std::string system_;
  float energy_;
  HParticleEvtChara evt_chara_bk_;
  HEnergyLossCorrPar dE_dx_corr_;
  HCategory* particle_category_{nullptr};
  HCategory* event_info_category_{nullptr};
  HCategory* wall_category_{nullptr};
  HCategory* start2hit_category_{nullptr};
  HCategory* start2cal_category_{nullptr};
  HCategory* geant_kine_{nullptr};
  HParticleEvtInfo*hades_event_info_{nullptr};
  HEventHeader*hades_event_header_{nullptr};
};

#endif // HTREE_TO_AT_SRC_HADES_EVENT_READER_H_
