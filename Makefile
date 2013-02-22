CXX      ?= g++
AR       ?= ar
CXXFLAGS ?= -std=c++11 -O2 -g3 -Werror

# OGRE headers generate lots of hiding overloaded virtual function warnings
CXXFLAGS += -Wall -Wno-overloaded-virtual

OGRE_CFLAGS ?= $(shell pkg-config --cflags OGRE)
OGRE_PLUGIN_DIR ?= $(shell pkg-config --variable=plugindir OGRE)
CXXFLAGS += $(OGRE_CFLAGS) -DOGRE_PLUGIN_DIR="\"$(OGRE_PLUGIN_DIR)\""
CXXFLAGS += $(shell pkg-config --cflags OIS)

OGRE_LDFLAGS ?= $(shell pkg-config --libs OGRE)
LDFLAGS  += $(OGRE_LDFLAGS)
LDFLAGS  += $(shell pkg-config --libs OIS)
LDFLAGS  += -lboost_system


BINDIR  = bin
BINNAME = main
BIN     = $(BINDIR)/$(BINNAME)

SRCDIR = src

SRCFILES = main.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.dep)

.PHONY: clean all

all: $(BIN)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BIN): $(BINDIR) $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(BIN)

%.dep: %.cpp
	@rm -f $@
	@$(CC) -MM $(CPPFLAGS) $< > $@.P
	@sed 's,\($(notdir $*)\)\.o[ :]*,$(dir $*)\1.o $@ : ,g' < $@.P > $@
	@rm -f $@.P

clean:
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*.dep $(BIN)
	rmdir $(BINDIR)

-include $(DEPS)

