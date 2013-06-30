/*
This file is part of topiary.

 topiary is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 topiary is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with topiary.  If not, see <http://www.gnu.org/licenses/>.

 Copyright 2013 Paul Hewlett, phewlett76@gmail.com

typical contents of /proc/mounts file::

rootfs / rootfs rw 0 0
sysfs /sys sysfs rw,nosuid,nodev,noexec,relatime 0 0
proc /proc proc rw,nosuid,nodev,noexec,relatime 0 0
udev /dev devtmpfs rw,relatime,size=8191108k,nr_inodes=2047777,mode=755 0 0
devpts /dev/pts devpts rw,nosuid,noexec,relatime,gid=5,mode=620,ptmxmode=000 0 0
tmpfs /run tmpfs rw,nosuid,noexec,relatime,size=1641696k,mode=755 0 0
/dev/mapper/ubuntu-root / ext4 rw,relatime,errors=remount-ro,data=ordered 0 0
none /sys/fs/cgroup tmpfs rw,relatime,size=4k,mode=755 0 0
none /sys/fs/fuse/connections fusectl rw,relatime 0 0
none /sys/kernel/debug debugfs rw,relatime 0 0
none /sys/kernel/security securityfs rw,relatime 0 0
none /sys/firmware/efi/efivars efivarfs rw,relatime 0 0
tmpfs /tmp tmpfs rw,nosuid,nodev,noatime 0 0
none /run/lock tmpfs rw,nosuid,nodev,noexec,relatime,size=5120k 0 0
cgroup /sys/fs/cgroup/cpuset cgroup rw,relatime,cpuset 0 0
none /run/shm tmpfs rw,nosuid,nodev,relatime 0 0
none /run/user tmpfs rw,nosuid,nodev,noexec,relatime,size=102400k,mode=755 0 0
cgroup /sys/fs/cgroup/cpu cgroup rw,relatime,cpu 0 0
cgroup /sys/fs/cgroup/cpuacct cgroup rw,relatime,cpuacct 0 0
cgroup /sys/fs/cgroup/memory cgroup rw,relatime,memory 0 0
cgroup /sys/fs/cgroup/devices cgroup rw,relatime,devices 0 0
cgroup /sys/fs/cgroup/freezer cgroup rw,relatime,freezer 0 0
cgroup /sys/fs/cgroup/blkio cgroup rw,relatime,blkio 0 0
cgroup /sys/fs/cgroup/perf_event cgroup rw,relatime,perf_event 0 0
cgroup /sys/fs/cgroup/hugetlb cgroup rw,relatime,hugetlb 0 0
/dev/sda2 /boot ext2 rw,relatime,errors=continue 0 0
/dev/sda1 /boot/efi vfat rw,relatime,fmask=0022,dmask=0022,codepage=437,iocharset=iso8859-1,shortname=mixed,errors=remount-ro 0 0
/dev/mapper/vm-chroot /srv/chroot ext4 rw,relatime,errors=remount-ro,data=ordered 0 0
/dev/mapper/vg-images /var/lib/libvirt ext4 rw,noatime,data=ordered 0 0
/dev/mapper/vg-home /home ext4 rw,relatime,errors=remount-ro,data=ordered 0 0
binfmt_misc /proc/sys/fs/binfmt_misc binfmt_misc rw,nosuid,nodev,noexec,relatime 0 0
rpc_pipefs /run/rpc_pipefs rpc_pipefs rw,relatime 0 0
nfsd /proc/fs/nfsd nfsd rw,relatime 0 0
/dev/mapper/vm-chroot /var/lib/schroot/mount/quantal_i386-2067f996-115f-4d50-8c79-0ce81663e317 ext4 rw,relatime,errors=remount-ro,data=ordered 0 0
proc /var/lib/schroot/mount/quantal_i386-2067f996-115f-4d50-8c79-0ce81663e317/proc proc rw,nosuid,nodev,noexec,relatime 0 0
sysfs /var/lib/schroot/mount/quantal_i386-2067f996-115f-4d50-8c79-0ce81663e317/sys sysfs rw,nosuid,nodev,noexec,relatime 0 0
udev /var/lib/schroot/mount/quantal_i386-2067f996-115f-4d50-8c79-0ce81663e317/dev devtmpfs rw,relatime,size=8191108k,nr_inodes=2047777,mode=755 0 0
devpts /var/lib/schroot/mount/quantal_i386-2067f996-115f-4d50-8c79-0ce81663e317/dev/pts devpts rw,nosuid,noexec,relatime,gid=5,mode=620,ptmxmode=000 0 0
/dev/mapper/vg-home /var/lib/schroot/mount/quantal_i386-2067f996-115f-4d50-8c79-0ce81663e317/home ext4 rw,relatime,errors=remount-ro,data=ordered 0 0
tmpfs /var/lib/schroot/mount/quantal_i386-2067f996-115f-4d50-8c79-0ce81663e317/tmp tmpfs rw,nosuid,nodev,noatime 0 0

*/

#include "config.h"

#include "topiary_stdlib.h"
#include <stdio.h>
#include <string.h>

#include "strlcpy.h"
#include "val2str.h"
#include "error_private.h"
#include "limits_private.h"
#include "array_private.h"
#include "results_private.h"
#include "interface_private.h"
#include "base_private.h"
#include "modules.h"

static int
proc_mounts_normalize(_TOPIARY_BASE_T *base)
{
    _TOPIARY_RESULTS_T *results = base->current->results;
    _TOPIARY_ARRAY_T *array = base->current->array;

    size_t nrows = array->vector->length;

    _TOPIARY_DEBUG("Nrows %zd\n", nrows);
    char ***values = (char ***) array->vector->values;
    char *val;

    int i, j;

    static const char *titles[] =
        { "spec", "file", "vfstype", "mntops", "freq", "passno", };

    char buf[64];

    int n1 = 0;

    STRLCAT(buf, "/", n1, sizeof(buf));

    _topiary_results_init(results, nrows);
    for (i = 0; i < nrows; i++) {
        char **value1 = (char **) values[i];
        size_t ncols = array->vector->children[i]->length;

        _TOPIARY_DEBUG("%d:Ncols %zd\n", i, ncols);

        int n2 = n1;

        INTCAT(buf, i, n2, sizeof(buf));
        STRLCAT(buf, "/", n2, sizeof(buf));

        for (j = 0; j < ncols; j++) {
            val = value1[j];

            if (!val)
                continue;

            _TOPIARY_DEBUG("%d,%d:Val %s\n", i, j, val);

            int n3 = n2;

            STRLCAT(buf, titles[j], n3, sizeof(buf));
            _TOPIARY_DEBUG("%d,%d:Key %s\n", i, j, buf);
            if (j > 3) {
                _topiary_results_add_int(results, buf, atoi(val));
            }
            else {
                _topiary_results_add_stringref(results, buf, val);
            }
        }
    }
    return TOPIARY_OK;
}

int
_topiary_proc_mounts_new(_TOPIARY_BASE_T **base, TOPIARY_OPT_T *optional)
{

    _TOPIARY_LIMITS_T *limits = NULL;
    int ret = _topiary_limits_new(&limits, 2);

    if (ret) {
        return ret;
    }
    ret = _topiary_limits_set(limits, 0, 9, "\f\n", 2); /* row delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }
    ret = _topiary_limits_set(limits, 1, 4, " ", 1);    /* column delimiters */
    if (ret) {
        _TOPIARY_LIMITS_FREE(limits);
        return ret;
    }

    char *filenames[] = { "/proc/mounts" };
    ret = _topiary_base_new(base, "proc_mounts", _TOPIARY_BASE_TYPE_VANILLA,
                            NULL, proc_mounts_normalize, NULL, 256, limits);
    if (!ret) {
        ret = _topiary_base_set_filenames(*base, filenames, 1);
    }
    _TOPIARY_LIMITS_FREE(limits);
    return ret;
}

/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
