cmd_/datadisk/dev_mys_y6ulx/nfs_rootfs/usr/include/misc/.install := /bin/bash scripts/headers_install.sh /datadisk/dev_mys_y6ulx/nfs_rootfs/usr/include/misc ./include/uapi/misc cxl.h; /bin/bash scripts/headers_install.sh /datadisk/dev_mys_y6ulx/nfs_rootfs/usr/include/misc ./include/misc ; /bin/bash scripts/headers_install.sh /datadisk/dev_mys_y6ulx/nfs_rootfs/usr/include/misc ./include/generated/uapi/misc ; for F in ; do echo "\#include <asm-generic/$$F>" > /datadisk/dev_mys_y6ulx/nfs_rootfs/usr/include/misc/$$F; done; touch /datadisk/dev_mys_y6ulx/nfs_rootfs/usr/include/misc/.install