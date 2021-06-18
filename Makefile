VERBOSE = @

BUILDDIR ?= .build
SOURCE_FOLDER = src
CXX = g++
CXXFLAGS ?= -Og -g
CXXFLAGS += -I include
CXXFLAGS += -ffunction-sections -fdata-sections
CXXFLAGS += -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-stack-protector -mno-red-zone -fno-pic
CXXFLAGS += -nodefaultlibs -nostdlib -nostdinc
CXXFLAGS += -Wall -Wextra -Wno-nonnull-compare -Wno-comment
#   -fno-rtti -fno-exceptions -Wno-write-strings -fno-stack-protector -mno-red-zone
SOURCES = $(shell find $(SOURCE_FOLDER)/ -name "*.cpp")
OBJECTS = $(patsubst $(SOURCE_FOLDER)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))
DEPFILES = $(patsubst $(SOURCE_FOLDER)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.d)) $(patsubst %.cpp,$(BUILDDIR)/%.d,$(wildcard test/*.cpp))
LIBNAME = dlh
TARGET = lib$(LIBNAME).a

all: $(TARGET)

%.a: $(OBJECTS) $(MAKEFILE_LIST)
	ar rcs $@ $^

tests: $(patsubst test/%.cpp,test-%,$(wildcard test/*.cpp))

test-%: test/%.cpp $(TARGET) $(MAKEFILE_LIST)
	@echo "Build		$@ ($<)"
	@mkdir -p $(@D)
	$(VERBOSE) $(CXX) $(CXXFLAGS) -no-pie -o $@ $< -L. -l$(LIBNAME)

$(BUILDDIR)/test/%.d: test/%.cpp $(MAKEFILE_LIST)
	@echo "DEP		$<"
	@mkdir -p $(@D)
	$(VERBOSE) $(CXX) $(CXXFLAGS) -MM -MT $(BUILDDIR)/$*.o -MF $@ $<

$(BUILDDIR)/%.d: $(SOURCE_FOLDER)/%.cpp $(MAKEFILE_LIST)
	@echo "DEP		$<"
	@mkdir -p $(@D)
	$(VERBOSE) $(CXX) $(CXXFLAGS) -MM -MT $(BUILDDIR)/$*.o -MF $@ $<

$(BUILDDIR)/%.o: $(SOURCE_FOLDER)/%.cpp $(MAKEFILE_LIST)
	@echo "CXX		$<"
	@mkdir -p $(@D)
	$(VERBOSE) $(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@echo "RM		$(BUILDDIR)"
	$(VERBOSE) rm -rf $(BUILDDIR)


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPFILES)
endif

# Phony targets
.PHONY: all clean
