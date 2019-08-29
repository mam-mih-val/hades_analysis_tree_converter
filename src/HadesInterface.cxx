//TODO runid, eventid, vertex, fitter!, match in STS, constants

#include "HadesInterface.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <TGeoManager.h>
#include <TGeoBBox.h>
#include "TROOT.h"
#include "TDirectory.h"

#include "AnalysisTree/Constants.h"

using std::cout;
using std::endl;
using std::vector;

ClassImp(Analysis::HadesInterface)

	namespace Analysis
{

HadesInterface::HadesInterface() :
	candCat((HCategory*)HCategoryManager::getCategory(catParticleCand)),
	evtInfoCat((HCategory*)HCategoryManager::getCategory(catParticleEvtInfo)),
	wallCat((HCategory*)HCategoryManager::getCategory(catWallHit))
{
}
HadesInterface::~HadesInterface()
{
}

void HadesInterface::Init()
{
	this->SetEvtCharaParameterFile();
	this->InitInput();
	this->InitOutput();
	this->ReadStaticInfo();
}
// //----------------------------------
void HadesInterface::InitInput()
{
	fEvtChara.setParameterFile(fParameterFile.data());
	fEvtChara.init();
	dEdxCorr.setDefaultPar("apr12");
	candCat = (HCategory*)HCategoryManager::getCategory(catParticleCand);
	evtInfoCat = (HCategory*)HCategoryManager::getCategory(catParticleEvtInfo);
	wallCat = (HCategory*)HCategoryManager::getCategory(catWallHit);
	divider = new MHWallDivider();	
	evtInfo = HCategoryManager::getObject(evtInfo, evtInfoCat, 0);
	fHeader = gHades->getCurrentEvent()->getHeader();
}

//----------------------------------
void HadesInterface::InitOutput()
{
	//  ***** Vertex Tracks *******

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
	fTreeFile = TFile::Open(fOutputFileName.data(), "RECREATE");
	fTreeFile->cd();
	fATree = new TTree("aTree", "Analysis Tree, HADES data");
	fATree->Branch("VtxTracks", "AnalysisTree::TrackDetector", &fVtxTracks);
	fATree->Branch("EventHeader", "AnalysisTree::EventHeader", &fEventHeader);
	fATree->Branch("FwModules", "AnalysisTree::HitDetector", &fFwHits);
	fATree->Branch("TofHits", "AnalysisTree::HitDetector", &fTofHits);
	fConfig.Write("Configuration");
}

void HadesInterface::Exec(Option_t * opt)
{
	cout << "HadesInterface::Exec" << endl;
	ReadEvent();
	ReadVertexTracks();
	ReadTof();
	ReadFw();
	fATree->Fill();
}

void HadesInterface::Finish()
{
	cout << "HadesInterface::Finish" << endl;
	fATree->Write();
	fTreeFile->Write();
	fTreeFile->Close();
}

void HadesInterface::ReadStaticInfo()
{
	std::cout << "ReadStaticInfo" << std::endl;
	fStaticInfo.SetSystem("Au+Au");
	float pBeam = sqrt((1.23)*(1.23) - (0.938)*(0.938)); 
	fStaticInfo.SetBeamMomentum(pBeam);
	float yBeam = 0.5*log( (1.23+pBeam) - (1.23-pBeam) );
	fStaticInfo.SetBeamMomentum(pBeam);
	fStaticInfo.SetBeamRapidity(yBeam);
	
	fStaticInfo.Write("StaticInfo");
}

void HadesInterface::ReadEvent()
{
	cout << "HadesInterface::ReadEvent" << endl;

	if (!fHeader)
	{
		cout << "No fHeader!" << endl;
		return;
	}

	HEventHeader* header = gHades->getCurrentEvent()->getHeader();
	HVertex vertexReco = header->getVertexReco();

	fEventHeader->SetVertexX(vertexReco.getX());
	fEventHeader->SetVertexY(vertexReco.getY());
	fEventHeader->SetVertexZ(vertexReco.getZ());

	// for( int i=0; i<fCEmapBK.size(); i++ )
	// {
	// 	fEventHeader->SetField( int(fEvtChara.getCentralityEstimator(fCEmapBK.at(i).second), fConfig.GetBranchConfig(fEventHeader->GetId()).GetFieldId( fCEmapNames.second ) ) );
	// 	fEventHeader->SetField( float(fEvtChara.getCentralityPercentile(fCEmapBK.at(i).second), fConfig.GetBranchConfig(fRecEventHeader->GetId()).GetFieldId("centrality_"+fCEmapNames.second)));
	// }
}

void HadesInterface::ReadFw()
{
	std::cout << "ReadFw" << std::endl;
	fFwHits->ClearChannels();

	const int iAdc = 		fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("adc");
	const int iCharge = 	fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("charge");
	const int iModule_id = 	fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("module_id");
	const int iBeta = 		fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("beta");
	const int iRing = 		fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("ring");
	const int iTime = 		fConfig.GetBranchConfig( fFwHits->GetId() ).GetFieldId("time");

	Float_t wallHitBeta, wallHitX, wallHitY, wallHitZ;
    ushort wallModuleIndex, ring, nWallHitsTot;
    float wallHitCharge, wallHitTime, wallHitDistance;
    short wallHitChargeZ;
	HWallHitSim* wallHit = 0;

	for(Short_t j = 0; j < nWallHitsTot; j++)
	{
		wallHit = HCategoryManager::getObject(wallHit, wallCat, j);
		wallHit->getXYZLab(wallHitX, wallHitY, wallHitZ);
		wallModuleIndex = wallHit->getCell();
		wallHit->getXYZLab(wallHitX, wallHitY, wallHitZ);
		wallHitTime = wallHit->getTime();
		wallHitDistance = wallHit->getDistance();
		wallHitBeta = wallHitDistance / wallHitTime / 299.792458;
		wallHitCharge = wallHit->getCharge();
		wallHitChargeZ = fEvtChara.getFWCharge(wallHit);
		ring = divider->GetRing(wallModuleIndex);
		
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

void HadesInterface::ReadVertexTracks()
{
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

	Int_t size = candCat->getEntries();
	HParticleCand* cand = 0;
	Int_t itr, pid;
	TLorentzVector trackPar;
	const Float_t D2R = TMath::DegToRad();
	float p, theta, pt, eta, phi, mass;

	for(Int_t j = 0; j < size; j++)
	{
		cand = HCategoryManager::getObject(cand, candCat, j);
		if(!cand)
			continue;
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
		Track->SetField( float(trackParamFirst->GetX()-fPrimVtx->GetX()), iDcax); 
		Track->SetField( float(trackParamFirst->GetY()-fPrimVtx->GetY()), iDcax+1);
		Track->SetField( float(trackParamFirst->GetZ()-fPrimVtx->GetZ()), iDcax+2);
		Track->SetField( float( cand->getMdcdEdx() ), idEdx);
		Track->SetField( int( pid ), iPid );
		Track->SetField( int( cand->getNLayer(0) ), iNhits);
		Track->SetField( int( cand->getNLayer(1) ), iNhits+1);
		Track->SetField( int( cand->getNLayer(2) ), iNhits+2);
		Track->SetField( float(vChiToPrimVtx.at(0)), iVtx_ch2);
		Track->SetField( float(pt), iPt);
		Track->SetField( float(phi), iPhi);
	}
}

void HadesInterface::ReadTof()
{
	std::cout << "ReadTOF" << std::endl;
	fTofHits->ClearChannels();

	const int iStat = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("status");
	const int iTime = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("time");
	const int iPath = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("path");
	const int iMatch = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("meta_mdc_match");
	const int iCharhe = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("charge");
	const int iSqr_mass = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("sqr_mass");
	const int iSqr_mass_error = fConfig.GetBranchConfig(fTofHits->GetId()).GetFieldId("sqr_mass_error");

	for(Int_t j = 0; j < size; j++)
	{
		cand = HCategoryManager::getObject(cand, candCat, j);
		auto* Hit = fTofHits->AddChannel();
		Hit->Init( fConfig.GetBranchConfig( fTofHits->GetId() ) );

		if(cand->getSystem() == 0)
			Hit.SetField(HADES_constants::kRPC, iStat);
		else
			Hit.SetField(HADES_constants::kTOF, iStat);
		Hit.SetField( float(cand->getDistanceToMetaHit() / cand->getBeta() / 299.792458), iTime);
		Hit.SetField( float(cand->getDistanceToMetaHit()), iPath);
		Hit.SetField( float(cand->getMetaMatchRadius()), iMatch); // META match qa - NOT POSITION!!!
		Hit.SetField( int(cand->getCharge()), iCharge);
		Hit.SetField( float(cand->getMass2()), iSqr_mass);
		Hit.SetField( float(cand->getMetaMatchQuality()), iSqr_mass_error);
	}
} // namespace Analysis
