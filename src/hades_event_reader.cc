//
// Created by mikhail on 5/23/20.
//

#include "hades_event_reader.h"
#include "mhwalldivider.h"

void HadesEventReader::ReadEvent(){
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
  ReadWallHits();
  ReadParticleCandidates();
}

void HadesEventReader::ReadWallHits(){
  int n_wall_hits = wall_category_->getEntries();
  if( n_wall_hits <= 0 )
    return;
  HWallHitSim* wall_hit = 0;
  MHWallDivider divider;
  for(int i=0; i<n_wall_hits; ++i){
    wall_hit = HCategoryManager::getObject(wall_hit, wall_category_, i);
    int module_id = wall_hit->getCell();
    float hit_x, hit_y, hit_z;
    wall_hit->getXYZLab(hit_x, hit_y, hit_z);
    float hit_time = wall_hit->getTime();
    float hit_distance = wall_hit->getDistance();
    float hit_beta = hit_distance / hit_time / 299.792458;
    float signal = wall_hit->getCharge();
    float p_signal =  93. * pow(signal, 0.46 - 0.006 * sqrt(signal));
    int charge_z = evtChara.getFWCharge(wall_hit);
    int ring = divider.GetRing(module_id);
    if(ring == 0)
    {
      std::cerr << "Error in short MHWallDivider::GetRing(short i=" << module_id << "): it returned 0" << std::endl;
      continue;
    }
    bool has_passed_cuts = evtChara.PassesCutsFW(wall_hit);
    Analysis::TreeManager::Instance()->NewWallModule();
    Analysis::WallHitsManager::Instance()->SetPosition(hit_x, hit_y, hit_z);
    Analysis::WallHitsManager::Instance()->SetSignal(signal);
    Analysis::WallHitsManager::Instance()->SetField(
        hit_time, Analysis::WallHitsManager::TIME);
    Analysis::WallHitsManager::Instance()->SetField(
        hit_distance, Analysis::WallHitsManager::DISTANCE);
    Analysis::WallHitsManager::Instance()->SetField(
        hit_beta, Analysis::WallHitsManager::BETA);
    Analysis::WallHitsManager::Instance()->SetField(
        p_signal, Analysis::WallHitsManager::P_CHARGE);
    Analysis::WallHitsManager::Instance()->SetField(
        charge_z, Analysis::WallHitsManager::D_CHARGE);
    Analysis::WallHitsManager::Instance()->SetField(
        ring, Analysis::WallHitsManager::RING);
    Analysis::WallHitsManager::Instance()->SetField(
        ring, Analysis::WallHitsManager::RING);
    Analysis::WallHitsManager::Instance()->SetField(
        module_id, Analysis::WallHitsManager::MODULE_ID);
    Analysis::WallHitsManager::Instance()->SetField(
        has_passed_cuts, Analysis::WallHitsManager::PASSED_CUTS);
  }
}

void HadesEventReader::ReadParticleCandidates(){
  HParticleCand* candidate{nullptr};
  int n_candidates = (int) particle_category_->getEntries();
//    Analysis::TreeManager::Instance()->ReserveTracks(n_candidates);
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