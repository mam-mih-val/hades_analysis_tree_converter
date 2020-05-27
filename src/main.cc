#include <iostream>
#include "hades_event_reader.h"

int main(int argv, char **argc) {
  std::string file_list = argc[1];
  std::string file_output{"output.root"};
  HadesEventReader reader;
  reader.Init( file_list );
  reader.InitEvtChara( "../evtchara07/centrality_epcorr_apr12_gen8_2019_02_pass30.root" );
  Analysis::TreeManager::Instance()->CreateTree(file_output);
  int i=0;
  while( !reader.Eof() ) {
    reader.SwitchNextEvent();
    reader.ReadEvent();
    bool is_written = Analysis::TreeManager::Instance()->WriteEvent();
    if(is_written)
      ++i;
  }
  std::cout << i << " events were converted from hades tree to analysis tree." << std::endl;
  Analysis::TreeManager::Instance()->Finalize();
  return 0;
}