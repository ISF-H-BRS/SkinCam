####################################################################################################
#                                                                                                  #
#   This file is part of the ISF SkinCam project.                                                  #
#                                                                                                  #
#   Author:                                                                                        #
#   Marcel Hasler <mahasler@gmail.com>                                                             #
#                                                                                                  #
#   Copyright (c) 2020 - 2024                                                                      #
#   Bonn-Rhein-Sieg University of Applied Sciences                                                 #
#                                                                                                  #
#   This program is free software: you can redistribute it and/or modify it under the terms        #
#   of the GNU General Public License as published by the Free Software Foundation, either         #
#   version 3 of the License, or (at your option) any later version.                               #
#                                                                                                  #
#   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
#   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      #
#   See the GNU General Public License for more details.                                           #
#                                                                                                  #
#   You should have received a copy of the GNU General Public License along with this program.     #
#   If not, see <https:# www.gnu.org/licenses/>.                                                   #
#                                                                                                  #
####################################################################################################

#  Generate Makefile by running
#
#  mkdir build && cd build
#  cmake -DCMAKE_TOOLCHAIN_FILE=../AVRToolchain.cmake ..
#
#  Supported make commands:
#
#  make         = Build the firmware
#  make clean   = Clean out built project files
#  make fuses   = Set correct AVR fuse bits, using avrdude
#  make program = Download the firmware to the device, using avrdude

project(Ringlight)
cmake_minimum_required(VERSION 3.4)

include_directories(/usr/avr/include ${CMAKE_BINARY_DIR})

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER   /usr/bin/avr-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/avr-g++)
set(CMAKE_ASM_COMPILER /usr/bin/avr-g++)

set(OBJCOPY_COMMAND /usr/bin/avr-objcopy)
set(OBJDUMP_COMMAND /usr/bin/avr-objdump)

set(NM_COMMAND /usr/bin/avr-nm)
set(SIZE_COMMAND /usr/bin/avr-size)

set(MCU "atmega168")
set(F_CPU "16000000")

set(SRC
    src/hostinterface.cpp
    src/ledcontrol.cpp
    src/nircontrol.cpp
    src/main.cpp
    src/ringlight.cpp
    src/trigger.cpp
)

# Set compiler and linker flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -gstabs -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmcu=${MCU}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DF_CPU=${F_CPU}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map,--cref")

# Create ELF
add_executable(${CMAKE_PROJECT_NAME}.elf ${SRC})

# Create HEX
add_custom_command(TARGET  ${CMAKE_PROJECT_NAME}.elf POST_BUILD
                   COMMAND ${OBJCOPY_COMMAND} -O ihex -R .eeprom
                           ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
                   COMMENT "Creating ${CMAKE_PROJECT_NAME}.hex")

# Create EEP
add_custom_command(TARGET  ${CMAKE_PROJECT_NAME}.elf POST_BUILD
                   COMMAND ${OBJCOPY_COMMAND} -j .eeprom --set-section-flags=.eeprom=alloc,load
                                                         --change-section-lma .eeprom=0 -O ihex
                           ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.eep
                   COMMENT "Creating ${CMAKE_PROJECT_NAME}.eep")

# Create LSS
add_custom_command(TARGET  ${CMAKE_PROJECT_NAME}.elf POST_BUILD
                   COMMAND ${OBJDUMP_COMMAND} -h -S ${CMAKE_PROJECT_NAME}.elf > ${CMAKE_PROJECT_NAME}.lss
                   COMMENT "Creating ${CMAKE_PROJECT_NAME}.lss")

# Create SYM
add_custom_command(TARGET  ${CMAKE_PROJECT_NAME}.elf POST_BUILD
                   COMMAND ${NM_COMMAND} -n ${CMAKE_PROJECT_NAME}.elf > ${CMAKE_PROJECT_NAME}.sym
                   COMMENT "Creating ${CMAKE_PROJECT_NAME}.sym")

# Print size
add_custom_command(TARGET  ${CMAKE_PROJECT_NAME}.elf POST_BUILD
                   COMMAND ${SIZE_COMMAND} ${CMAKE_PROJECT_NAME}.elf
                   COMMENT "Size:")

# Fuse bits as given by Engbedded Fuse Calculator
set(AVRDUDE_FUSE -U lfuse:w:0xf7:m -U hfuse:w:0xdf:m -U efuse:w:0xf9:m)

# Write command
set(AVRDUDE_WRITE -U flash:w:${CMAKE_PROJECT_NAME}.hex -U eeprom:w:${CMAKE_PROJECT_NAME}.eep)

# General flags
set(AVRDUDE_FLAGS -p ${MCU} -P usb -c avrisp2 -v)

# Program memory
add_custom_target(program COMMAND avrdude ${AVRDUDE_FLAGS} ${AVRDUDE_WRITE}
                          DEPENDS ${CMAKE_PROJECT_NAME}.elf)

# Set fuse bits
add_custom_target(fuses COMMAND avrdude ${AVRDUDE_FLAGS} ${AVRDUDE_FUSE})
