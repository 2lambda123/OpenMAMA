project (mamac)

# avoid warning from xxx_ROOT variables
if(${CMAKE_VERSION} VERSION_LESS "3.16.5")
else()
   cmake_policy(SET CMP0074 NEW)
endif()

get_target_property(WOMBAT_INCLUDES wombatcommon INCLUDE_DIRECTORIES)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${WOMBAT_INCLUDES}
	${APR_INCLUDE_DIR}
	${APRUTIL_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR} # for generated files like version.h etc
)

set(ENTITLEMENT_BRIDGES "")
if(DEFINED ENTITLEMENTS)
	string(REPLACE "," ";" ENT_LIST ${ENTITLEMENTS})
	foreach(ENT ${ENT_LIST})
		set(ENTITLEMENT_BRIDGES "${ENTITLEMENT_BRIDGES}\"${ENT}\", ")
		if(${ENT} STREQUAL "noop")
			add_subdirectory (entitlement/noop)
		endif()
		if(${ENT} STREQUAL "oea")
			add_subdirectory (entitlement/oea)
		endif()
	endforeach()
endif()
set(ENTITLEMENT_BRIDGES "${ENTITLEMENT_BRIDGES}NULL")

add_definitions(-DMAMA -DAPR_DECLARE_STATIC)

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/mama/version.h" @ONLY)
configure_file(version.c.in "${CMAKE_CURRENT_BINARY_DIR}/version.c" @ONLY)
configure_file(entitlementlibraries.c.in "${CMAKE_CURRENT_BINARY_DIR}/entitlementlibraries.c" @ONLY)

set(sources
	bridge.c
	clientmanageresponder.c
	datetime.c
	dictionary.c
	dqpublisher.c
	dqpublishermanager.c
	dqstrategy.c
	endpointpool.c
	entitlement.c
	${CMAKE_CURRENT_BINARY_DIR}/entitlementlibraries.c
	error.c
	fielddesc.c
	fileutils.c
	ft.c
	imagerequest.c
	inbox.c
	io.c
	listenermsgcallback.c
	log.c
	mama.c
	queuegroup.c
	mama/queuegroup.h
	resourcepool.c
	resourcepool.h
	mama/resourcepool.h
	mamaStrUtils.c
	mamautils.c
	marketdata.c
	middleware.c
	msg.c
	msgfield.c
	msgqualifier.c
	msgstatus.c
	msgtype.c
	msgutils.c
	plugin.c
	price.c
	priceimpl.c
	publisher.c
	quality.c
	queue.c
	refreshtransport.c
	regexchannelfilter.c
	mama/regexchannelfilter.h
	registerfunctions.c
	reservedfields.c
	senderId.c
	servicelevel.c
	source.c
	sourceman.c
	stat.c
	statfields.c
	statscollector.c
	statsgenerator.c
	statslogger.c
	statsloggerfields.c
	status.c
	subscmsgtype.c
	subscription.c
	subscriptiontype.c
	symbollist.c
	symbollistmember.c
	symbolmapfile.c
	syncresponder.c
	throttle.c
	timer.c
	timezone.c
	transport.c
	${CMAKE_CURRENT_BINARY_DIR}/version.c
	playback/playbackFileParser.c
	playback/playbackpublisher.c
	playback/playbackcapture.c
	conflation/connection.c
	conflation/serverconnection.c
	conflation/manager.c
	fieldcache/fieldcache.c
	fieldcache/fieldcachefieldimpl.c
 	fieldcache/fieldcacheiterator.c
	fieldcache/fieldcachemaparray.c
	fieldcache/fieldcachemap.c
	fieldcache/fieldcacherecord.c
	fieldcache/fieldcachefield.c
	fieldcache/fieldcacheimpl.c
	fieldcache/fieldcachelist.c
	fieldcache/fieldcachevector.c
)

# Compile this code with C++ compiler to leverage inbuilt regex support
set_source_files_properties(regexchannelfilter.c PROPERTIES LANGUAGE CXX)

list(APPEND sources ${SYSTEM_SOURCES})

list(APPEND MAMA_LINK_LIBRARIES ${APR_LIBRARY} ${APRUTIL_LIBRARY} wombatcommon ${CMAKE_THREAD_LIBS_INIT})
list(APPEND MAMA_STATIC_LINK_LIBRARIES ${APR_LIBRARY} ${APRUTIL_LIBRARY} wombatcommonstatic ${CMAKE_THREAD_LIBS_INIT})
if(UNIX)
	list(APPEND MAMA_LINK_LIBRARIES m)
	list(APPEND MAMA_STATIC_LINK_LIBRARIES m)
else()
	list(APPEND MAMA_LINK_LIBRARIES rpcrt4)
	list(APPEND MAMA_STATIC_LINK_LIBRARIES rpcrt4)
endif()

add_library(mama SHARED ${sources})
target_link_libraries(mama ${MAMA_LINK_LIBRARIES})

add_library(mamastatic STATIC ${sources})
target_link_libraries(mamastatic ${MAMA_STATIC_LINK_LIBRARIES})

# On windows this file has historically been named libmamac for some reason. Will conform in future.
set_target_properties(mama PROPERTIES OUTPUT_NAME "mama${OPENMAMA_MAMA_TARGET_SUFFIX}${OPENMAMA_LIBRARY_SUFFIX}")
set_target_properties(mamastatic PROPERTIES OUTPUT_NAME "mama${OPENMAMA_MAMA_TARGET_SUFFIX}${OPENMAMA_LIBRARY_STATIC_SUFFIX}")

install(TARGETS mama mamastatic
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
if (WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:mama> DESTINATION bin OPTIONAL)
endif()
install(DIRECTORY mama/ DESTINATION include/mama)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/mama/version.h" DESTINATION include/mama)

# Install any dependant libraries
if (INSTALL_RUNTIME_DEPENDENCIES)
	if (WIN32)
		install (DIRECTORY ${APR_ROOT}/bin/ DESTINATION bin FILES_MATCHING PATTERN "*apr*.dll")
    elseif(UNIX)
        message(STATUS "Installing apr_root")
        install (FILES ${APR_LIBRARY_FILES} ${APRUTIL_LIBRARY_FILES} DESTINATION lib)
	endif()
endif()
