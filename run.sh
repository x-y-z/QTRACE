#!/bin/bash
rm -rf $PWD/qemu.log
$PWD/x86_64-softmmu/qemu-system-x86_64 -m 1024 -hda ~/disks/ubuntu-natty.qcow2  -nographic

#endif
