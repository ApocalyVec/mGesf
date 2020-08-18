OS := $(shell uname -s)
$(info OS: $(OS))

GXX = g++
GCC = gcc

#-Wall #-Werror

GXX_FLAGS += --std=gnu++11

C_FLAGS = -Wno-unused-variable -std=c11

INCLUDES += \
	-Iinclude/ \
	-Isrc/XDriver/ \
        -Isrc/Radar/\
        -Isrc/hal/\


C_SOURCE += \
	src/XDriver/x4driver.c \
        src/hal/radar_hal.c \

CPP_SOURCE += \
	src/main.cpp \
	src/Radar/taskRadar.cpp \

ifeq ($(OS),$(filter $(OS),Linux Darwin))
	CPP_SOURCE +=
else # Windows
	CPP_SOURCE +=
endif

OBJECTS = $(patsubst %.cpp,_build/%.o, $(CPP_SOURCE))
OBJECTS += $(patsubst %.c,_build/%.o, $(C_SOURCE))

CREATE_DEPS = -MMD -MP -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"



.PHONY: all
all: Runme
	echo "done"

_build/%.o: %.cpp Makefile
	mkdir -p $(@D)
	${GXX} ${GXX_FLAGS} ${CREATE_DEPS} ${INCLUDES} -c $< -o $@

_build/%.o: %.c Makefile
	mkdir -p $(@D)
	$(GCC) ${C_FLAGS} ${INCLUDES} ${CREATE_DEPS} -c $< -o $@

Runme: $(OBJECTS)
	${GXX} $^ -o $@ ${TARGET_LIB_LINKER_ARGS} -lpthread -lwiringPi

.PHONY: clean
clean:
	-rm -r _build
	-rm Runme.exe
	-rm Runme


C_DEPS := $(patsubst %.o,%.d, $(OBJECTS))

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif
