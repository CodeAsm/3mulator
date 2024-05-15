CXX=gcc
CXXFLAGS=-I.$(addprefix -I,$(INC_DIRS))
CXXFLAGS += -g -Wall
LIBS +=
OBJDIR=obj/
SRC_DIRS=hw/ ui/ cores/ include/
# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := 

SOURCES = main.cpp
OBJS = $(addprefix $(OBJDIR), $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
EXE = 3mu

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    # (-g)  => Enable debug symbols -O0)
    CXXFLAGS  += -g -O0
else
     CXXFLAGS += -O3
endif

$(OBJDIR)%.o:%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) 

all:$(EXE)
	@echo Build complete for Linux $(shell git describe --dirty --always --tags)
	./3mu

.PHONY: clean
clean:
	rm -rf $(EXE) $(OBJS) $(OBJDIR)
