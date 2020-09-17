//
// Created by mikhail on 5/23/20.
//

#include "hades_event_reader.h"
#include "mhwalldivider.h"
#include "HADES_constants.h"

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
  std::vector<int> centrality_estimators{
      HADES_constants::kTOFtot,
      HADES_constants::kTOF,
      HADES_constants::kRPCtot,
      HADES_constants::kRPC,
      HADES_constants::kTOFRPCtot,
      HADES_constants::kTOFRPC,
      HADES_constants::kPrimaryParticleCand,
      HADES_constants::kSelectedParticleCand,
      HADES_constants::kFWSumChargeSpec,
      HADES_constants::kFWSumChargeZ,
  };
  std::vector<int> physical_triggers{11, 12, 13};
  Analysis::TreeManager::Instance()->NewEvent();
  if(is_mc_)
    Analysis::TreeManager::Instance()->NewSimEvent();
  event_info_ = HCategoryManager::getObject(event_info_, event_info_category_, 0);
  event_header_ = gHades->getCurrentEvent()->getHeader();
  HVertex vertex_reco = event_header_->getVertexReco();
  for(auto trigger : triggers)
    Analysis::EventManager::Instance()->SetField(event_info_->isGoodEvent(trigger), trigger);
  for(auto trigger : physical_triggers)
    Analysis::EventManager::Instance()->SetField(event_header_->isTBit(trigger), trigger);
  Analysis::EventManager::Instance()->GetEventHeader()->SetVertexPosition3( {vertex_reco.getX(), vertex_reco.getY(), vertex_reco.getZ()} );
  Analysis::EventManager::Instance()->SetField(vertex_reco.getChi2(), Analysis::EventManager::VTX_CHI2);
  for( auto estimator : centrality_estimators ) {
    Analysis::EventManager::Instance()->SetField(
        (int)evt_chara_bk_.getCentralityEstimator(estimator), estimator);
    Analysis::EventManager::Instance()->SetField(
        (float)evt_chara_bk_.getCentralityPercentile(estimator), estimator);
  }
  ReadWallHits();
  ReadParticleCandidates();
  if( is_mc_ )
    ReadSimData();
}

void HadesEventReader::ReadWallHits(){
  int n_wall_hits = wall_category_->getEntries();
  std::vector<int> module_ids;
  if( n_wall_hits <= 0 )
    return;
  for( int i=0; i<n_wall_hits; ++i )
    module_ids.emplace_back(i);
  std::random_shuffle( module_ids.begin(), module_ids.end() );
  std::map<int, int> module_sub;
  for( int i=0; i<module_ids.size(); ++i ){
    module_sub.insert(std::make_pair( module_ids.at(i), i%2 ));
  }
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
    int charge_z = evt_chara_bk_.getFWCharge(wall_hit);
    int ring = divider.GetRing(module_id);
    if(ring == 0)
    {
      std::cerr << "Error in short MHWallDivider::GetRing(short i=" << module_id << "): it returned 0" << std::endl;
      continue;
    }
    bool has_passed_cuts = evt_chara_bk_.PassesCutsFW(wall_hit);
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
    Analysis::WallHitsManager::Instance()->SetField(
        module_sub.at(i), Analysis::WallHitsManager::RND_SUB);
  }
}

void HadesEventReader::ReadParticleCandidates(){
  HParticleCandSim* candidate{nullptr};
  HGeantKine* kine{nullptr};
  int n_candidates = (int) particle_category_->getEntries();
  HVertex vertex_reco = event_header_->getVertexReco();
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
    float p, theta, pt, phi, mass;
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
    mass = candidate->getMass(); // META mass
    p = candidate->getMomentum();
    theta = candidate->getTheta() * TMath::DegToRad();
    phi = candidate->getPhi() * TMath::DegToRad();
    pt = p*TMath::Sin(theta) / 1000.0; // MeV->GeV
    mass/=1000.0; // MeV->GeV
    TVector3 momentum;
    momentum.SetPtThetaPhi(pt, theta, phi);
    int layers_0{0};
    int layers_1{0};
    int layers_2{0};
    int layers_3{0};
    int layers_total{0};
    int layers_bits{0x00};

    for( int i=0; i<24; ++i ){
      if( !candidate->getLayer(0, i) )
          continue;
      layers_bits |=  ( 0x01 << i );
      if( 0 <= i && i < 6 )
        layers_0++;
      if( 6 <= i && i < 12 )
        layers_1++;
      if( 12 <= i && i < 18 )
        layers_2++;
      if( 18 <= i && i < 24 )
        layers_3++;
      layers_total++;
    }
    Analysis::TrackManager::Instance()->SetMomentum(momentum);
    Analysis::TrackManager::Instance()->SetMass(mass);
    Analysis::TrackManager::Instance()->SetPdgCode( TDatabasePDG::Instance()->ConvertGeant3ToPdg(pid_code) );
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

    Analysis::TrackManager::Instance()->SetField( (int) layers_0, Analysis::TrackManager::LAYERS_0);
    Analysis::TrackManager::Instance()->SetField( (int) layers_1, Analysis::TrackManager::LAYERS_1);
    Analysis::TrackManager::Instance()->SetField( (int) layers_2, Analysis::TrackManager::LAYERS_2);
    Analysis::TrackManager::Instance()->SetField( (int) layers_3, Analysis::TrackManager::LAYERS_3);
    Analysis::TrackManager::Instance()->SetField( (int) layers_total, Analysis::TrackManager::LAYERS_TOTAL);
    Analysis::TrackManager::Instance()->SetField( (int) layers_bits, Analysis::TrackManager::LAYERS_BITS);

    Analysis::TrackManager::Instance()->SetField(
        (float) candidate->getMdcdEdx(), Analysis::TrackManager::DE_DX);
    Analysis::TrackManager::Instance()->SetField(
        (float) candidate->getR(), Analysis::TrackManager::DCA_XY);
    float dca_z = candidate->getZ()-vertex_reco.getZ();
    Analysis::TrackManager::Instance()->SetField(
        (float) dca_z, Analysis::TrackManager::DCA_Z);
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
        (float)candidate->getTofdEdx(), Analysis::HitManager::DE_DX);
    Analysis::HitManager::Instance()->SetField(
        (float)candidate->getMass2()/powf( 10, 6 ), Analysis::HitManager::MASS2); // Mev -> GeV
    Analysis::TrackTofMatch::Instance()->Match( Analysis::TrackManager::Instance()->GetTrackId(), Analysis::HitManager::Instance()->GetHitId() );
    if( is_mc_ ) {
      Analysis::SimRecoMatch::Instance()->Match( Analysis::TrackManager::Instance()->GetTrackId(),
                                                 candidate->getGeantTrack() - 1);
    }
  }
}

void HadesEventReader::ReadSimData(){
  HGeantHeader* header = loop_.getGeantHeader();
  float impact_parameter = header->getImpactParameter();
  float reaction_plane = header->getEventPlane()*TMath::DegToRad();
//  float reaction_plane = header->getEventPlane();
  HGeantKine* sim_track{nullptr};
  bool is_set_vertex{false};
  float vx, vy, vz;
  int k=0;
  while(!is_set_vertex){
    sim_track = HCategoryManager::getObject(sim_track, geant_kine_, k);
    ++k;
    if( !sim_track->isPrimary() )
      continue;
    sim_track->getVertex(vx, vy, vz);
    is_set_vertex=true;
  }
  Analysis::SimEventManager::Instance()->SetVertex(vx, vy, vz);
  Analysis::SimEventManager::Instance()->SetField(
      impact_parameter, Analysis::SimEventManager::IMPACT_PARAMETER);
  Analysis::SimEventManager::Instance()->SetField(
      (float) reaction_plane, Analysis::SimEventManager::REACTION_PLANE);
  std::vector<int> selected_tracks;
  for( int i=0; i<geant_kine_->getEntries(); ++i ){
    sim_track = HCategoryManager::getObject(sim_track, geant_kine_, i);
    int parent_track_id = sim_track->getParentTrack();
    bool parent_is_selected = std::count( selected_tracks.begin(), selected_tracks.end(), parent_track_id ) != 0;
    if( !sim_tracks->isPrimary() && !parent_is_selected )
      continue;
    selected_tracks.push_back( sim_track->getTrack() );
    float pt = sim_track->getTransverseMomentum() / 1000.; // MeV->GeV
    float theta = sim_track->getThetaDeg()*TMath::DegToRad();
    float phi = sim_track->getPhiDeg()*TMath::DegToRad();
    float mass = sim_track->getM()/1000.; // MeV->GeV
    int pid = sim_track->getID();
    bool is_primary = sim_track->isPrimary();
    TVector3 p; p.SetPtThetaPhi(pt, theta, phi);
    Analysis::TreeManager::Instance()->NewSimTrack();
    Analysis::SimTrackManager::Instance()->SetMomentum(p);
    Analysis::SimTrackManager::Instance()->SetMass(mass);
    Analysis::SimTrackManager::Instance()->SetPdgCode( TDatabasePDG::Instance()->ConvertGeant3ToPdg(pid) );
    Analysis::SimTrackManager::Instance()->SetField(
        is_primary,Analysis::SimTrackManager::IS_PRIMARY);
    Analysis::SimTrackManager::Instance()->SetField(
        pid,Analysis::SimTrackManager::GEANT_PID);
  }
}