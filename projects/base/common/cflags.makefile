
export MAKEFLAGS="-j 8" 

include $(PROJECT_ROOT)/.config

# path to autoconf.h
CFLAGS += -I$(PROJECT_ROOT)/include
#CFLAGS += -ggdb
CFLAGS += -O2
CFLAGS += -Wall
#CFLAGS += -fvisibility=hidden
CFLAGS += -std=gnu99
