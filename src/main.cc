#include <iostream>
#include "hades_event_reader.h"

int main(int argv, char **argc) {
  std::string file_list = argc[1];
  HadesEventReader reader;
  reader.Init( file_list );
  reader.InitEvtChara( "../evtchara07/centrality_epcorr_apr12_gen8_2019_02_pass30.root" );
  reader.SwitchNextEvent();
  reader.ReadEvent();
  return 0;
}