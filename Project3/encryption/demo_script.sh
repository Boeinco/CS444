#!/bin/bash
echo "===DEMOSCRIPT=== : building module ebd.ko"
make #run the makefile

echo
echo "===DEMOSCRIPT=== : Please start VM. Press any key to continue..."

read -n 1 s

echo "===DEMOSCRIPT=== : Transferring file to VM..."
./transfer_file.sh ebd.ko assign3
echo "===DEMOSCRIPT=== : File transferred to VM. Press any key to continue..."

echo "===DEMOSCRIPT=== : Currently installed modules:"
ssh -p 8022 root@localhost lsmod


echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : Installing module on VM..."
ssh -p 8022 root@localhost 'insmod ./assign3/ebd.ko'
echo "===DEMOSCRIPT=== : Currently installed modules:"
ssh -p 8022 root@localhost lsmod
echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : Making filesystem on /dev/ebd0..."
ssh -p 8022 root@localhost mkfs.ext2 /dev/ebd0
echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : Mounting /dev/ebd0 to /mnt..."
ssh -p 8022 root@localhost mount /dev/ebd0 /mnt
echo "===DEMOSCRIPT=== : Is ebd0 mounted?" 
ssh -p 8022 root@localhost 'mount -l | grep "ebd"' 
echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : File writing to /mnt..." 
ssh -p 8022 root@localhost 'echo "lorem ipsum" > /mnt/tryme'
echo "===DEMOSCRIPT=== : Contents of /mnt:" 
ssh -p 8022 root@localhost 'ls /mnt'
echo "===DEMOSCRIPT=== : READING OUT CONTENTS OF FILE ON /mnt/tryme:"
ssh -p 8022 root@localhost 'cat  /mnt/tryme' -v
echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : Unmounting /dev/ebd0"
ssh -p 8022 root@localhost umount /dev/ebd0
echo "===DEMOSCRIPT=== : Contents of /mnt:" 
ssh -p 8022 root@localhost 'ls /mnt'
echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : Shredding /dev/ebd0"
ssh -p 8022 root@localhost 'shred -z /dev/ebd0'


echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : Making filesystem on /dev/ebd0..."
ssh -p 8022 root@localhost mkfs.ext2 /dev/ebd0


echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : SEARCHING FOR CONTENTS OF FILE /mnt/tryme ON /mnt:"
ssh -p 8022 root@localhost 'grep -r -a "lorem ipsum" /mnt/ || echo "NO MATCHES FOUND! Filesystem is encrypted."'


echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : Removing mod ebd.ko" 
ssh -p 8022 root@localhost rmmod ebd.ko


echo "===DEMOSCRIPT=== : Press any key to continue..."
read -n 1 s

echo "===DEMOSCRIPT=== : Currently installed modules:"
ssh -p 8022 root@localhost lsmod
