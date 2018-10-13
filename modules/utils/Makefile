include $(PROJECT_ROOT)/common/cflags.makefile

BIN_TARGET-$(CONFIG_LIBUTILS)	= libutils.so
LIB_SHARED = $(BIN_TARGET-y)
#Always build
obj-$(CONFIG_LIBUTILS)		+= debug.o
obj-$(CONFIG_LIBUTILS)		+= gc.o
obj-$(CONFIG_LIBUTILS)		+= sobj.o

CFLAGS		+= -fPIC
CFLAGS		+= $(INCLUDES-y)

LDFLAGS 	+= -shared

LDFLAGS		+= $(LIBS-y)

include $(PROJECT_ROOT)/common/compile.makefile
