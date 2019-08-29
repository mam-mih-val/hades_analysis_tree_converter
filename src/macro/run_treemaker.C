
void run_treemaker( const std::string InFileName, std::string OutFileName="out.root")
{
	Analysis::HadesInterface* fInterface = new Analysis::HadesInterface();
	HLoop loop(kTRUE);
	loop.addFiles(InFileName.data());
	loop.setInput("-*,+HParticleCand,+HParticleEvtInfo,+HWallHit")
	
	fInterface -> SetOutputFile(OutFileName);
	fInterface -> SetInputFile(InFileName);
	fInterface->Init();

	Int_t entries = loop.getEntries();
	for(Int_t i = 1; i < entries; i++)
  	{
	    Int_t nbytes = loop.nextEvent(i);
		fInterface->Exec();
	}
	fInterface->Finish();
}
