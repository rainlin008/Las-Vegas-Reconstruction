set(FUSION_SOURCES
    Options.cpp
    Main.cpp
)

add_executable(lvr_fusion ${FUSION_SOURCES})

target_link_libraries(lvr_fusion lvr_static)
target_link_libraries(lvr_fusion ${Boost_FILESYSTEM_LIBRARY}
							 ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY}
							 ${Boost_PROGRAM_OPTIONS_LIBRARY} ${GMP_LIBRARIES}
							 ${MPFR_LIBRARIES} ${CGAL_LIBRARY})

install(TARGETS lvr_fusion
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
