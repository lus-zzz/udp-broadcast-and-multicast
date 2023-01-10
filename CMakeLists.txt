cmake_minimum_required(VERSION 3.0.0)
project(md_udp VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(./)
aux_source_directory(./ udp_src )

add_executable(md_udp main.cpp ${udp_src})
target_link_libraries(md_udp -lpthread)


add_custom_command(TARGET md_udp 
                    POST_BUILD 
                    COMMAND bash -c "sshpass -p anchor@2022 scp  -o stricthostkeychecking=no ./md_udp root@192.168.0.91:/root/siproxd/"
                    VERBATIM
)

add_custom_command(TARGET md_udp 
                    POST_BUILD 
                    COMMAND bash -c "sshpass -p anchor@2022 scp  -o stricthostkeychecking=no ./md_udp root@192.168.0.92:/root/siproxd/"
                    VERBATIM
)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
