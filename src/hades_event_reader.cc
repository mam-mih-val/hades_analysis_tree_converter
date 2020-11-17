//
// Created by mikhail on 5/23/20.
//

#include "hades_event_reader.h"
#include "HADES_constants.h"
#include "mhwalldivider.h"
#include <hwallhitsim.h>

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
  auto analysis_event_manager = Analysis::TreeManager::Instance()->GetEventManager();
  if(is_mc_)
    Analysis::TreeManager::Instance()->NewSimEvent();
  hades_event_info_ = HCategoryManager::getObject(hades_event_info_, event_info_category_, 0);
  hades_event_header_ = gHades->getCurrentEvent()->getHeader();
  HVertex vertex_reco = hades_event_header_->getVertexReco();
  for(auto trigger : triggers)
    analysis_event_manager->SetField(hades_event_info_->isGoodEvent(trigger), trigger);
  for(auto trigger : physical_triggers)
    analysis_event_manager->SetField(hades_event_header_->isTBit(trigger), trigger);
  analysis_event_manager->GetEventHeader()->SetVertexPosition3( {vertex_reco.getX(), vertex_reco.getY(), vertex_reco.getZ()} );
  analysis_event_manager->SetField(vertex_reco.getChi2(), Analysis::EventManager::VTX_CHI2);
  analysis_event_manager->SetField( (int)hades_event_header_->getId(), Analysis::EventManager::EVENT_ID);
  analysis_event_manager->SetField( (int)hades_event_header_->getEventRunNumber(), Analysis::EventManager::RUN_ID);
  for( auto estimator : centrality_estimators ) {
    analysis_event_manager->SetField((int)evt_chara_bk_.getCentralityEstimator(estimator), estimator);
    float percentile = evt_chara_bk_.getCentralityClass(estimator, HParticleEvtChara::k5)*5 - 2.5; // 5% centrality class
//    std::cout << percentile << std::endl;
    analysis_event_manager->SetField(percentile, estimator);
  }
  auto start2hit = (HStart2Hit*) start2hit_category_->getObject(0);
  analysis_event_manager->SetField( (int)start2hit->getModule(), Analysis::EventManager::START_MODULE);
  analysis_event_manager->SetField( (int)start2hit->getStrip(), Analysis::EventManager::START_STRIP);
  analysis_event_manager->SetField( (int)start2hit->getMultiplicity(), Analysis::EventManager::START_MULT);
  analysis_event_manager->SetField( (float)start2hit->getTime(), Analysis::EventManager::START_TIME);
  auto vz = vertex_reco.getZ();
  int target = -1;
  if (vz >= -63.0 && vz < -60.0) target = 0;
  if (vz >= -60.0 && vz < -56.5) target = 1;
  if (vz >= -56.5 && vz < -52.5) target = 2;
  if (vz >= -52.5 && vz < -49.5) target = 3;
  if (vz >= -49.5 && vz < -45.5) target = 4;
  if (vz >= -45.0 && vz < -42.0) target = 5;
  if (vz >= -42.0 && vz < -39.0) target = 6;
  if (vz >= -39.0 && vz < -35.5) target = 7;
  if (vz >= -35.5 && vz < -31.5) target = 8;
  if (vz >= -31.5 && vz < -28.0) target = 9;
  if (vz >= -28.0 && vz < -24.0) target = 10;
  if (vz >= -24.0 && vz < -21.5) target = 11;
  if (vz >= -21.5 && vz < -17.0) target = 12;
  if (vz >= -17.0 && vz < -13.0) target = 13;
  if (vz >= -13.0) target = 14;
  analysis_event_manager->SetField( (int) target, Analysis::EventManager::TARGER_SEGMENT);
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
  HWallHitSim* hades_wall_hit{nullptr};
  MHWallDivider divider;
  auto analysis_wall_hit_manager =
      Analysis::TreeManager::Instance()->GetWallHitsManager();
  for(int i=0; i<n_wall_hits; ++i){
    hades_wall_hit = HCategoryManager::getObject(hades_wall_hit, wall_category_, i);
    int module_id = hades_wall_hit->getCell();
    float hit_x, hit_y, hit_z;
    hades_wall_hit->getXYZLab(hit_x, hit_y, hit_z);
    float hit_time = hades_wall_hit->getTime();
    float hit_distance = hades_wall_hit->getDistance();
    float hit_beta = hit_distance / hit_time / 299.792458;
    float signal = hades_wall_hit->getCharge();
    float p_signal =  93.f * pow(signal, 0.46 - 0.006 * sqrt(signal));
    int charge_z = evt_chara_bk_.getFWCharge(hades_wall_hit);
    int ring = divider.GetRing(module_id);
    if(ring == 0){
      std::cerr << "Error in short MHWallDivider::GetRing(short i=" << module_id << "): it returned 0" << std::endl;
      continue;
    }
    bool has_passed_cuts = evt_chara_bk_.PassesCutsFW(hades_wall_hit);
    Analysis::TreeManager::Instance()->NewWallModule();
    analysis_wall_hit_manager->SetPosition(hit_x, hit_y, hit_z);
    analysis_wall_hit_manager->SetSignal(signal);
    analysis_wall_hit_manager->SetField(hit_time, Analysis::WallHitsManager::TIME);
    analysis_wall_hit_manager->SetField(hit_distance, Analysis::WallHitsManager::DISTANCE);
    analysis_wall_hit_manager->SetField(hit_beta, Analysis::WallHitsManager::BETA);
    analysis_wall_hit_manager->SetField(p_signal, Analysis::WallHitsManager::P_CHARGE);
    analysis_wall_hit_manager->SetField(charge_z, Analysis::WallHitsManager::D_CHARGE);
    analysis_wall_hit_manager->SetField(ring, Analysis::WallHitsManager::RING);
    analysis_wall_hit_manager->SetField(module_id, Analysis::WallHitsManager::MODULE_ID);
    analysis_wall_hit_manager->SetField(has_passed_cuts, Analysis::WallHitsManager::PASSED_CUTS);
    analysis_wall_hit_manager->SetField(module_sub.at(i), Analysis::WallHitsManager::RND_SUB);
  }
}

void HadesEventReader::ReadParticleCandidates(){
  HParticleCandSim* hades_candidate{nullptr};
  int n_candidates = (int) particle_category_->getEntries();
  HVertex vertex_reco = hades_event_header_->getVertexReco();
  auto analysis_track_manager =
      Analysis::TreeManager::Instance()->GetMdcTracksManager();
  auto analysis_meta_hit_manager =
      Analysis::TreeManager::Instance()->GetMetaHitsManager();
  auto analysis_mdc_meta_matching =
      Analysis::TreeManager::Instance()->GetMdcMetaMatching();
  for( int i=0; i<n_candidates; ++i ){
    hades_candidate = HCategoryManager::getObject(hades_candidate, particle_category_, i);
    if(!hades_candidate)
      continue;
    if(!loop_.goodSector(hades_candidate->getSector()))
      continue; // skip inactive sectors
    if(!hades_candidate->isFlagBit(kIsUsed))
      continue;
    if(hades_candidate->getMomentum() == hades_candidate->getMomentumOrg())
      continue; // skip tracks with too high pt ???
    Analysis::TreeManager::Instance()->NewTrack();
    int pid_code = hades_candidate->getPID();
    float p, theta, pt, phi, mass;
    TLorentzVector energy_momentum;
    if( pid_code >= 0 ){
      mass = HPhysicsConstants::mass(pid_code);
      p = hades_candidate->getCorrectedMomentumPID(pid_code);
      hades_candidate->setMomentum(p);                   // write it back
      hades_candidate->calc4vectorProperties(mass);      // sync with lorentz vector
    }
    else{
      mass = hades_candidate->getMass(); // META mass
      p = hades_candidate->getMomentum();
    }
    mass = hades_candidate->getMass(); // META mass
    p = hades_candidate->getMomentum();
    theta = hades_candidate->getTheta() * TMath::DegToRad();
    phi = hades_candidate->getPhi() * TMath::DegToRad();
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
      if( !hades_candidate->getLayer(0, i) )
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
    analysis_track_manager->SetMomentum(momentum);
    analysis_track_manager->SetMass(mass);
    analysis_track_manager->SetPdgCode( TDatabasePDG::Instance()->ConvertGeant3ToPdg(pid_code) );
    analysis_track_manager->SetField(
        (int)hades_candidate->getCharge(), Analysis::MdcTracksManager::CHARGE);
    analysis_track_manager->SetField(
        (float)hades_candidate->getChi2(), Analysis::MdcTracksManager::CHI2);
    analysis_track_manager->SetField(
        (int)hades_candidate->getNLayer(0), Analysis::MdcTracksManager::N_HITS_0);
    analysis_track_manager->SetField(
        (int)hades_candidate->getNLayer(1), Analysis::MdcTracksManager::N_HITS_1);
    analysis_track_manager->SetField(
        (int)hades_candidate->getNLayer(2), Analysis::MdcTracksManager::N_HITS_2);

    analysis_track_manager->SetField( (int) layers_0, Analysis::MdcTracksManager::LAYERS_0);
    analysis_track_manager->SetField( (int) layers_1, Analysis::MdcTracksManager::LAYERS_1);
    analysis_track_manager->SetField( (int) layers_2, Analysis::MdcTracksManager::LAYERS_2);
    analysis_track_manager->SetField( (int) layers_3, Analysis::MdcTracksManager::LAYERS_3);
    analysis_track_manager->SetField( (int) layers_total, Analysis::MdcTracksManager::LAYERS_TOTAL);
    analysis_track_manager->SetField( (int) layers_bits, Analysis::MdcTracksManager::LAYERS_BITS);

    analysis_track_manager->SetField(
        (float)hades_candidate->getMdcdEdx(), Analysis::MdcTracksManager::DE_DX);
    analysis_track_manager->SetField(
        (float)hades_candidate->getR(), Analysis::MdcTracksManager::DCA_XY);
    float dca_z = hades_candidate->getZ()-vertex_reco.getZ();
    analysis_track_manager->SetField(
        (float) dca_z, Analysis::MdcTracksManager::DCA_Z);
    analysis_track_manager->SetField(
        (int) pid_code, Analysis::MdcTracksManager::GEANT_PID);
// META FILLING
    analysis_meta_hit_manager->SetField(
        (int)hades_candidate->getCharge(), Analysis::MetaHitsManager::CHARGE);
    if(hades_candidate->getSystem() == 0) {
      analysis_meta_hit_manager->SetField(
          true, Analysis::MetaHitsManager::IS_RPC_HIT);
      analysis_meta_hit_manager->SetField(
          false, Analysis::MetaHitsManager::IS_TOF_HIT);
    } else{
      analysis_meta_hit_manager->SetField(
          false, Analysis::MetaHitsManager::IS_RPC_HIT);
      analysis_meta_hit_manager->SetField(
          true, Analysis::MetaHitsManager::IS_TOF_HIT);
    }
    analysis_meta_hit_manager->SetField(
        (float)hades_candidate->getDistanceToMetaHit(), Analysis::MetaHitsManager::PATH_LENGTH);
    analysis_meta_hit_manager->SetField(
        (float)hades_candidate->getBeta(), Analysis::MetaHitsManager::BETA);
    analysis_meta_hit_manager->SetField(
        (float) (hades_candidate->getDistanceToMetaHit() /
                hades_candidate->getBeta() / 299.792458), Analysis::MetaHitsManager::TIME_OF_FLIGHT);
    analysis_meta_hit_manager->SetField(
        (float)hades_candidate->getMetaMatchRadius(), Analysis::MetaHitsManager::MATCH_RADIUS);
    analysis_meta_hit_manager->SetField(
        (float)hades_candidate->getMetaMatchQuality(), Analysis::MetaHitsManager::MATCH_QUALITY);
    analysis_meta_hit_manager->SetField(
        (float)hades_candidate->getTofdEdx(), Analysis::MetaHitsManager::DE_DX);
    analysis_meta_hit_manager->SetField(
        (float)hades_candidate->getMass2()/powf( 10, 6 ), Analysis::MetaHitsManager::MASS2); // Mev -> GeV
    analysis_mdc_meta_matching->Match( analysis_track_manager->GetTrackId(), analysis_meta_hit_manager->GetHitId() );
    if( is_mc_ ) {
      Analysis::TreeManager::Instance()->GetRecoSimMatching()->Match( analysis_track_manager->GetTrackId(),
          hades_candidate->getGeantTrack() - 1);
    }
  }
}

void HadesEventReader::ReadSimData(){
  HGeantHeader* hades_geant_header = loop_.getGeantHeader();
  float beam_energy = hades_geant_header->getBeamEnergy() / 1.23f;
  float impact_parameter = hades_geant_header->getImpactParameter();
  float reaction_plane =
      hades_geant_header->getEventPlane()*TMath::DegToRad();
  int geant_event_id = hades_geant_header->getEventId();
  HGeantKine*hades_geant_track{nullptr};
  bool is_set_vertex{false};
  float vx, vy, vz;
  int k=0;
  while(!is_set_vertex){
    hades_geant_track = HCategoryManager::getObject(hades_geant_track, geant_kine_, k);
    ++k;
    if( !hades_geant_track->isPrimary() )
      continue;
    hades_geant_track->getVertex(vx, vy, vz);
    is_set_vertex=true;
  }
  auto analysis_sim_header = Analysis::TreeManager::Instance()->GetSimEventManager();
  auto analysis_sim_tracks =
      Analysis::TreeManager::Instance()->GetSimTracksManager();
  auto reco_sim_matching =
      Analysis::TreeManager::Instance()->GetRecoSimMatching();
  analysis_sim_header->SetVertex(vx, vy, vz);
  analysis_sim_header->SetField(
      geant_event_id, Analysis::SimEventManager::GEANT_EVENT_ID);
  analysis_sim_header->SetField(
      beam_energy, Analysis::SimEventManager::BEAM_ENERGY);

  analysis_sim_header->SetField(
      impact_parameter, Analysis::SimEventManager::IMPACT_PARAMETER);
  analysis_sim_header->SetField(
      (float) reaction_plane, Analysis::SimEventManager::REACTION_PLANE);
  std::vector<int> selected_tracks;
  int n_rejected_electrons{0};
  std::map<int, int> track_id_position; // matching between selected particle track id and its position in detector
  int position{0};
  // loop over all gen particles

  for( int i=0; i<geant_kine_->getEntries(); ++i ){
    bool is_matched = false;
    int match_idx = reco_sim_matching->GetMatching()->GetMatchInverted(i);
    if( match_idx != AnalysisTree::UndefValueInt ){
      is_matched = true;
    }
    hades_geant_track = HCategoryManager::getObject(hades_geant_track, geant_kine_, i);
    if(hades_geant_track->getMechanism() != 0 &&
        hades_geant_track->getMechanism() != 5 ) {
      if( !is_matched )
        continue;
    }
    int parent_id = hades_geant_track->getParentTrack();
    bool is_parent_in_list = std::count( selected_tracks.begin(), selected_tracks.end(), parent_id ) > 0;
    if( !is_parent_in_list && !hades_geant_track->isPrimary() ){
      if( !is_matched )
        continue;
    }
    if(hades_geant_track->getID() == 3 ){ // selection of electron
      if( !is_matched ){
        n_rejected_electrons++;
        continue;
      }
    }
    selected_tracks.push_back(hades_geant_track->getTrack() );
    float pt = hades_geant_track->getTransverseMomentum() / 1000.; // MeV->GeV
    float theta = hades_geant_track->getThetaDeg()*TMath::DegToRad();
    float phi = hades_geant_track->getPhiDeg()*TMath::DegToRad();
    float mass = hades_geant_track->getM()/1000.; // MeV->GeV
    int pid = hades_geant_track->getID();
    bool is_primary = hades_geant_track->isPrimary();
    TVector3 p; p.SetPtThetaPhi(pt, theta, phi);
    Analysis::TreeManager::Instance()->NewSimTrack();
    analysis_sim_tracks->SetMomentum(p);
    analysis_sim_tracks->SetMass(mass);
    analysis_sim_tracks->SetPdgCode( TDatabasePDG::Instance()->ConvertGeant3ToPdg(pid) );
    analysis_sim_tracks->SetField(
        is_primary,Analysis::SimTracksManager::IS_PRIMARY);
    analysis_sim_tracks->SetField(
        pid,Analysis::SimTracksManager::GEANT_PID);
    analysis_sim_tracks->SetField(
        hades_geant_track->getTrack(),Analysis::SimTracksManager::GEANT_TRACK_ID);
    analysis_sim_tracks->SetField(
        hades_geant_track->getParentTrack(),Analysis::SimTracksManager::GEANT_PARENT_ID);
    analysis_sim_tracks->SetField(
        hades_geant_track->getMechanism(),Analysis::SimTracksManager::GEANT_PROCESS_ID);
    analysis_sim_tracks->SetField(
        hades_geant_track->getMedium(),Analysis::SimTracksManager::GEANT_MEDIUM_NUMBER);
    float vtx_x, vtx_y, vtx_z;
    hades_geant_track->getVertex(vtx_x, vtx_y, vtx_z);
    analysis_sim_tracks->SetField(vtx_x,Analysis::SimTracksManager::VTX_X);
    analysis_sim_tracks->SetField(vtx_y,Analysis::SimTracksManager::VTX_Y);
    analysis_sim_tracks->SetField(vtx_z,Analysis::SimTracksManager::VTX_Z);
    track_id_position.insert({hades_geant_track->getTrack(), position});
    position++;
  }

  analysis_sim_header->SetField(
      n_rejected_electrons, Analysis::SimEventManager::REJECTED_ELECTRONS);

  // matching reco and gen tracks again due to skipped gen tracks
  HParticleCandSim* hades_candidate{nullptr};
  int n_candidates = (int) particle_category_->getEntries();
  reco_sim_matching->ClearMatching();
  int reco_position=0;
  for( int i=0; i<n_candidates; ++i ) {
    hades_candidate = HCategoryManager::getObject(hades_candidate, particle_category_, i);
    if (!hades_candidate)
      continue;
    if (!loop_.goodSector(hades_candidate->getSector()))
      continue; // skip inactive sectors
    if (!hades_candidate->isFlagBit(kIsUsed))
      continue;
    if (hades_candidate->getMomentum() == hades_candidate->getMomentumOrg())
      continue; // skip tracks with too high pt ???
    int geant_track_id = hades_candidate->getGeantTrack();
    try {
      reco_sim_matching->Match(
          reco_position, track_id_position.at(geant_track_id));
    }catch(std::out_of_range&){ std::cout << "reco track not matched with sim track" << std::endl; }
    reco_position++;
  }
}