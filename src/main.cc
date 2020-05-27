#include <iostream>
#include "hades_event_reader.h"

int main(int argv, char **argc) {
  auto start = std::chrono::system_clock::now();
  if( argv < 2 ){
    std::cout << "Error: Incorrect number of arguments. Minimum 1 argument required." << std::endl;
    std::cout << "Example: ./HT2AT path/to/input.root path/to/output.root" << std::endl;
    return 1;
  }
  std::string file_list;
  file_list = argc[1];
  std::string file_output{"output.root"};
  if( argv > 2 )
    file_output = argc[2];

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

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "Elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}