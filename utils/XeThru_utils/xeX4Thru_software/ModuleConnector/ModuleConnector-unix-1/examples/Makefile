ARCH_TRIPLET = $(shell $(CXX) -dumpmachine)

CXX11_ABI = 1

ifneq ($(filter x86_64%-linux-gnu,$(ARCH_TRIPLET)),)
LIBRARY_PATH = ../lib
SO_SUFFIX = .so
else ifneq ($(filter arm-linux-gnueabihf,$(ARCH_TRIPLET)),)
LIBRARY_PATH = ../lib
SO_SUFFIX = .so
else ifneq ($(filter x86_64-apple-darwin%,$(ARCH_TRIPLET)),)
LIBRARY_PATH = ../lib
SO_SUFFIX = .dylib
else ifneq ($(filter x86_64-%-mingw32,$(ARCH_TRIPLET)),)
LIBRARY_PATH = ../lib64
EXE_SUFFIX = .exe
SO_SUFFIX = .dll
else ifneq ($(filter i686-%-mingw32,$(ARCH_TRIPLET)),)
LIBRARY_PATH = ../lib32
EXE_SUFFIX = .exe
SO_SUFFIX = .dll
else
$(error Bad target $(ARCH_TRIPLET))
endif

CXXFLAGS = -D_GLIBCXX_USE_CXX11_ABI=$(CXX11_ABI) --std=gnu++11 -Wall -Werror -I../include

SRCS = $(wildcard *.cpp)
EXES = $(patsubst %.cpp,%$(EXE_SUFFIX),$(SRCS))

.PHONY: all
all: $(EXES)
	@echo "Done."

ifdef SYSTEM_MC
MC_LIB =
LDLIBS = -lModuleConnector
LDFLAGS = -L$(LIBRARY_PATH)
else
MC_LIB = ./libModuleConnector$(SO_SUFFIX)
$(MC_LIB):
	cp $(LIBRARY_PATH)/$(MC_LIB) .
endif

%$(EXE_SUFFIX) : %.cpp | $(MC_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(MC_LIB)

.PHONY: clean
clean:
	rm -f $(EXES) $(MC_LIB)
