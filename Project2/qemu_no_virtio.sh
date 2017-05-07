source env_setup_file

qemu-system-i386 -gdb tcp::5628 -S -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3 -enable-kvm -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug" -redir tcp:8022::22
