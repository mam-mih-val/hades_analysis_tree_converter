cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(HT2AT)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH $ENV{AnalysisTree_HOME})
list(APPEND CMAKE_PREFIX_PATH ${AnalysisTree_HOME})

find_package(ROOT REQUIRED COMPONENTS RIO Tree)
find_package( AnalysisTree REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall")

include(${ROOT_USE_FILE})

include_directories(
		$ENV{HADDIR}/include
		${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/src
		${CMAKE_SOURCE_DIR}/evtchara07
		${ROOT_INCLUDE_DIRS}
		${AnalysisTree_INCLUDE_DIR}
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
		src/track_manager.h
		src/hit_manager.h
		src/wall_hits_manager.h
		src/tree_manager.h
		src/track_tof_match.h
		src/sim_track_manager.h
		src/sim_event_manager.h
		src/sim_reco_match.h
		src/mhwalldivider.h
		)

set (SOURCES
		src/hades_event_reader.cc
		src/detector_manager.cc
		src/event_manager.cc
		src/tree_manager.cc
		src/track_manager.cc
		src/hit_manager.cc
		src/wall_hits_manager.cc
		src/tree_manager.h
		src/track_tof_match.cc
		src/sim_track_manager.cc
		src/sim_event_manager.cc
		src/sim_reco_match.cc
		src/mhwalldivider.cc
		src/main.cc
		)

set (DICT_FILE_NAME G__EvtCharaBK)
ROOT_GENERATE_DICTIONARY(${DICT_FILE_NAME}
		evtchara07/hparticleevtcharaBK.h
		LINKDEF evtchara07/EvtCharaLinkDef.h
		OPTIONS -s libEvtCharaBK -I${HADES_INCLUDE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}
		)
add_library(EvtCharaBK SHARED evtchara07/hparticleevtcharaBK.cc ${DICT_FILE_NAME}.cxx)
target_link_libraries(EvtCharaBK ${ROOT_LIBRARIES} ${HADES_LIBS})

add_executable(HT2AT ${HEADERS} ${SOURCES})
target_link_libraries(HT2AT ${ROOT_LIBRARIES} ${HADES_LIBS}  AnalysisTreeBase)