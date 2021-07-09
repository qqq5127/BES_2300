# Use relative path to specify the include path (so that no change is needed after rename)
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))


obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o)

ccflags-y := -I$(obj)/inc

