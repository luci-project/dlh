VERBOSE = @

SRCFOLDER = src
BUILDDIR ?= .build

CPPLINT ?= cpplint
TIDY ?= clang-tidy
TIDYCONFIG ?= .clang-tidy
TIDYIGNORE := %$(SRCFOLDER)/libc/stdlib_qsort.cpp

AR ?= ar
CXX ?= g++

ifeq ($(OPTIMIZE), 1)
	CXXFLAGS := -O3 -DNDEBUG
else
	CXXFLAGS := -Og -g
endif
CXXFLAGS += -I include
CXXFLAGS += -fno-builtin -ffunction-sections -fdata-sections -fvisibility=hidden
CXXFLAGS += -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-stack-protector -mno-red-zone
CXXFLAGS += -fno-jump-tables -fno-plt -fPIE
CXXFLAGS += -nodefaultlibs -nostdlib -nostdinc
CXXFLAGS += -Wall -Wextra -Wno-nonnull-compare -Wno-comment
ifeq ($(LEGACY), 1)
	CXXFLAGS += -DDLH_LEGACY
endif

LIBNAME = dlh
BUILDINFO = $(BUILDDIR)/.build_$(LIBNAME).o
SOURCES = $(shell find $(SRCFOLDER)/ -name "*.cpp")
OBJECTS = $(patsubst $(SRCFOLDER)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o)) $(BUILDINFO)
DEPFILES = $(patsubst $(SRCFOLDER)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.d)) $(patsubst %.cpp,$(BUILDDIR)/%.d,$(wildcard test/*.cpp))
TARGET = lib$(LIBNAME).a
TEST := $(patsubst test/%.cpp,test-%,$(wildcard test/*.cpp))

all: $(TARGET)

%.a: $(OBJECTS) $(MAKEFILE_LIST)
	@echo "AR		$@"
	@rm -f $@
	$(VERBOSE) $(AR) rcs $@ $(OBJECTS)

test: $(TEST)

test-%: test/%.cpp $(TARGET) $(MAKEFILE_LIST)
	@echo "Build		$@ ($<)"
	$(VERBOSE) $(CXX) $(CXXFLAGS) -no-pie -o $@ $< -L. -l$(LIBNAME)

check: $(patsubst test-%,check-%,$(TEST))

check-%: test-%
	$(VERBOSE) test/check.sh $*

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
	$(VERBOSE) $(CXX) $(CXXFLAGS) -D__MODULE__="$(LIBNAME)" -c -o $@ $<

$(BUILDINFO): FORCE
	@echo "CXX		$@"
	@echo 'const char * build_$(LIBNAME)_version() { return "$(shell git describe --dirty --always --tags)"; } ' \
	'const char * build_$(LIBNAME)_date() { return "$(shell date -R)"; }' \
	'const char * build_$(LIBNAME)_flags() { return "$(CXXFLAGS)"; }' | $(CXX) $(CXXFLAGS) -x c++ -c -o $@ -

lint::
	@if $(CPPLINT) --quiet --recursive . ; then \
		echo "Congratulations, coding style obeyed!" ; \
	else \
		echo "Coding style violated -- see CPPLINT.cfg for details" ; \
		exit 1 ; \
	fi

tidy:: $(TIDYCONFIG)
	$(VERBOSE) $(TIDY) --config-file=$< $(filter-out $(TIDYIGNORE),$(SOURCES)) -- -stdlib=libc++  $(CXXFLAGS)

clean::
	$(VERBOSE) rm -rf $(BUILDDIR)
	$(VERBOSE) test -d $(BUILDDIR) && rmdir $(BUILDDIR) || true

mrproper:: clean
	$(VERBOSE) rm -f $(TEST) $(TARGET)

$(BUILDDIR): ; @mkdir -p $@

$(DEPFILES):

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPFILES)
endif

FORCE:

.PHONY: all tests clean mrproper
