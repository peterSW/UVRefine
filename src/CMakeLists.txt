cmake_minimum_required (VERSION 2.6)
project (NormalRefine)


include_directories(/usr/include/OpenEXR)


set(LIBS boost_program_options IlmImf Imath Half Iex z)


add_executable(NormalRefine main.cpp)
target_link_libraries(NormalRefine ${LIBS})

add_executable(UV2Normal UV2Normal.cpp)
target_link_libraries(UV2Normal ${LIBS})

add_executable(Normal2UV Normal2UV.cpp)
target_link_libraries(Normal2UV ${LIBS})