VERBOSE = @

SRCFOLDER = src
BUILDDIR ?= .build

AR ?= ar
CXX ?= g++

CXXFLAGS ?= -Og -g
CXXFLAGS += -I include
CXXFLAGS += -fno-builtin -ffunction-sections -fdata-sections
CXXFLAGS += -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-stack-protector -mno-red-zone -fno-pic
CXXFLAGS += -nodefaultlibs -nostdlib -nostdinc
CXXFLAGS += -Wall -Wextra -Wno-nonnull-compare -Wno-comment

SOURCES = $(shell find $(SRCFOLDER)/ -name "*.cpp")
OBJECTS = $(patsubst $(SRCFOLDER)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))
DEPFILES = $(patsubst $(SRCFOLDER)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.d)) $(patsubst %.cpp,$(BUILDDIR)/%.d,$(wildcard test/*.cpp))
LIBNAME = dlh
TARGET = lib$(LIBNAME).a
TESTS := $(patsubst test/%.cpp,test-%,$(wildcard test/*.cpp))

all: $(TARGET)

%.a: $(OBJECTS) $(MAKEFILE_LIST)
	@echo "AR		$@"
	$(VERBOSE) $(AR) rcs $@ $^

tests: $(TESTS)

test-%: test/%.cpp $(TARGET) $(MAKEFILE_LIST)
	@echo "Build		$@ ($<)"
	$(VERBOSE) $(CXX) $(CXXFLAGS) -no-pie -o $@ $< -L. -l$(LIBNAME)

$(BUILDDIR)/test/%.d: test/%.cpp $(BUILDDIR) $(MAKEFILE_LIST)
	@echo "DEP		$<"
	@mkdir -p $(@D)
	$(VERBOSE) $(CXX) $(CXXFLAGS) -MM -MP -MT test-$* -MF $@ $<

$(BUILDDIR)/%.d: $(SRCFOLDER)/%.cpp $(BUILDDIR) $(MAKEFILE_LIST)
	@echo "DEP		$<"
	@mkdir -p $(@D)
	$(VERBOSE) $(CXX) $(CXXFLAGS) -MM -MP -MT $(BUILDDIR)/$*.o -MF $@ $<

$(BUILDDIR)/%.o: $(SRCFOLDER)/%.cpp $(BUILDDIR) $(MAKEFILE_LIST)
	@echo "CXX		$<"
	@mkdir -p $(@D)
	$(VERBOSE) $(CXX) $(CXXFLAGS) -c -o $@ $<

clean::
	$(VERBOSE) rm -rf $(BUILDDIR)
	$(VERBOSE) test -d $(BUILDDIR) && rmdir $(BUILDDIR) || true

mrproper:: clean
	$(VERBOSE) rm -f $(TESTS) $(TARGET)

$(BUILDDIR): ; @mkdir -p $@

$(DEPFILES):

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPFILES)
endif

.PHONY: all tests clean mrproper
