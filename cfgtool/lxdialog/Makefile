# Makefile to build lxdialog package
#

check-lxdialog  := sh ./check-lxdialog.sh

# Use reursively expanded variables so we do not call gcc unless
# we really need to do so. (Do not call gcc as part of make mrproper)
HOST_EXTRACFLAGS = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags)
HOST_LOADLIBES   = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))

HOST_EXTRACFLAGS += -DLOCALE

#PHONY += dochecklxdialog
#$(obj)/dochecklxdialog:


#echo "$(HOSTCC) $(HOST_EXTRACFLAGS) $(HOST_LOADLIBES)"

#$(Q)$(CONFIG_SHELL) $(check-lxdialog) -check $(HOSTCC) $(HOST_EXTRACFLAGS) $(HOST_LOADLIBES)

hostprogs-y	:= lxdialog
always		:= $(hostprogs-y) dochecklxdialog

lxdialog-objs := checklist.o menubox.o textbox.o yesno.o inputbox.o \
		 util.o lxdialog.o msgbox.o

CFLAGS :=  $(HOST_EXTRACFLAGS)
LDFLAGS := $(HOST_LOADLIBES)


my_objs := $(lxdialog-objs)
#$(hostprogs-y)

all: $(my_objs)
	gcc  $(my_objs) $(HOST_LOADLIBES) -o $(hostprogs-y)

