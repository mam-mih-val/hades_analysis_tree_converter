cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(HT2AT)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH $ENV{AnalysisTree_HOME})
list(APPEND CMAKE_PREFIX_PATH ${AnalysisTree_HOME})

find_package(Boost REQUIRED COMPONENTS program_options regex)
find_package(ROOT REQUIRED COMPONENTS RIO Tree)
find_package( AnalysisTree REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall")

include(${ROOT_USE_FILE})

include_directories(
		$ENV{HADDIR}/include
		${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/src
		${ROOT_INCLUDE_DIRS}
		${AnalysisTree_INCLUDE_DIR}
		${Boost_INCLUDE_DIRS}
)

link_directories(
		$ENV{HADDIR}/lib
		${CMAKE_SOURCE_DIR}
		${AnalysisTreeBase_LIBRARY_DIR}
)
set (HADES_LIBS
		libAlignment.so
		libHydra.so
		libMdcTrackD.so
		libOra.so
		libPionTracker.so
		libRpc.so
		libStart.so
		libDst.so
		libKalman.so
		libMdcTrackG.so
		libOraSim.so
		libQA.so
		libShower.so
		libTof.so
		libEmc.so
		libMdc.so
		libMdcUtil.so
		libOraUtil.so
		libRevt.so
		libShowerUtil.so
		libTools.so
		libEventDisplay.so
		libMdcGarfield.so
		libOnline.so
		libParticle.so
		libRich.so
		libSimulation.so
		libWall.so
		)
add_definitions(${ROOT_CXX_FLAGS})

set (HEADERS
		src/HADES_constants.h
		src/hades_event_reader.h
		src/detector_manager.h
		src/event_manager.h
		src/mdc_tracks_manager.h
		src/meta_hits_manager.h
		src/wall_hits_manager.h
		src/tree_manager.h
		src/mdc_meta_matching.h
        src/sim_tracks_manager.h
		src/sim_event_manager.h
		src/reco_sim_matching.h
		src/mhwalldivider.h
		)

set (SOURCES
		src/hades_event_reader.cc
		src/detector_manager.cc
		src/event_manager.cc
		src/tree_manager.cc
		src/mdc_tracks_manager.cc
		src/meta_hits_manager.cc
		src/wall_hits_manager.cc
		src/tree_manager.h
		src/mdc_meta_matching.cc
        src/sim_tracks_manager.cc
		src/sim_event_manager.cc
		src/reco_sim_matching.cc
		src/mhwalldivider.cc
		src/main.cc
        src/start_hits_manager.cc
		src/start_hits_manager.h
		src/start_hits_manager.cc
		)

add_executable(HT2AT ${HEADERS} ${SOURCES})
target_link_libraries(HT2AT ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ROOT::EG ${HADES_LIBS}  AnalysisTreeBase)

#add_executable(convert_wall
#		src/geant_wall_converter/convert.cc
#		src/geant_wall_converter/analysis_tree_manager.cc
#		src/geant_wall_converter/hades_tree_manager.cc
#		src/geant_wall_converter/wall_hits_filler.cc
#		)
#target_link_libraries(convert_wall ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ROOT::EG ${HADES_LIBS}  AnalysisTreeBase)