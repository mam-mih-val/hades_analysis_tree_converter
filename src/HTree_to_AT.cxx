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

#include "TFile.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"

#include <iostream>
#include <map>

#include "AnalysisTree/Detector.h"
#include "AnalysisTree/EventHeader.h"
#include "AnalysisTree/Track.h"
#include "AnalysisTree/StaticInfo.h"
#include "AnalysisTree/Configuration.h"

#include "HADES_constants.h"
#include "hparticleevtcharaBK.h"
#include "mhwalldivider.h"

using namespace std;

const map<int, int> triggerMap = { { HADES_constants::kGoodVertexClust, Particle::kGoodVertexClust },
	{ HADES_constants::kGoodVertexCand, Particle::kGoodVertexCand },
	{ HADES_constants::kGoodSTART, Particle::kGoodSTART }, { HADES_constants::kNoPileUpSTART, Particle::kNoPileUpSTART },
	{ HADES_constants::kNoPileUpMETA, Particle::kNoPileUpMETA },
	{ HADES_constants::kNoPileUpMDC, Particle::kNoPileUpMDC }, { HADES_constants::kNoFlashMDC, Particle::kNoFlashMDC },
	{ HADES_constants::kGoodMDCMult, Particle::kGoodMDCMult },
	{ HADES_constants::kGoodMDCMIPSMult, Particle::kGoodMDCMIPSMult },
	{ HADES_constants::kGoodLepMult, Particle::kGoodLepMult }, { HADES_constants::kGoodTRIGGER, Particle::kGoodTRIGGER },
	{ HADES_constants::kGoodSTART2, Particle::kGoodSTART2 }, { HADES_constants::kNoVETO, Particle::kNoVETO },
	{ HADES_constants::kGoodSTARTVETO, Particle::kGoodSTARTVETO },
	{ HADES_constants::kGoodSTARTMETA, Particle::kGoodSTARTMETA }, { HADES_constants::kPT1, 11 },
	{ HADES_constants::kPT2, 12 }, { HADES_constants::kPT3, 13 } };

const map<int, int> centralityEstimatorMap = { { HADES_constants::kNhitsTOF, HParticleEvtCharaBK::kTOFtot },
	{ HADES_constants::kNhitsTOF_cut, HParticleEvtCharaBK::kTOF },
	{ HADES_constants::kNhitsRPC, HParticleEvtCharaBK::kRPCtot },
	{ HADES_constants::kNhitsRPC_cut, HParticleEvtCharaBK::kRPC },
	{ HADES_constants::kNhitsTOF_RPC, HParticleEvtCharaBK::kTOFRPCtot },
	{ HADES_constants::kNhitsTOF_RPC_cut, HParticleEvtCharaBK::kTOFRPC },
	{ HADES_constants::kNtracks, HParticleEvtCharaBK::kPrimaryParticleCand },
	{ HADES_constants::kNselectedTracks, HParticleEvtCharaBK::kSelectedParticleCand },
	{ HADES_constants::kFWSumChargeSpec, HParticleEvtCharaBK::kFWSumChargeSpec },
	{ HADES_constants::kFWSumChargeZ, HParticleEvtCharaBK::kFWSumChargeZ } };

const std::map<int, int> fCEmapBK = { 
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

const std::map<int, std::string> fCEmapNames = {
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

const Float_t D2R = TMath::DegToRad();

int HTree_to_AT(TString infileList = "/lustre/nyx/hades/dst/apr12/gen8/108/root/be1210816080601.hld_dst_apr12.root",
	TString outfile = "output.root",
	Int_t nEvents = -1,
	TString parameterFile = "../evtchara07/centrality_epcorr_apr12_gen8_2019_02_pass30.root")
{
	// create loop object and hades
	HLoop loop(kTRUE);
	// list of all files with working sectors
	// reading input files and declaring containers
	Bool_t ret = kFALSE;
	if(infileList.Contains(","))
		ret = loop.addMultFiles(infileList); // file1,file2,file3
	else
		ret = loop.addFiles(infileList); // myroot*.root

	if(ret == 0)
	{
		cerr << "READBACK: ERROR : cannot find inputfiles : " << infileList.Data() << endl;
		return 1;
	}	

	if(!loop.setInput("-*,+HParticleCand,+HParticleEvtInfo,+HWallHit"))
	{
		cerr << "READBACK: ERROR : cannot read input !" << endl;
		exit(1);
	}

	// configure event characterization class
	HParticleEvtCharaBK evtChara;
	evtChara.setParameterFile(parameterFile);
	evtChara.init();

	// read all categories
	loop.printCategories();
	loop.printChain();

	// parameters
	HEnergyLossCorrPar dEdxCorr;
	dEdxCorr.setDefaultPar("apr12");

	// input data
	HCategory* candCat = (HCategory*)HCategoryManager::getCategory(catParticleCand);
	HCategory* evtInfoCat = (HCategory*)HCategoryManager::getCategory(catParticleEvtInfo);
	HCategory* wallCat = (HCategory*)HCategoryManager::getCategory(catWallHit);

	// Time
	//    Int_t time;

	AnalysisTree::Configuration fConfig;    
    AnalysisTree::StaticInfo fStaticInfo;
    AnalysisTree::EventHeader *fEventHeader {nullptr};
    AnalysisTree::TrackDetector *fVtxTracks {nullptr};
    AnalysisTree::HitDetector *fTofHits {nullptr};
    AnalysisTree::HitDetector *fFwHits {nullptr};

	TFile* out = new TFile(outfile.Data(), "RECREATE");
	out->cd();
	auto fATree = new TTree("ATree", "Analysis Tree, HADES data");

	AnalysisTree::BranchConfig VtxTracksBranch("VtxTracks");
	VtxTracksBranch.SetType(AnalysisTree::DetType::kTrack);
	VtxTracksBranch.AddFloatField("chi2");
	VtxTracksBranch.AddFloatField("vtx_chi2");
	VtxTracksBranch.AddFloatField("dEdx");
	VtxTracksBranch.AddFloatField({"dcax", "dcay", "dcaz"});
	VtxTracksBranch.AddIntegerField("q");
	VtxTracksBranch.AddIntegerField( {"nhits_0","nhits_1","nhits_2"} );
	VtxTracksBranch.AddIntegerField("pid");
	VtxTracksBranch.AddIntegerField("pt");
	VtxTracksBranch.AddIntegerField("phi");
	fConfig.AddBranchConfig(VtxTracksBranch);
	fVtxTracks = new AnalysisTree::TrackDetector(fConfig.GetLastId());

	//  ***** TOF Hits *******

	AnalysisTree::BranchConfig TofHitsBranch("TofHits");
	TofHitsBranch.SetType(AnalysisTree::DetType::kHit);
	TofHitsBranch.AddIntegerField("status");
	TofHitsBranch.AddFloatField("time");
	TofHitsBranch.AddFloatField("path");
	TofHitsBranch.AddFloatField("meta_mdc_match");
	TofHitsBranch.AddIntegerField("charge");
	TofHitsBranch.AddFloatField("sqr_mass");
	TofHitsBranch.AddFloatField("sqr_mass_error");
	fConfig.AddBranchConfig(TofHitsBranch);
	fTofHits = new AnalysisTree::HitDetector(fConfig.GetLastId());

	//  ***** Forvard Wall *******

	AnalysisTree::BranchConfig FwHitsBranch("FwHits");
	FwHitsBranch.SetType(AnalysisTree::DetType::kHit);
	FwHitsBranch.AddFloatField("adc");
	FwHitsBranch.AddIntegerField("charge");
	FwHitsBranch.AddIntegerField("module_id");
	FwHitsBranch.AddFloatField("beta");
	FwHitsBranch.AddIntegerField("ring");
	FwHitsBranch.AddFloatField("time");
	fConfig.AddBranchConfig(FwHitsBranch);
	fFwHits = new AnalysisTree::HitDetector(fConfig.GetLastId());

	//  ***** EventHeader *******

	AnalysisTree::BranchConfig EventHeaderBranch("EventHeader");
	EventHeaderBranch.SetType(AnalysisTree::DetType::kEventHeader);
	EventHeaderBranch.AddFloatField("vtx_chi2");
	EventHeaderBranch.AddIntegerField("run_id");
	EventHeaderBranch.AddIntegerField("evt_id");
	for(auto centEst : fCEmapNames)
	{
		EventHeaderBranch.AddIntegerField( centEst.second ); // centrality estimator
		EventHeaderBranch.AddFloatField( "centrality_"+centEst.second ); // centrality class
	}
	fConfig.AddBranchConfig(EventHeaderBranch);
	fEventHeader = new AnalysisTree::EventHeader(fConfig.GetLastId());
	fEventHeader->Init(EventHeaderBranch);

	//  ***** List of Branches *******
	fATree->Branch("VtxTracks", "AnalysisTree::TrackDetector", &fVtxTracks);
	fATree->Branch("EventHeader", "AnalysisTree::EventHeader", &fEventHeader);
	fATree->Branch("FwModules", "AnalysisTree::HitDetector", &fFwHits);
	fATree->Branch("TofHits", "AnalysisTree::HitDetector", &fTofHits);
	fConfig.Write("Configuration");

	Int_t entries = loop.getEntries();
	if(nEvents < entries && nEvents > 0)
	entries = nEvents;
	TString filename;
	MHWallDivider* divider = new MHWallDivider();

	for(Int_t i = 1; i < 3; i++)
	{
		Int_t nbytes = loop.nextEvent(i); // get next event. categories will be cleared before
		if(nbytes <= 0)
		{
			cout << nbytes << endl; // last event reached
			break;
		}
		if(i % 5000 == 0)
			cout << "event " << i << endl;

		Int_t g, day, hour, minute;
		TString* be = new TString("be");

		if(loop.isNewFile(filename))
		{
			filename = HTime::stripFileName(filename, kTRUE, kFALSE);
			HTime::splitFileName(filename, *be, g, day, hour, minute, g, g, kFALSE);
		}
		//-------------------------------------------------
		// summary event info object
		HParticleEvtInfo* evtInfo = 0;
		evtInfo = HCategoryManager::getObject(evtInfo, evtInfoCat, 0);
		// get type of trigger
		HEventHeader* header = gHades->getCurrentEvent()->getHeader();

		// get primary vertex
		HVertex vertexReco = header->getVertexReco();
		fEventHeader->SetVertexX(vertexReco.getX());
		fEventHeader->SetVertexY(vertexReco.getY());
		fEventHeader->SetVertexZ(vertexReco.getZ());
		// loop over FW hits
		Float_t wallHitBeta, wallHitX, wallHitY, wallHitZ;
		ushort wallModuleIndex, ring, nWallHitsTot;
		float wallHitCharge, wallHitChargeSpec, wallHitTime, wallHitDistance, wallChargeTot = 0.;
		short wallHitChargeZ;
		bool isWallHitOk;
		HWallHitSim* wallHit = 0;

		fFwHits->ClearChannels();
		{
			int iAdc = 		fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("adc");
			int iCharge = 	fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("charge");
			int iModule_id = 	fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("module_id");
			int iBeta = 		fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("beta");
			int iRing = 		fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("ring");
			int iTime = 		fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("time");

			for(Short_t j = 0; j < nWallHitsTot; j++)
			{ // loop over wall hits
				wallHit = HCategoryManager::getObject(wallHit, wallCat, j);
				wallModuleIndex = wallHit->getCell();
				wallHit->getXYZLab(wallHitX, wallHitY, wallHitZ);
				wallHitTime = wallHit->getTime();
				wallHitDistance = wallHit->getDistance();
				wallHitBeta = wallHitDistance / wallHitTime / 299.792458;
				wallHitCharge = wallHit->getCharge();
				wallHitChargeSpec =
					93. * pow(wallHitCharge, 0.46 - 0.006 * sqrt(wallHitCharge)); // parametrization from R.Holzmann
				wallHitChargeZ = evtChara.getFWCharge(wallHit);

				ring = divider->GetRing(wallModuleIndex);
				if(ring == 0)
				{
					cerr << "Error in short MHWallDivider::GetRing(short i=" << wallModuleIndex << "): it returned 0" << endl;
					return 2;
				}
				cout << "291 line" << endl;
				auto Hit = fFwHits->AddChannel();
				Hit->Init( fConfig.GetBranchConfig( fFwHits->GetId() ) );
				Hit->SetPosition( wallHitX, wallHitY, wallHitZ );
				Hit->SetField( float(wallHitCharge), iAdc);
				Hit->SetField( float(wallHitChargeZ), iCharge);
				Hit->SetField( int(wallModuleIndex), iModule_id);
				Hit->SetField( float(wallHitBeta), iBeta);
				Hit->SetField( int(ring), iRing);
				Hit->SetField( float(wallHitTime), iTime);
		}
		}
		// loop over particle candidates in event
		if(!candCat)
			continue;
		Int_t size = candCat->getEntries();
		HParticleCand* cand = 0;
		Int_t itr, pid;
		TLorentzVector trackPar;
		float p, theta, pt, eta, phi, mass;

		fVtxTracks->ClearChannels();

		const int iChi2 = 		fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("chi2");
		const int iVtx_ch2 = 	fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("vtx_chi2");
		const int idEdx = 		fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("dEdx");
		const int iDcax = 		fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("dcax");
		const int iQ = 			fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("q");
		const int iNhits = 		fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("nhits_0");
		const int iPid = 		fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("pid");
		const int iPt = 		fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("pt");
		const int iPhi = 		fConfig.GetBranchConfig( fVtxTracks->GetId() ).GetFieldId("phi");

		fTofHits->ClearChannels();

		const int iStat = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("status");
		const int iTime = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("time");
		const int iPath = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("path");
		const int iMatch = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("meta_mdc_match");
		const int iCharge = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("charge");
		const int iSqr_mass = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("sqr_mass");
		const int iSqr_mass_error = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("sqr_mass_error");


		for(Int_t j = 0; j < size; j++)
		{
			cand = HCategoryManager::getObject(cand, candCat, j);
			if(!cand)
			continue;
			if(!loop.goodSector(cand->getSector()))
			{
				continue; // skip inactive sectors
			}
				if(!cand->isFlagBit(kIsUsed))
				continue;
			if(cand->getMomentum() == cand->getMomentumOrg())
				continue; // skip tracks with too high pt ???
			pid = cand->getPID();

			if(pid >= 0)
			{
				mass = HPhysicsConstants::mass(pid);
				p = cand->getCorrectedMomentumPID(pid); // retrieve corrected momentum
				cand->setMomentum(p);                   // write it back
				cand->calc4vectorProperties(mass);      // sync with lorentz vector
			}
			else
			{
				mass = cand->getMass(); // META mass
				p = cand->getMomentum();
			}

			//						cout << pid << "\t" << cand->getMomentum() << "\t" <<
			//cand->getMomentumOrg() << "\t" << p << endl;

			theta = cand->getTheta() * D2R;
			phi = cand->getPhi() * D2R;
			pt = p * TMath::Sin(theta);
			eta = -TMath::Log(TMath::Tan(theta / 2.));

			trackPar.SetPtEtaPhiM(0.001 * pt, eta, phi, 0.001 * mass); // MeV -> GeV
			auto* Track = fVtxTracks->AddChannel();
			Track->SetMomentum( trackPar );
			Track->Init( fConfig.GetBranchConfig( fVtxTracks->GetId() ) );
			Track->SetField( int(cand->getCharge()), iQ);
			Track->SetField( float(cand->getChi2()), iChi2);
			Track->SetField( float(cand->getR()), iDcax); 
			Track->SetField( float(cand->getR()), iDcax+1);
			// Track->SetField( float(cand->getZ() - vertexReco.getZ()), iDcax+2);
			Track->SetField( float( cand->getMdcdEdx() ), idEdx);
			Track->SetField( int( pid ), iPid );
			Track->SetField( int( cand->getNLayer(0) ), iNhits);
			Track->SetField( int( cand->getNLayer(1) ), iNhits+1);
			Track->SetField( int( cand->getNLayer(2) ), iNhits+2);
			// Track->SetField( float(vChiToPrimVtx.at(0)), iVtx_ch2);
			Track->SetField( float(pt), iPt);
			Track->SetField( float(phi), iPhi);

			auto* Hit = fTofHits->AddChannel();
			Hit->Init( fConfig.GetBranchConfig( fTofHits->GetId() ) );
			if(cand->getSystem() == 0)
				Hit->SetField(HADES_constants::kRPC, iStat);
			else
				Hit->SetField(HADES_constants::kTOF, iStat);
			Hit->SetField( float(cand->getDistanceToMetaHit() / cand->getBeta() / 299.792458), iTime);
			Hit->SetField( float(cand->getDistanceToMetaHit()), iPath);
			Hit->SetField( float(cand->getMetaMatchRadius()), iMatch); // META match qa - NOT POSITION!!!
			Hit->SetField( int(cand->getCharge()), iCharge);
			Hit->SetField( float(cand->getMass2()), iSqr_mass);
			Hit->SetField( float(cand->getMetaMatchQuality()), iSqr_mass_error);

			itr++;
		} // end cand loop
		fATree->Fill();
	} // end eventloop

	cout << endl;
	fATree->Write();
	out->Close();

	delete gHades;
	return 0;
}

int main(int argc, char** argv)
{
	TString nevts;
	TString filenumber;
	switch(argc)
	{
		case 5:
			nevts = argv[3];
			return HTree_to_AT(TString(argv[1]), TString(argv[2]), nevts.Atoi(), TString(argv[4]));
			break;
		case 4:
			nevts = argv[3];
			return HTree_to_AT(TString(argv[1]), TString(argv[2]), nevts.Atoi());
			break;
		case 3:
			return HTree_to_AT(TString(argv[1]), TString(argv[2]));
			break;
		case 2:
			return HTree_to_AT(TString(argv[1]));
			break;
		case 1:
			return HTree_to_AT();
			break;
		default:
		cerr << "ERROR: loopDST() : WRONG NUMBER OF ARGUMENTS! TString infile, TString outfile, nevents=-1, TString "
				"parameterFile" << endl;
		return 1; // fail
	}
}
