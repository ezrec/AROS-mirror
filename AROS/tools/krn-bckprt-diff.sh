#!/bin/bash

V1TREE=$1;
V0TREE=$2;

mydiff()
{
	diff -urN $V1TREE/$1 $V0TREE/$1
}

echo "Diffs generated between source V1 tree and V0 tree after backport"
echo "Generate on `date` by tools/krn-bckprt-diff.sh"
echo "-----------------------------------------------------------------"
echo

mydiff "/arch/all-hosted/bootstrap"
mydiff "/arch/all-hosted/bootstrap"
mydiff "/arch/all-linux/bootstrap"
mydiff "/arch/all-linux/kernel"
mydiff "/arch/all-mingw32/boot"
mydiff "/arch/all-mingw32/bootstrap"
mydiff "/arch/all-mingw32/exec"
mydiff "/arch/all-mingw32/include"
mydiff "/arch/all-mingw32/kernel"
mydiff "/arch/all-mingw32/timer"
mydiff "/arch/all-native/bootconsole"
mydiff "/arch/all-native/kernel"
mydiff "/arch/all-pc/acpi"
mydiff "/arch/all-pc/bootstrap"
mydiff "/arch/all-pc/exec"
mydiff "/arch/all-pc/hpet"
mydiff "/arch/all-pc/kernel"
mydiff "/arch/all-pc/timer"
mydiff "/arch/all-unix/boot"
mydiff "/arch/all-unix/bootstrap"
mydiff "/arch/all-unix/exec"
mydiff "/arch/all-unix/kernel"
mydiff "/arch/all-unix/timer"
mydiff "/arch/i386-all/exec"
mydiff "/arch/i386-all/kernel"
mydiff "/arch/i386-pc/boot"
mydiff "/arch/i386-pc/bootstrap"
mydiff "/arch/i386-pc/exec"
mydiff "/arch/i386-pc/kernel"
mydiff "/bootstrap"
mydiff "/compiler/include/exec/tasks.h"
mydiff "/rom/debug"
mydiff "/rom/efi"
mydiff "/rom/exec"
mydiff "/rom/kernel"
mydiff "/rom/timer"
mydiff "/workbench/libs/uuid"

