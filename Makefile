VERBOSE = @

BUILDDIR ?= .build
SOURCE_FOLDER = src
CXX = g++
CXXFLAGS := -Og -g -I include
CXXFLAGS += -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-pic
CXXFLAGS += -nodefaultlibs -nostdlib -nostdinc
CXXFLAGS += -Wall -Wextra -Wno-nonnull-compare -Wno-comment
#   -fno-rtti -fno-exceptions -Wno-write-strings -fno-stack-protector -mno-red-zone
SOURCES = $(shell find $(SOURCE_FOLDER)/ -name "*.cpp")
OBJECTS = $(patsubst $(SOURCE_FOLDER)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))
DEPFILES = $(patsubst $(SOURCE_FOLDER)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.d))
TARGET = libdlh.a

all: $(TARGET)

%.a: $(OBJECTS)
	ar rcs $@ $^

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
-include $(DEP_FILES)
endif

# Phony targets
.PHONY: all clean
