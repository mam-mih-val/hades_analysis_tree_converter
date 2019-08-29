cmake_minimum_required(VERSION 2.4 FATAL_ERROR)

project(HTree_TO_AT)

set(CMAKE_BUILD_TYPE debug)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
list(APPEND CMAKE_PREFIX_PATH $ENV{AnalysisTree_HOME})
list(APPEND CMAKE_PREFIX_PATH ${AnalysisTree_HOME})

find_package(ROOT REQUIRED COMPONENTS RIO Tree)
find_package(AnalysisTree REQUIRED)

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
    ${AnalysisTree_LIBRARY_DIR}
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
		HADES_constants.h 
		mhwalldivider.h
		HadesInterface.h
)

set (SOURCES
		HadesInterface.cxx
)

set (DICT_FILE_NAME G__EvtCharaBK)
ROOT_GENERATE_DICTIONARY(${DICT_FILE_NAME}
        evtchara07/hparticleevtcharaBK.h
        LINKDEF evtchara07/EvtCharaLinkDef.h
        OPTIONS -s libEvtCharaBK -I${HADES_INCLUDE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}
        )
add_library(EvtCharaBK SHARED evtchara07/hparticleevtcharaBK.cc ${DICT_FILE_NAME}.cxx)
target_link_libraries(EvtCharaBK ${ROOT_LIBRARIES} ${HADES_LIBS})

add_executable(HTree_to_DT ${HEADERS} ${SOURCES})
target_link_libraries(HTree_to_DT ${ROOT_LIBRARIES} ${HADES_LIBS} AnalysisTree EvtCharaBK)
