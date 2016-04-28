MODULE := engines/gnap

MODULE_OBJS := \
	datarchive.o \
	debugger.o \
	detection.o \
	gamesys.o \
	gnap.o \
	grid.o \
	menu.o \
	resource.o \
	sound.o \
	scenes/groupcs.o \
	scenes/group0.o \
	scenes/group1.o \
	scenes/group2.o \
	scenes/group3.o \
	scenes/group4.o \
	scenes/intro.o \
	scenes/scene50.o \
	scenes/scene51.o \
	scenes/scene52.o \
	scenes/scene53.o \
	scenes/scenecore.o

# This module can be built as a plugin
ifeq ($(ENABLE_GNAP), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
