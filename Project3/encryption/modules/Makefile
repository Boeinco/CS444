obj-m := sbd.o

KERN_DIR :=/scratch/spring2017/12-08/linux-yocto-3.14

all:
	make -C $(KERN_DIR) M=$(PWD) modules 

modules_install:
	make -C $(KERN_dir) M=$(PWD) modules_install
