//
// Created by mikhail on 2/11/21.
//

#include <iostream>
#include <chrono>

#include <boost/program_options.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "converting_manager.h"

int main(int argv, char **argc) {
  namespace po = boost::program_options;
  auto start = std::chrono::system_clock::now();
  if( argv < 2 ){
    throw std::runtime_error( "Incorrect number of argument. Type ./HT2AT --help to see options" );
  }
  std::string input_list;
  std::string output_file{"output.root"};
  std::string config_file;
  std::string geant_file;
  long long n_events{0};
  long long first_event{0};
  po::options_description options("Options");
  options.add_options()
      ("help,h", "Help screen")
      ("input,i", po::value<std::string>(&input_list),
       "Input file list")
      ("output,o", po::value<std::string>(&output_file),
       "Name of output file")
      ("config,c", po::value<std::string>(&config_file),
       "Path to config file")
      ("events,N", po::value<long long>(&n_events),
       "Number of analysing events")
      ("first-event", po::value<long long>(&first_event),
       "Number of analysing events");
  po::variables_map vm;
  po::parsed_options parsed = po::command_line_parser(argv, argc).options(options).run();
  po::store(parsed, vm);
  po::notify(vm);
  if (vm.count("help")){
    std::cout << options << std::endl;
    return 0;
  }
  bool is_mc=vm.count("MC");
  boost::property_tree::ptree config;
  try {
    boost::property_tree::read_json(config_file, config);
  } catch( const std::exception& e ){
    throw std::runtime_error( "JSON config is not specified or specified incorrectly" );
  }
  auto system = config.get<std::string>("system", "");
  auto energy = config.get<float>("energy", 0.0f);
  auto event_chara_param_file = config.get<std::string>("param file", "");
  auto szymon_start_histograms_file = config.get<std::string>("start strips file", "");

  ConvertingManager converting_manager;

  converting_manager.InitInput(input_list);
  converting_manager.InitOutput(output_file, "reconstructed_wall");
  converting_manager.Process(first_event, n_events);
  converting_manager.Finalize();

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "Elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}