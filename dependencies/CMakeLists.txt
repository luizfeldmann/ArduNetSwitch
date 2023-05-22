# Create dependency library targets

set(ETHERNET_DIR 
    "${CMAKE_CURRENT_LIST_DIR}/Ethernet/src")

set(ETHERNET_SRCS
    "${ETHERNET_DIR}/Dhcp.cpp"
    "${ETHERNET_DIR}/Dhcp.h"
    "${ETHERNET_DIR}/Dns.cpp"
    "${ETHERNET_DIR}/Dns.h"
    "${ETHERNET_DIR}/Ethernet.cpp"
    "${ETHERNET_DIR}/Ethernet.h"
    "${ETHERNET_DIR}/EthernetClient.cpp"
    "${ETHERNET_DIR}/EthernetClient.h"
    "${ETHERNET_DIR}/EthernetServer.cpp"
    "${ETHERNET_DIR}/EthernetServer.h"
    "${ETHERNET_DIR}/EthernetUdp.cpp"
    "${ETHERNET_DIR}/EthernetUdp.h"
    "${ETHERNET_DIR}/socket.cpp"
    "${ETHERNET_DIR}/utility/w5100.h"
    "${ETHERNET_DIR}/utility/w5100.cpp" )

add_library(Ethernet STATIC ${ETHERNET_SRCS})

target_include_directories(Ethernet PUBLIC ${ETHERNET_DIR})

target_link_arduino_libraries(Ethernet PRIVATE core SPI)