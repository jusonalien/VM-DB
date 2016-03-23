# VM-DB
- Welcome to the project of the VM-DB,VM-DB for the Virtual Machine Database
- Files in this repository are mainly used for detecting and debuging the the path of the qemu disk IO.
- Files in this repository can be used in any where you like.
- If you are interested in this project,you can have a look on this [website](http://jusonalien.gitbooks.io/scnu-vmdb/content/),maybe you should know some Chinese~


# About

We are now adding a cache system under the qemu-kvm virtulazation's block layer to get a higher performance which is named gear-cache,now the cache system is running properly.But it's performance still not achieved the requests :(.

# Architecture

![origin](https://github.com/jusonalien/VM-DB/blob/master/origin.png)

![gear-cache](https://github.com/jusonalien/VM-DB/blob/master/gear-cache-Arch.png)

# How to use it?

just put the source files in the core code directory to the qemu's sorce code(qemu-2.4.0/block),replace the old raw-posix.c with our new raw-posix.c .Then just compile the qemu source code as usual

If you use the instruction script to start the vm,adjust the parameters like this:


```sh
-drive file=centos7.img,if=virtio,aio=threads,cache=none
```
Bypassing the host page cache system,and using our own lightweight cache system on the block layer.

The following script is used to start our vms.

```sh
qemu -enable-kvm \
-m 1024 \
-smp 2 \
-netdev type=tap,script=/etc/qemu-ifup,downscript=no,id=net0 \
-device virtio-net-pci,netdev=net0,mac=00:11:22:33:22:FF \
-drive file=centos7.img,if=virtio,aio=threads,cache=none \
-nographic
```

# License
  [MIT](https://opensource.org/licenses/MIT)
