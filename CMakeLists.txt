# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Nome do projeto mantido
project(projeto-lib-andrew-tobias C CXX ASM)

# Inicializa o SDK da Pico
pico_sdk_init()

# Define o arquivo principal como main.c
add_executable(projeto-lib-andrew-tobias 
    main.c
    MatrizLED.c
    ws2818b.pio
    ssd1306.c
    callbacks_timer.c
    mic.c
)


pico_generate_pio_header(projeto-lib-andrew-tobias ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)


pico_set_program_name(projeto-lib-andrew-tobias "projeto-lib-andrew-tobias")
pico_set_program_version(projeto-lib-andrew-tobias "0.1")

# Modifique abaixo para habilitar/desabilitar saída UART/USB
pico_enable_stdio_uart(projeto-lib-andrew-tobias 1)
pico_enable_stdio_usb(projeto-lib-andrew-tobias 1)

# Adiciona a biblioteca padrão à build
target_link_libraries(projeto-lib-andrew-tobias
        pico_stdlib
        hardware_pio
        hardware_clocks
        hardware_i2c
        hardware_dma
        hardware_adc)

# Adiciona os diretórios de include ao projeto
target_include_directories(projeto-lib-andrew-tobias PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Adiciona qualquer biblioteca extra necessária
target_link_libraries(projeto-lib-andrew-tobias)

pico_add_extra_outputs(projeto-lib-andrew-tobias)
