
PROJECT_ROOT := $(shell pwd)

export PROJECT_ROOT


all:
	@make -f make.build

config:
	@./menuconfig Config.in

clean:
	@make -f make.build clean

distclean: clean
	@rm -f .config .config.old .kconfig.d include/autoconf.h


base_defconfig:
	@cp configs/base_defconfig ./.config
	@./menuconfig Config.in "no_menu"

