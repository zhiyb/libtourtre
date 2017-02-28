#########################################################
##                                                     ##
##       Generic makefile for simple projects.         ## 
##                                                     ##
##  Makes one executable, named with the EXE variable. ##
##   All source goes in a flat directory, named ./src  ##
##                                                     ##
#########################################################

# Sample data
DATA	= sampledata/CThead.256x256x113.uint16
DATA	= sampledata/CThead-2by2.256x512x226.uint16
#DATA	= sampledata/CThead-2by4.256x512x452.uint16

GDB_DATA	= sampledata/bucky.32x32x32.uint8

#name of the executable
EXE = simple

# libtourtre dir
TOURTRE_DIR	= ../..

#include directories
INCLUDE_DIRS = -I$(TOURTRE_DIR)/include

#compiler flags
COMPILE_FLAGS = -O3 $(DEBUG_FLAGS)

#lib directories
LIB_DIRS = -L$(TOURTRE_DIR)

#linker flags
LINK_FLAGS = $(TOURTRE_DIR)/libtourtre.a -lz -ltbb $(DEBUG_FLAGS)

#preprocessor defines
DEFINES = -DUSE_ZLIB

#source suffixes to scan for
SOURCE_SUFFIXES = cpp cc

#directory to scan
SOURCE_DIR = src

#temporaries dir (object files, moc files, etc)
OBJ_DIR = obj

SHELL	= /bin/bash

#targets
.DELETE_ON_ERROR:

.PHONY: all
all:
	cd $(TOURTRE_DIR); $(MAKE) $@
	$(MAKE) $(EXE)

$(EXE): $(TOURTRE_DIR)/libtourtre.a

$(TOURTRE_DIR)/libtourtre.a:
	cd $(TOURTRE_DIR); $(MAKE) all

.PHONY: test
test:	$(EXE)
	(time ./$(EXE) -c -i $(GDB_DATA)) 2>&1 | tee $(EXE).log

.PHONY: gdb
gdb:
	$(MAKE) clean
	$(MAKE) all DEBUG_FLAGS="-pg -g -O0"
	gdb --tui $(EXE) -ex "break main" -ex "run -c -i $(GDB_DATA)"
	$(MAKE) clean

.PHONY: gprof
gprof:
	$(MAKE) clean
	$(MAKE) all DEBUG_FLAGS="-pg -g -O2"
	(time ./$(EXE) -c -i $(DATA)) 2>&1 | tee $(EXE).gprof.log
	gprof $(EXE) > $(EXE).gprof
	$(MAKE) clean

AVERAGE	?= 6
JOBS	?= -j1

SEQ	:= $(shell seq 1 $(AVERAGE))
LOGS	:= $(SEQ:%=$(EXE).%.log)

.PHONY: time
time:
	$(MAKE) all
	$(MAKE) $(EXE).time $(JOBS)

$(EXE).time: $(LOGS)
	grep -F -h real $(LOGS) | awk '{print $$2}' | sed 's/m/*60+/g;s/s//g' | bc | xargs | tee $@

$(EXE).%.log: $(EXE)
	(time ./$< -c -i $(DATA)) 2>&1 | tee $@;

.PHONY: clean
clean: 
	-cd $(TOURTRE_DIR); $(MAKE) $@
	-rm -f obj/* $(LOGS)
	-rm -f $(EXE)

.PHONY: remake
remake:
	$(MAKE) clean
	$(MAKE) all

#standard make vars
CXX = g++
CC = gcc
CFLAGS = $(INCLUDE_DIRS) $(COMPILE_FLAGS)
CXXFLAGS = $(INCLUDE_DIRS) $(COMPILE_FLAGS)
CPPFLAGS = $(DEFINES) $(INCLUDE_DIRS)
LDFLAGS = $(LIB_DIRS) $(LINK_FLAGS)






#####################################################
## everything beyond this point should be automatic,
## assuming all your source files are in ./src .
## For each file in ./src that matches a suffix in 
## SOURCE_SUFFIXES, .o and .d files with the same name
## will be made in ./obj, which must exist. The .o files
## will be used to build EXE, and the .d files will be
## included in the makefile. The .d files contain targets
## for themselves so dependencies should be self-updating

OBJ_FILES = $(foreach suffix,$(SOURCE_SUFFIXES),$(addprefix $(OBJ_DIR)/,$(notdir $(subst .$(suffix),.o,$(wildcard $(SOURCE_DIR)/*.$(suffix))))))
DEP_FILES = $(foreach suffix,$(SOURCE_SUFFIXES),$(addprefix $(OBJ_DIR)/,$(notdir $(subst .$(suffix),.d,$(wildcard $(SOURCE_DIR)/*.$(suffix))))))

$(EXE): $(OBJ_FILES) $(DEP_FILES)
	$(CXX) -o $(EXE) $(filter $(OBJ_DIR)/%.o, $^) $(LDFLAGS)

-include $(DEP_FILES)

#implicit-rule macro for .o files
define OBJ_RULE
$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.$(1) | $(OBJ_DIR) ; $$(CXX) $$(CPPFLAGS) $$(CXXFLAGS) -c -o $$@ $$<
endef

#implicit-rule macro for .d files
define DEP_RULE
$(OBJ_DIR)/%.d: $(SOURCE_DIR)/%.$(1) | $(OBJ_DIR) ; $$(CXX) $$(CPPFLAGS) $$< -MM -MT $$@ -MT $$(subst .d,.o,$$@) -MF $$(subst .o,.d,$$@)
endef

#call the macros for each suffix
$(foreach suffix,$(SOURCE_SUFFIXES),$(eval $(call OBJ_RULE,$(suffix))))
$(foreach suffix,$(SOURCE_SUFFIXES),$(eval $(call DEP_RULE,$(suffix))))

#for reference, this is what the output of these macros would look like if hand-written, for the suffix cpp

$(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@  $<

$(OBJ_DIR)/%.d: $(SOURCE_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $< -MM -MT $@ -MT $(subst .d,.o,$@) -MF $(subst .o,.d,$@)

%.h :
	@echo
	@echo caught unsatisfied header : $@
	@echo
	
