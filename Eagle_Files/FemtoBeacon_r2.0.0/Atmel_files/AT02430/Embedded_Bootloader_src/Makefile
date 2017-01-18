APP_NAME = Bootloader
PROJECT_NAME = Atmega1281
CONFIG_NAME = Bootloader_Atmega1281_Gcc

#PROJECT_NAME = Atmega128rfa1_OTAU
#CONFIG_NAME = Bootloader_Atmega128rfa1_Gcc
#CONFIG_NAME = Bootloader_Atmega128rfa1_Iar

#PROJECT_NAME = Atxmega256A3
#CONFIG_NAME = Bootloader_Atxmega256A3_Gcc
#CONFIG_NAME = Bootloader_Atxmega256A3_Iar

#PROJECT_NAME = Atxmega256A3_OTAU
#CONFIG_NAME = Bootloader_Atxmega256A3_Gcc
#CONFIG_NAME = Bootloader_Atxmega256A3_Iar

#PROJECT_NAME = Atxmega128A1
#CONFIG_NAME = Bootloader_Atxmega128A1_Gcc
#CONFIG_NAME = Bootloader_Atxmega128A1_Iar

#PROJECT_NAME = Atmega2561
#CONFIG_NAME = Bootloader_Atmega2561_Gcc
#CONFIG_NAME = Bootloader_Atmega2561_Iar

#PROJECT_NAME = At91sam3s4c
#CONFIG_NAME = Bootloader_At91sam3s4c_Gcc
#CONFIG_NAME = Bootloader_At91sam3s4c_Iar

#PROJECT_NAME = Atmega1281_OTAU
#CONFIG_NAME = Bootloader_Atmega1281_Gcc
#CONFIG_NAME = Bootloader_Atmega1281_Iar

#PROJECT_NAME = Atmega1281_RF4CE
#CONFIG_NAME = Bootloader_Atmega1281_Gcc

#PROJECT_NAME = Atxmega256D3
#CONFIG_NAME = Bootloader_Atxmega256D3_Gcc
#CONFIG_NAME = Bootloader_Atxmega256D3_Iar

#PROJECT_NAME = Atxmega256D3_OTAU
#CONFIG_NAME = Bootloader_Atxmega256D3_Gcc
#CONFIG_NAME = Bootloader_Atxmega256D3_Iar

#PROJECT_NAME = Atmega128rfa1
#CONFIG_NAME = Bootloader_Atmega128rfa1_Gcc
#CONFIG_NAME = Bootloader_Atmega128rfa1_Iar

#PROJECT_NAME = Atmega1281
#CONFIG_NAME = Bootloader_Atmega1281_Gcc
#CONFIG_NAME = Bootloader_Atmega1281_Iar

#PROJECT_NAME = Atmega128rfa1_RF4CE
#CONFIG_NAME = Bootloader_Atmega128rfa1_Gcc

all:
	make -C makefiles/$(PROJECT_NAME) -f Makefile_$(CONFIG_NAME) all APP_NAME=$(APP_NAME)

clean:
	make -C makefiles/$(PROJECT_NAME) -f Makefile_$(CONFIG_NAME) clean APP_NAME=$(APP_NAME)
