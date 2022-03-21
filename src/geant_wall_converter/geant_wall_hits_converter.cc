//
// Created by mikhail on 2/11/21.
//

#include "geant_wall_hits_converter.h"
#include <TRandom.h>
#include <hcategorymanager.h>
#include <hgeantwall.h>
#include <hparticledef.h>
#include <src/mhwalldivider.h>

void GeantWallHitsConverter::InitOutput(AnalysisTree::Configuration &config,
                                        TTree *tree) {
  branch_config_ = AnalysisTree::BranchConfig("reconstructed_forward_wall_hits",
                                              AnalysisTree::DetType::kHit);
  branch_config_.AddField<int>("ring");
  branch_config_.AddField<int>("module_id");
  branch_config_.AddField<int>("rnd_sub");
  branch_config_.AddField<float>("beta");
  branch_config_.AddField<float>("time");
  branch_config_.AddField<float>("distance");
  branch_config_.AddField<float>("parametrized_charge");
  branch_config_.AddField<int>("discrete_charge");
  branch_config_.AddField<bool>("has_passed_cuts");

  config.AddBranchConfig(branch_config_);
  auto& branch_configs = config.GetBranchConfigs();
  branch_configs.front().SetId(100+config.GetNumberOfBranches());
  wall_hits_ = new AnalysisTree::HitDetector(100+config.GetNumberOfBranches());
  tree->Branch("reconstructed_forward_wall_hits", "AnalysisTree::HitDetector",
               &wall_hits_);
  this->InitWallCellsPositions();
}
void GeantWallHitsConverter::InitInput() {
  geant_wall_hits_category_ =
      (HCategory *)HCategoryManager::getCategory(catWallGeantRaw);
}
void GeantWallHitsConverter::Convert() {
  wall_hits_->ClearChannels();
  auto ring_id = branch_config_.GetFieldId("ring");
  auto module_id = branch_config_.GetFieldId("module_id");
  auto beta_id = branch_config_.GetFieldId("beta");
  auto time_id = branch_config_.GetFieldId("time");
  auto distance = branch_config_.GetFieldId("distance");
  int n_wall_hits = geant_wall_hits_category_->getEntries();
  std::vector<int> module_ids;
  if (n_wall_hits <= 0)
    return;
  for (int i = 0; i < n_wall_hits; ++i)
    module_ids.emplace_back(i);
  std::random_shuffle(module_ids.begin(), module_ids.end());
  std::map<int, int> module_sub;
  for (int i = 0; i < module_ids.size(); ++i) {
    module_sub.insert(std::make_pair(module_ids.at(i), i % 2));
  }
  HGeantWall *wall_hit{nullptr};
  MHWallDivider divider;
  std::array<float, 304> sum_energy{};
  std::array<float, 304> tof_modules{};
  std::array<float, 304> len_modules{};

  for (int i = 0; i < n_wall_hits; ++i) {
    wall_hit =
        HCategoryManager::getObject(wall_hit, geant_wall_hits_category_, i);
    Float_t x, y, p, len, energy, tof;
    wall_hit->getHit(energy, x, y, tof, p, len);
    if (23.5 > tof || tof > 27.5)
      continue;
    auto cell = wall_hit->getCell();
    auto amplitude_resolution = energy * relative_amplitude_resolution;
    energy = gRandom->Gaus(energy, amplitude_resolution);
    sum_energy.at(cell) += energy;
    tof_modules.at(cell) = tof;
    len_modules.at(cell) = len;
  }
  for (int i = 0; i < 304; ++i) {
    auto sum = sum_energy.at(i);
    auto new_hit = wall_hits_->AddChannel();
    new_hit->Init(branch_config_);
    sum = TMath::Sqrt(sum) * 43.49; // botvina
    if (sum < 56)
      continue;
    if (i + 1 <= 144) {
      if (sum < 400)
        sum = 159.25 * TMath::Log(0.0179 * sum);
      else
        sum = 155.0 + 0.3981 * sum;
    } else if (i + 1 > 144 && i + 1 <= 208) {
      if (sum < 400)
        sum = 145.5 * TMath::Log(0.02 * sum);
      else
        sum = 157.7 + 0.3636 * sum;
    } else {
      if (sum < 200)
        sum = 151.987 * TMath::Log(0.019155 * sum);
      else
        sum = 52.155 + 0.759936 * sum;
    }
    auto x = x_wall_modules_positions_.at(i);
    auto y = y_wall_modules_positions_.at(i);
    auto z = z_wall_modules_positions_.at(i);
    new_hit->SetPosition(x,y,z);
    new_hit->SetSignal(sum);
    new_hit->SetField(i, module_id);
    new_hit->SetField(tof_modules.at(i), time_id);
    float beta = len_modules.at(i) / tof_modules.at(i) / 299.792458;
    new_hit->SetField(beta, beta_id);
    int ring = divider.GetRing(i);
    new_hit->SetField(ring, ring_id);
  }
}
void GeantWallHitsConverter::InitWallCellsPositions() {
  x_wall_modules_positions_ = {
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   0,    0,    -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   0,    0,    -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      220,  180,  140,  100,  60,   20,   -20,  -60,  -100, -140, -180, -220,
      360,  280,  200,  120,  40,   -40,  -120, -200, -280, -360, 360,  280,
      200,  120,  40,   -40,  -120, -200, -280, -360, 360,  280,  -280, -360,
      360,  280,  -280, -360, 360,  280,  -280, -360, 360,  280,  -280, -360,
      360,  280,  -280, -360, 360,  280,  -280, -360, 360,  280,  200,  120,
      40,   -40,  -120, -200, -280, -360, 360,  280,  200,  120,  40,   -40,
      -120, -200, -280, -360, 0,    0,    480,  320,  160,  0,    -160, -320,
      -480, 0,    0,    0,    640,  480,  320,  160,  0,    -160, -320, -480,
      -640, 0,    800,  640,  480,  320,  160,  0,    -160, -320, -480, -640,
      -800, 800,  640,  480,  -480, -640, -800, 800,  640,  480,  -480, -640,
      -800, 800,  640,  480,  -480, -640, -800, 800,  640,  480,  -480, -640,
      -800, 800,  640,  480,  -480, -640, -800, 800,  640,  480,  320,  160,
      0,    -160, -320, -480, -640, -800, 0,    640,  480,  320,  160,  0,
      -160, -320, -480, -640, 0,    0,    0,    480,  320,  160,  0,    -160,
      -320, -480, 0,    0,
  };
  y_wall_modules_positions_ = {
      220,  220,  220,  220,  220,  220,  220,  220,  220,  220,  220,  220,
      180,  180,  180,  180,  180,  180,  180,  180,  180,  180,  180,  180,
      140,  140,  140,  140,  140,  140,  140,  140,  140,  140,  140,  140,
      100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,  100,
      60,   60,   60,   60,   60,   60,   60,   60,   60,   60,   60,   60,
      20,   20,   20,   20,   20,   0,    0,    20,   20,   20,   20,   20,
      -20,  -20,  -20,  -20,  -20,  0,    0,    -20,  -20,  -20,  -20,  -20,
      -60,  -60,  -60,  -60,  -60,  -60,  -60,  -60,  -60,  -60,  -60,  -60,
      -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100,
      -140, -140, -140, -140, -140, -140, -140, -140, -140, -140, -140, -140,
      -180, -180, -180, -180, -180, -180, -180, -180, -180, -180, -180, -180,
      -220, -220, -220, -220, -220, -220, -220, -220, -220, -220, -220, -220,
      360,  360,  360,  360,  360,  360,  360,  360,  360,  360,  280,  280,
      280,  280,  280,  280,  280,  280,  280,  280,  200,  200,  200,  200,
      120,  120,  120,  120,  40,   40,   40,   40,   -40,  -40,  -40,  -40,
      -120, -120, -120, -120, -200, -200, -200, -200, -280, -280, -280, -280,
      -280, -280, -280, -280, -280, -280, -360, -360, -360, -360, -360, -360,
      -360, -360, -360, -360, 0,    0,    800,  800,  800,  800,  800,  800,
      800,  0,    0,    0,    640,  640,  640,  640,  640,  640,  640,  640,
      640,  0,    480,  480,  480,  480,  480,  480,  480,  480,  480,  480,
      480,  320,  320,  320,  320,  320,  320,  160,  160,  160,  160,  160,
      160,  0,    0,    0,    0,    0,    0,    -160, -160, -160, -160, -160,
      -160, -320, -320, -320, -320, -320, -320, -480, -480, -480, -480, -480,
      -480, -480, -480, -480, -480, -480, 0,    -640, -640, -640, -640, -640,
      -640, -640, -640, -640, 0,    0,    0,    -800, -800, -800, -800, -800,
      -800, -800, 0,    0,
  };
  z_wall_modules_positions_ = {
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 0,    0,    6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 0,    0,    6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 0,    0,    6800, 6800, 6800, 6800, 6800, 6800,
      6800, 0,    0,    0,    6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 0,    6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 6800, 6800, 0,    6800, 6800, 6800, 6800, 6800,
      6800, 6800, 6800, 6800, 0,    0,    0,    6800, 6800, 6800, 6800, 6800,
      6800, 6800, 0,    0,
  };
}
