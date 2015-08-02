CXX      ?= g++
AR       ?= ar
CXXFLAGS ?= -O2 -g3 -Werror -Wno-unused-private-field

# OGRE headers generate some warnings
CXXFLAGS += -std=c++11 -Wall -Wno-overloaded-virtual -Wno-deprecated-register

CXXFLAGS += -Isrc

OGRE_CFLAGS ?= $(shell pkg-config --cflags OGRE OGRE-Overlay)
OGRE_PLUGIN_DIR ?= $(shell pkg-config --variable=plugindir OGRE)
CXXFLAGS += $(OGRE_CFLAGS) -DOGRE_PLUGIN_DIR="\"$(OGRE_PLUGIN_DIR)\""
CXXFLAGS += $(shell pkg-config --cflags OIS)

OGRE_LDFLAGS ?= $(shell pkg-config --libs OGRE OGRE-Overlay)
LDFLAGS  += $(OGRE_LDFLAGS)
LDFLAGS  += $(shell pkg-config --libs OIS)
LDFLAGS  += -lboost_system
LDFLAGS  += -lnoise
LDFLAGS  += -ljsoncpp


BINDIR  = bin
BINNAME = main
BIN     = $(BINDIR)/$(BINNAME)

SRCDIR = src/app

SRCFILES = Constants.cpp Terrain.cpp \
	   Entity.cpp Vehicle.cpp Missile.cpp Plane.cpp LandVehicle.cpp SAM.cpp Tank.cpp \
	   General.cpp \
	   UserInput.cpp InputHandler.cpp GeneralInput.cpp AIController.cpp Game.cpp \
	   MouseCursor.cpp TextRenderer.cpp UserInterface.cpp main.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.dep)

# Common lib

COMMONSRCDIR = src/common
COMMONLIB = $(COMMONSRCDIR)/libcommon.a
COMMONSRCS = $(shell (find $(COMMONSRCDIR) \( -name '*.cpp' -o -name '*.h' \)))


.PHONY: clean all

all: $(BIN)

$(COMMONLIB): $(COMMONSRCS)
	make -C $(COMMONSRCDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BIN): $(BINDIR) $(COMMONLIB) $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(COMMONLIB) -o $(BIN)

%.dep: %.cpp
	@rm -f $@
	@$(CC) -MM $(CXXFLAGS) $< > $@.P
	@sed 's,\($(notdir $*)\)\.o[ :]*,$(dir $*)\1.o $@ : ,g' < $@.P > $@
	@rm -f $@.P

clean:
	make -C $(COMMONSRCDIR) clean
	rm -f $(SRCDIR)/*.o $(SRCDIR)/*.dep $(BIN)
	rmdir $(BINDIR)

-include $(DEPS)

