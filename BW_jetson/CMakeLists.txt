cmake_minimum_required(VERSION 3.10)
project(main C)

# Imposta lo standard C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Aggiungi le cartelle degli header
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/emotions
    ${CMAKE_SOURCE_DIR}/touch
    ${CMAKE_SOURCE_DIR}/sonar 
)

# Trova tutti i file .c nella cartella src e src/emotions
file(GLOB SRC_FILES
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/emotions/*.c"
    "${CMAKE_SOURCE_DIR}/touch/*.c"
    "${CMAKE_SOURCE_DIR}/sonar/*.c"
)

# Crea l'eseguibile con tutti i file sorgente
add_executable(start_bw ${SRC_FILES})

# Linka librerie di sistema comuni
target_link_libraries(start_bw
    pthread   # threading
    m         # math
    rt        # realtime
)
