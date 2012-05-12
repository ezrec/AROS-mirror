/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   /home/jmcmullan/private/dragonfly/sys/bus/pci/pci_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#include <sys/param.h>
#include <sys/queue.h>
#include <sys/kernel.h>
#include <sys/kobj.h>
#include <sys/bus.h>
#include "pci_if.h"


static int
null_msi_count(device_t dev, device_t child)
{
	return (0);
}

struct kobjop_desc pci_read_config_desc = {
	0, { &pci_read_config_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_write_config_desc = {
	0, { &pci_write_config_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_get_powerstate_desc = {
	0, { &pci_get_powerstate_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_set_powerstate_desc = {
	0, { &pci_set_powerstate_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_get_vpd_ident_desc = {
	0, { &pci_get_vpd_ident_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_get_vpd_readonly_desc = {
	0, { &pci_get_vpd_readonly_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_enable_busmaster_desc = {
	0, { &pci_enable_busmaster_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_disable_busmaster_desc = {
	0, { &pci_disable_busmaster_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_enable_io_desc = {
	0, { &pci_enable_io_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_disable_io_desc = {
	0, { &pci_disable_io_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_assign_interrupt_desc = {
	0, { &pci_assign_interrupt_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_find_extcap_desc = {
	0, { &pci_find_extcap_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_alloc_msi_desc = {
	0, { &pci_alloc_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_release_msi_desc = {
	0, { &pci_release_msi_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_alloc_msix_vector_desc = {
	0, { &pci_alloc_msix_vector_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_release_msix_vector_desc = {
	0, { &pci_release_msix_vector_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc pci_msi_count_desc = {
	0, { &pci_msi_count_desc, (kobjop_t)null_msi_count }
};

struct kobjop_desc pci_msix_count_desc = {
	0, { &pci_msix_count_desc, (kobjop_t)null_msi_count }
};

