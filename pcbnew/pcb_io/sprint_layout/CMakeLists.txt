
# Sprint Layout (.lay6/.lay) board importer

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( SPRINT_LAYOUT_SRCS
    pcb_io_sprint_layout.cpp
    sprint_layout_parser.cpp
    )

add_library( sprint_layout2pcbnew STATIC ${SPRINT_LAYOUT_SRCS} )

target_link_libraries( sprint_layout2pcbnew pcbcommon )

target_include_directories( sprint_layout2pcbnew PRIVATE
                            "${CMAKE_CURRENT_SOURCE_DIR}/../" )
