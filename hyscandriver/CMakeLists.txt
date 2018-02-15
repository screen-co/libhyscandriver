
find_program (MARSHAL_COMPILER glib-genmarshal)
if (${MARSHAL_COMPILER} STREQUAL MARSHAL_COMPILER-NOTFOUND)
  message (FATAL_ERROR "GLib marshallers compiler not found.")
endif ()

file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/marshallers")
include_directories ("${CMAKE_BINARY_DIR}/marshallers")

add_custom_command (OUTPUT "${CMAKE_BINARY_DIR}/marshallers/hyscan-driver-marshallers.c"
                    COMMAND ${MARSHAL_COMPILER} --header
                            --prefix hyscan_driver_marshal
                            hyscan-driver-marshallers.list >
                            "${CMAKE_BINARY_DIR}/marshallers/hyscan-driver-marshallers.h"
                    COMMAND ${MARSHAL_COMPILER} --body
                            --prefix hyscan_driver_marshal
                            hyscan-driver-marshallers.list >
                            "${CMAKE_BINARY_DIR}/marshallers/hyscan-driver-marshallers.c"
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    DEPENDS hyscan-driver-marshallers.list
                    VERBATIM)

add_library (${HYSCAN_DRIVER_LIBRARY} SHARED
             hyscan-sonar.c
             hyscan-sensor.c
             hyscan-driver.c
             hyscan-discover.c
             hyscan-device-schema.c
             hyscan-sonar-schema.c
             hyscan-sensor-schema.c
             hyscan-sonar-info.c
             hyscan-sensor-info.c
             "${CMAKE_BINARY_DIR}/marshallers/hyscan-driver-marshallers.c")

target_link_libraries (${HYSCAN_DRIVER_LIBRARY} ${GLIB2_LIBRARIES} ${GMODULE2_LIBRARIES} ${HYSCAN_LIBRARIES})

set_target_properties (${HYSCAN_DRIVER_LIBRARY} PROPERTIES DEFINE_SYMBOL "HYSCAN_API_EXPORTS")
set_target_properties (${HYSCAN_DRIVER_LIBRARY} PROPERTIES SOVERSION ${HYSCAN_DRIVER_VERSION})

configure_file (hyscandriver.pc.in
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hyscandriver-${HYSCAN_MAJOR_VERSION}.pc"
                @ONLY)

install (TARGETS ${HYSCAN_DRIVER_LIBRARY}
         COMPONENT runtime
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install (FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hyscandriver-${HYSCAN_MAJOR_VERSION}.pc"
         COMPONENT development
         DESTINATION "lib/pkgconfig"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install (FILES hyscan-sonar.h
               hyscan-sensor.h
               hyscan-driver.h
               hyscan-discover.h
               hyscan-sonar-schema.h
               hyscan-sensor-schema.h
               hyscan-sonar-info.h
         COMPONENT development
         DESTINATION "include/hyscan-${HYSCAN_MAJOR_VERSION}/hyscandriver"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
