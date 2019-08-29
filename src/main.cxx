#include "HadesInterface.h"
#include "hloop.h"

int main(int argv, char** argc)
{
	std::string inFile, outFile;
	if(argv == 0 || argv > 2)
	{
		cout << "Error amount of arguments" << endl;
		return 1;
	}
	if(argv==1)
	{
		inFile=std::string(argc[1]);
		outFile="ATreeOut.root";
	}
	if(argv==2)
	{
		inFile=std::string(argc[1]);
		outFile=std::string(argc[2]);
	}
	Analysis::HadesInterface* fInterface = new Analysis::HadesInterface();
	HLoop loop(kTRUE);
	loop.addFiles(inFile.data());
	loop.setInput("-*,+HParticleCand,+HParticleEvtInfo,+HWallHit")
	
	fInterface -> SetOutputFile(outFile);
	// fInterface -> SetInputFile(InFileName);
	fInterface->Init();

	Int_t entries = loop.getEntries();
	for(Int_t i = 1; i < entries; i++)
  	{
	    Int_t nbytes = loop.nextEvent(i);
		fInterface->Exec();
	}
	fInterface->Finish();

	return 0;
}