MODULE := engines/hanekawa

MODULE_OBJS = \
			  hanekawa.o \
			  console.o \
			  sound.o \
			  graphics.o


# this module can be build as a plugin
ifeq($(ENABLE_HANEKAWA), DYNAMIC_PLUGIN)
PLUGIN := 1
endif 

# Include common rules
include $(srcdir)/rules.mk
