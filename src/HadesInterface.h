#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "TFile.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"

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

#include "AnalysisTree/Detector.h"
#include "AnalysisTree/EventHeader.h"
#include "AnalysisTree/Track.h"
#include "AnalysisTree/StaticInfo.h"
#include "AnalysisTree/Configuration.h"

#include "HADES_constants.h"
#include "hparticleevtcharaBK.h"
#include "mhwalldivider.h"

namespace Analysis
{

class HadesInterface
{
    
public:
  
    HadesInterface();
    ~HadesInterface(){};
    
    void Init();
    void Exec();
    void Finish();

    void SetOutputFile(std::string filename) { fOutputFileName = filename; }
    void SetEvtCharaParameterFile(std::string fileName="../evtchara07/centrality_epcorr_apr12_gen8_2019_02_pass30.root") { fParameterFile = fileName; }

private:

    void InitInput();
    void InitOutput();
    void ReadEvent();
    
    void ReadVertexTracks();
    void ReadFw();
	void ReadStaticInfo();
    void ReadEventInfo();
    void ReadTof();
        
    std::string fOutputFileName {""};
    std::string fInputFileName{""};
    TFile *fTreeFile {nullptr};
    TFile *fInputFile {nullptr};
    TTree *fATree {nullptr};
    
    std::map <int, int> fTriggerMap{};
    std::map <int, int> fCentralityEstimatorMap{};
    
    AnalysisTree::Configuration fConfig;    
    AnalysisTree::StaticInfo fStaticInfo;
    AnalysisTree::EventHeader *fEventHeader {nullptr};
    AnalysisTree::TrackDetector *fVtxTracks {nullptr};
    AnalysisTree::HitDetector *fTofHits {nullptr};
    AnalysisTree::HitDetector *fFwHits {nullptr};
    
    AnalysisTree::Matching *fVtx2Tof{nullptr};

	std::map<int, int> fCEmapBK = { 
		{ HADES_constants::kNhitsTOF, HParticleEvtCharaBK::kTOFtot },
		{ HADES_constants::kNhitsTOF_cut, HParticleEvtCharaBK::kTOF },
		{ HADES_constants::kNhitsRPC, HParticleEvtCharaBK::kRPCtot },
		{ HADES_constants::kNhitsRPC_cut, HParticleEvtCharaBK::kRPC },
		{ HADES_constants::kNhitsTOF_RPC, HParticleEvtCharaBK::kTOFRPCtot },
		{ HADES_constants::kNhitsTOF_RPC_cut, HParticleEvtCharaBK::kTOFRPC },
		{ HADES_constants::kNtracks, HParticleEvtCharaBK::kPrimaryParticleCand },
		{ HADES_constants::kNselectedTracks, HParticleEvtCharaBK::kSelectedParticleCand },
		{ HADES_constants::kFWSumChargeSpec, HParticleEvtCharaBK::kFWSumChargeSpec },
		{ HADES_constants::kFWSumChargeZ, HParticleEvtCharaBK::kFWSumChargeZ } 
	};

	std::map<int, std::string> fCEmapNames = {
		{HADES_constants::kNhitsTOF, "tof_hits"},
		{HADES_constants::kNhitsTOF_cut, "selected_tof_hits"},
		{HADES_constants::kNhitsRPC, "rpc_hits"},
		{HADES_constants::kNhitsRPC_cut, "selected_rpc_hits"},
		{HADES_constants::kNhitsTOF_RPC, "tof_rpc_hits"},
		{HADES_constants::kNhitsTOF_RPC_cut, "selected_tof_rpc_hits"},
		{HADES_constants::kNtracks, "mdc_tracks"},
		{HADES_constants::kNselectedTracks, "selected_mdc_tracks"},
		{HADES_constants::kFWSumChargeSpec, "fw_adc"},
		{HADES_constants::kFWSumChargeZ, "fw_charge"},
	};

	HParticleEvtCharaBK fEvtChara;
	HEnergyLossCorrPar dEdxCorr;
	HCategory* candCat;
  	HCategory* evtInfoCat;
  	HCategory* wallCat;
	MHWallDivider* divider; 
	HParticleEvtInfo* evtInfo;
	HEventHeader* fHeader;
	std::string fParameterFile;
};

}
