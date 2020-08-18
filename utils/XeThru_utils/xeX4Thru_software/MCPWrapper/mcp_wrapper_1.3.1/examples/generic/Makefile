OS := $(shell uname -s)
$(info OS: $(OS))

GXX = g++
GCC = gcc

#-Wall #-Werror

GXX_FLAGS += --std=gnu++11

C_FLAGS = -Wno-unused-variable

INCLUDES += -I../../include/
INCLUDES += -I../../src/
INCLUDES += -I./src/


C_SOURCE += \
	../../src/mcp_wrapper.c \
	../../src/mcp/protocol.c \
	../../src/mcp/protocol_host_parser.c \
	../../src/mcp/protocol_parser.c \


CPP_SOURCE += \
    src/main.cpp

ifeq ($(OS),$(filter $(OS),Linux Darwin))
	CPP_SOURCE += \
		src/LinuxModuleIoFactory.cpp \
		src/LinuxModuleIo.cpp
else # Windows
	CPP_SOURCE += \
		src/WinModuleIoFactory.cpp \
		src/OverlappedModuleIo.cpp
endif



OBJECTS = $(patsubst %.cpp,_build/%.o, $(CPP_SOURCE))
OBJECTS += $(patsubst %.c,_build/%.o, $(C_SOURCE))

CREATE_DEPS = -MMD -MP -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"



.PHONY: all
all: mcpw_demo
	echo "done"

_build/%.o: %.cpp Makefile
	mkdir -p $(@D)
	${GXX} ${GXX_FLAGS} ${CREATE_DEPS} ${INCLUDES} -c $< -o $@

_build/%.o: %.c Makefile
	mkdir -p $(@D)
	$(GCC) ${C_FLAGS} ${INCLUDES} ${CREATE_DEPS} -c $< -o $@

mcpw_demo: $(OBJECTS)
	${GXX} $^ -o $@ ${TARGET_LIB_LINKER_ARGS} -lpthread

.PHONY: clean
clean:
	-rm -r _build
	-rm mcpw_demo.exe
	-rm mcpw_demo


C_DEPS := $(patsubst %.o,%.d, $(OBJECTS))

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif
