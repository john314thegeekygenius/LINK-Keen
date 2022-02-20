
#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-Wno-incompatible-pointer-types -Wall -O2  \
		-mcpu=arm7tdmi -mtune=arm7tdmi\
		$(ARCH)

CXXFLAGS	:=	$(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	$(ARCH)
LDFLAGS	=	$(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lmm -lgba

#---------------------------------------------
# files
#---------------------------------------------

GB_SRC = src/LK_Main.c src/LK_Game.c src/LK_Object.c src/LK_Map.c src/LK_Levels.c src/LK_Sound.c src/LK_Net.c \
		src/LK_Menu.c src/LK_Act1.c src/LK_Audio.c src/LK_Input.c src/LK_Graphics.c src/GBA_Defs.c 

GB_GBA = out/LKEEN.gba
GB_GBABOOT = out/BKEEN.gba

GB_MUS_O = ck_music_0.o ck_music_1.o ck_music_2.o
GB_SND_O = ck_snd_0.o ck_snd_1.o ck_snd_2.o ck_snd_3.o ck_snd_4.o ck_snd_5.o ck_snd_6.o ck_snd_7.o \
			ck_snd_8.o ck_snd_9.o ck_snd_10.o ck_snd_11.o ck_snd_12.o ck_snd_13.o ck_snd_14.o ck_snd_15.o \
			ck_snd_16.o ck_snd_17.o ck_snd_18.o ck_snd_19.o ck_snd_20.o

#https://gist.github.com/JShorthouse/bfe49cdfad126e9163d9cb30fd3bf3c2

build:
	arm-none-eabi-gcc $(GB_SND_O) $(GB_MUS_O) crt0.s $(GB_SRC) -mcpu=arm7tdmi -nostartfiles -Tlnkscript
	#mv a.out $(GB_GBA)
	arm-none-eabi-objcopy -v -O binary a.out $(GB_GBA)
	gbafix $(GB_GBA) -t LINK-KEEN
build_multiboot:
	arm-none-eabi-gcc -DLK_MULTIBOOT_ROM=0 crt0.s $(GB_SRC) -mcpu=arm7tdmi -nostartfiles -Tlnkscript
	#mv a.out $(GB_GBA)
	arm-none-eabi-objcopy -v -O binary a.out $(GB_GBABOOT)
	gbafix $(GB_GBABOOT) -t BOOT-KEEN

sound:
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_0.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_1.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_2.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_3.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_4.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_5.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_6.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_7.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_8.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_9.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_10.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_11.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_12.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_13.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_14.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_15.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_16.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_17.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_18.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_19.c  -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_snd_20.c  -mcpu=arm7tdmi -nostartfiles
music:
	arm-none-eabi-gcc -c src/CK_Audio/ck_music_0.c -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_music_1.c -mcpu=arm7tdmi -nostartfiles
	arm-none-eabi-gcc -c src/CK_Audio/ck_music_2.c -mcpu=arm7tdmi -nostartfiles

all: music sound build



