CXX=gcc
CXXFLAGS=-I.
CXXFLAGS += -g -Wall
LIBS +=
OBJDIR=obj/
SOURCES = main.cpp
OBJS = $(addprefix $(OBJDIR), $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
EXE = main

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
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) -o $(EXE) 

all:$(EXE)
	@echo Build complete for Linux $(shell git describe --dirty --always --tags)
	./main

.PHONY: clean
clean:
	rm -rf $(EXE) $(OBJS) $(OBJDIR)
