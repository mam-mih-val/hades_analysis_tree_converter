//
// Created by mikhail on 5/23/20.
//

#ifndef HTREE_TO_AT_SRC_TRACK_READER_H_
#define HTREE_TO_AT_SRC_TRACK_READER_H_

namespace Analysis {
class TrackManager {
public:
  enum track_fields_float{
    CHI2=0,
    VTX_CHI2,
    DE_DX,
    DCA_XY,
    DCA_Z,
    NUMBER_FIELDS_FLOAT
  };
  enum track_fields_int{
    CHARGE=0,
    N_HITS_0,
    N_HITS_1,
    N_HITS_2,
    GEANT_PID,
    NUMBER_FIELDS_INT
  };
  TrackManager() = default;
  ~TrackManager() = default;
  AnalysisTree::TrackDetector *
  CreateTrackDetector(AnalysisTree::Configuration &config,
                     std::string branch_name = "mdc_vtx_tracks") {
    AnalysisTree::BranchConfig vtx_tracks_branch(branchName, AnalysisTree::DetType::kTrack);
    vtx_tracks_branch.AddField<float>("chi2");
    vtx_tracks_branch.AddField<float>("vtx_chi2");
    vtx_tracks_branch.AddField<float>("dEdx");
    vtx_tracks_branch.AddField<float>("dca_xy");
    vtx_tracks_branch.AddField<float>("dca_z");
    vtx_tracks_branch.AddField<int>("charge");
    vtx_tracks_branch.AddField<int>("nhits_0");
    vtx_tracks_branch.AddField<int>("nhits_1");
    vtx_tracks_branch.AddField<int>("nhits_2");
    vtx_tracks_branch.AddField<int>("geant_pid");

    auto vtx_tracks = new AnalysisTree::TrackDetector(config.GetLastId());
    id_fields_float_.insert( CHI2, vtx_tracks_branch.GetFieldId("chi2") );
    id_fields_float_.insert( VTX_CHI2, vtx_tracks_branch.GetFieldId("vtx_chi2") );
    id_fields_float_.insert( DE_DX, vtx_tracks_branch.GetFieldId("dEdx") );
    id_fields_float_.insert( DCA_XY, vtx_tracks_branch.GetFieldId("dca_xy") );
    id_fields_float_.insert( DCA_Z, vtx_tracks_branch.GetFieldId("dca_z") );

    id_fields_float_.insert( CHARGE, vtx_tracks_branch.GetFieldId("charge") );
    id_fields_float_.insert( N_HITS_0, vtx_tracks_branch.GetFieldId("nhits_0") );
    id_fields_float_.insert( N_HITS_1, vtx_tracks_branch.GetFieldId("nhits_1") );
    id_fields_float_.insert( N_HITS_2, vtx_tracks_branch.GetFieldId("nhits_2") );
    id_fields_float_.insert( GEANT_PID, vtx_tracks_branch.GetFieldId("geant_pid") );

    config.AddBranchConfig(vtx_tracks_branch);
    return vtxTracks;
  }
private:
  std::map<int, int> id_fields_int_;
  std::map<int, int> id_fields_float_;

};
} // namespace Analysis
#endif // HTREE_TO_AT_SRC_TRACK_READER_H_
