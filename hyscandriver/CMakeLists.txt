
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
             hyscan-discover.c
             hyscan-driver.c
             hyscan-device.c
             hyscan-sonar.c
             hyscan-sensor.c
             hyscan-actuator.c
             hyscan-driver-schema.c
             hyscan-device-schema.c
             hyscan-sonar-schema.c
             hyscan-sensor-schema.c
             hyscan-actuator-schema.c
             hyscan-sonar-info.c
             hyscan-sensor-info.c
             hyscan-actuator-info.c
             hyscan-device-driver.c
             hyscan-sonar-driver.c
             hyscan-sensor-driver.c
             hyscan-uart.c
             "${CMAKE_BINARY_DIR}/marshallers/hyscan-driver-marshallers.c")

target_link_libraries (${HYSCAN_DRIVER_LIBRARY} ${GLIB2_LIBRARIES} ${GMODULE2_LIBRARIES} ${HYSCAN_LIBRARIES} ${WIN32_LIBRARIES})

set_target_properties (${HYSCAN_DRIVER_LIBRARY} PROPERTIES DEFINE_SYMBOL "HYSCAN_API_EXPORTS")
set_target_properties (${HYSCAN_DRIVER_LIBRARY} PROPERTIES SOVERSION ${HYSCAN_DRIVER_VERSION})

configure_file (hyscandriver.pc.in
                "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hyscandriver-${HYSCAN_MAJOR_VERSION}.pc"
                @ONLY)

install (TARGETS ${HYSCAN_DRIVER_LIBRARY}
         COMPONENT runtime
         RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
         LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install (FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hyscandriver-${HYSCAN_MAJOR_VERSION}.pc"
         COMPONENT development
         DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install (FILES hyscan-discover.h
               hyscan-driver.h
               hyscan-device.h
               hyscan-sonar.h
               hyscan-sensor.h
               hyscan-actuator.h
               hyscan-driver-schema.h
               hyscan-device-schema.h
               hyscan-sonar-schema.h
               hyscan-sensor-schema.h
               hyscan-actuator-schema.h
               hyscan-sonar-info.h
               hyscan-sensor-info.h
               hyscan-actuator-info.h
               hyscan-device-driver.h
               hyscan-sonar-driver.h
               hyscan-sensor-driver.h
               hyscan-uart.h
         COMPONENT development
         DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hyscan-${HYSCAN_MAJOR_VERSION}/hyscandriver"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
