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

#include "event_manager.h"
#include "tree_manager.h"

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
    if(!loop_.setInput("-*,+HParticleCand,+HParticleEvtInfo,+HWallHit"))
    {
      cerr << "READBACK: ERROR : cannot read input !" << endl;
      std::abort();
    }
    loop_.printCategories();
    loop_.printChain();
    dE_dx_corr_.setDefaultPar("apr12");
    particle_category_ = (HCategory*)HCategoryManager::getCategory(catParticleCand);
    event_info_category_ = (HCategory*)HCategoryManager::getCategory(catParticleEvtInfo);
    wall_category_ = (HCategory*)HCategoryManager::getCategory(catWallHit);

    n_events_=loop_.getEntries();
  }
  void InitEvtChara( std::string parameter_file ){
    evt_chara_bk_.setParameterFile(parameter_file.data());
    evt_chara_bk_.init();
  }
  void SwitchNextEvent(){
    if( !Eof() )
      loop_.nextEvent( position_ );
    position_++;
  }
  bool Eof(){
    return position_ >= n_events_;
  }
  void ReadEvent(){
    std::vector<int> triggers{
      Particle::kGoodVertexClust,
      Particle::kGoodVertexCand,
      Particle::kGoodSTART,
      Particle::kNoPileUpSTART,
      Particle::kNoPileUpMETA,
      Particle::kNoPileUpMDC,
      Particle::kNoFlashMDC,
      Particle::kGoodMDCMult,
      Particle::kGoodMDCMIPSMult,
      Particle::kGoodLepMult,
      Particle::kGoodTRIGGER,
      Particle::kGoodSTART2,
      Particle::kNoVETO,
      Particle::kGoodSTARTVETO,
      Particle::kGoodSTARTMETA,
    };
    std::vector<int> physical_triggers{11, 12, 13};
    std::vector<int> centrality_estimators{
      HParticleEvtCharaBK::kTOFtot,
      HParticleEvtCharaBK::kTOF,
      HParticleEvtCharaBK::kRPCtot,
      HParticleEvtCharaBK::kRPC,
      HParticleEvtCharaBK::kTOFRPCtot,
      HParticleEvtCharaBK::kTOFRPC,
      HParticleEvtCharaBK::kPrimaryParticleCand,
      HParticleEvtCharaBK::kSelectedParticleCand,
      HParticleEvtCharaBK::kFWSumChargeSpec,
      HParticleEvtCharaBK::kFWSumChargeZ
    };
    Analysis::TreeManager::Instance()->NewEvent();
    event_info_ = HCategoryManager::getObject(event_info_, event_info_category_, 0);
    event_header_ = gHades->getCurrentEvent()->getHeader();
    HVertex vertex_reco = event_header_->getVertexReco();
    for(auto trigger : triggers)
      Analysis::EventManager::Instance()->SetField(event_info_->isGoodEvent(trigger), trigger);
    for(auto trigger : physical_triggers)
      Analysis::EventManager::Instance()->SetField(event_header_->isTBit(trigger), trigger);
    Analysis::EventManager::Instance()->GetEventHeader()->SetVertexX( vertex_reco.getX() );
    Analysis::EventManager::Instance()->GetEventHeader()->SetVertexY( vertex_reco.getY() );
    Analysis::EventManager::Instance()->GetEventHeader()->SetVertexZ( vertex_reco.getZ() );
    Analysis::EventManager::Instance()->SetField(vertex_reco.getChi2(), Analysis::EventManager::VTX_CHI2);
    for( auto estimator : centrality_estimators )
      Analysis::EventManager::Instance()->SetField(
          (int) evt_chara_bk_.getCentralityEstimator(estimator), estimator);
    ReadParticleCandidates();
    Analysis::TreeManager::Instance()->WriteEvent();
  }
  void ReadParticleCandidates(){
    HParticleCand* candidate{nullptr};
    int n_candidates = (int) particle_category_->getEntries();
    for( int i=0; i<n_candidates; ++i ){
      candidate = HCategoryManager::getObject(candidate, particle_category_, i);
      if(!candidate)
        continue;
      if(!loop_.goodSector(candidate->getSector()))
        continue; // skip inactive sectors
      if(!candidate->isFlagBit(kIsUsed))
        continue;
      if(candidate->getMomentum() == candidate->getMomentumOrg())
        continue; // skip tracks with too high pt ???
      Analysis::TreeManager::Instance()->NewTrack();
      int pid_code = candidate->getPID();
      float p, theta, pt, eta, phi, mass;
      TLorentzVector energy_momentum;
      if( pid_code >= 0 ){
        mass = HPhysicsConstants::mass(pid_code);
        p = candidate->getCorrectedMomentumPID(pid_code);
        candidate->setMomentum(p);                   // write it back
        candidate->calc4vectorProperties(mass);      // sync with lorentz vector
      }
      else{
        mass = candidate->getMass(); // META mass
        p = candidate->getMomentum();
      }
      theta = candidate->getTheta() * TMath::DegToRad();
      phi = candidate->getPhi() * TMath::DegToRad();
      pt = p*TMath::Sin(theta);
      eta = -TMath::Log(TMath::Tan(theta / 2.));
      energy_momentum.SetPtEtaPhiM(pt, eta, phi, mass);
      TVector3 momentum;
      momentum.SetPtThetaPhi(pt, theta, phi);

      Analysis::TrackManager::Instance()->SetMomentum(momentum);
      Analysis::TrackManager::Instance()->SetMass(mass);
      Analysis::TrackManager::Instance()->SetField(
          (int) candidate->getCharge(), Analysis::TrackManager::CHARGE);
      Analysis::TrackManager::Instance()->SetField(
          (float) candidate->getChi2(), Analysis::TrackManager::CHI2);
      Analysis::TrackManager::Instance()->SetField(
          (int) candidate->getNLayer(0), Analysis::TrackManager::N_HITS_0);
      Analysis::TrackManager::Instance()->SetField(
          (int) candidate->getNLayer(1), Analysis::TrackManager::N_HITS_1);
      Analysis::TrackManager::Instance()->SetField(
          (int) candidate->getNLayer(2), Analysis::TrackManager::N_HITS_2);
      Analysis::TrackManager::Instance()->SetField(
          (float) candidate->getMdcdEdx(), Analysis::TrackManager::DE_DX);
      Analysis::TrackManager::Instance()->SetField(
          (float) candidate->getR(), Analysis::TrackManager::DCA_XY);
      Analysis::TrackManager::Instance()->SetField(
          (float) candidate->getZ(), Analysis::TrackManager::DCA_Z);
      Analysis::TrackManager::Instance()->SetField(
          (int) pid_code, Analysis::TrackManager::GEANT_PID);
// META FILLING
      Analysis::HitManager::Instance()->SetField(
          (int)candidate->getCharge(), Analysis::HitManager::CHARGE);
      if(candidate->getSystem() == 0) {
        Analysis::HitManager::Instance()->SetField(
            true, Analysis::HitManager::IS_RPC_HIT);
        Analysis::HitManager::Instance()->SetField(
            false, Analysis::HitManager::IS_TOF_HIT);
      } else{
        Analysis::HitManager::Instance()->SetField(
            false, Analysis::HitManager::IS_RPC_HIT);
        Analysis::HitManager::Instance()->SetField(
            true, Analysis::HitManager::IS_TOF_HIT);
      }
      Analysis::HitManager::Instance()->SetField(
          (float)candidate->getDistanceToMetaHit(), Analysis::HitManager::PATH_LENGTH);
      Analysis::HitManager::Instance()->SetField(
          (float)candidate->getBeta(), Analysis::HitManager::BETA);
      Analysis::HitManager::Instance()->SetField(
          (float) (candidate->getDistanceToMetaHit() / candidate->getBeta() / 299.792458), Analysis::HitManager::TIME_OF_FLIGHT);
      Analysis::HitManager::Instance()->SetField(
          (float)candidate->getMetaMatchRadius(), Analysis::HitManager::MATCH_RADIUS);
      Analysis::HitManager::Instance()->SetField(
          (float)candidate->getMetaMatchQuality(), Analysis::HitManager::MATCH_QUALITY);
      Analysis::HitManager::Instance()->SetField(
          (float)candidate->getMass2(), Analysis::HitManager::MASS2);
    }
  }

private:
  HLoop loop_;
  long long n_events_{0};
  long long position_{0};
  HParticleEvtCharaBK evt_chara_bk_;
  HEnergyLossCorrPar dE_dx_corr_;
  HCategory* particle_category_{nullptr};
  HCategory* event_info_category_{nullptr};
  HCategory* wall_category_{nullptr};

  HParticleEvtInfo* event_info_{nullptr};
  HEventHeader* event_header_{nullptr};
};

#endif // HTREE_TO_AT_SRC_HADES_EVENT_READER_H_
