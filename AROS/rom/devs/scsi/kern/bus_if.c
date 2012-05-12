/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   dragonfly/sys/kern/bus_if.m
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
#include "bus_if.h"


static struct resource *
null_alloc_resource(device_t dev, device_t child,
		    int type, int *rid,
		    u_long start, u_long end,
		    u_long count, u_int flags, int cpuid)
{
    return 0;
}

struct kobjop_desc bus_print_child_desc = {
	0, { &bus_print_child_desc, (kobjop_t)bus_generic_print_child }
};

struct kobjop_desc bus_probe_nomatch_desc = {
	0, { &bus_probe_nomatch_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_read_ivar_desc = {
	0, { &bus_read_ivar_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_write_ivar_desc = {
	0, { &bus_write_ivar_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_child_detached_desc = {
	0, { &bus_child_detached_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_driver_added_desc = {
	0, { &bus_driver_added_desc, (kobjop_t)bus_generic_driver_added }
};

struct kobjop_desc bus_add_child_desc = {
	0, { &bus_add_child_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_alloc_resource_desc = {
	0, { &bus_alloc_resource_desc, (kobjop_t)null_alloc_resource }
};

struct kobjop_desc bus_activate_resource_desc = {
	0, { &bus_activate_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_deactivate_resource_desc = {
	0, { &bus_deactivate_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_release_resource_desc = {
	0, { &bus_release_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_setup_intr_desc = {
	0, { &bus_setup_intr_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_teardown_intr_desc = {
	0, { &bus_teardown_intr_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_enable_intr_desc = {
	0, { &bus_enable_intr_desc, (kobjop_t)bus_generic_enable_intr }
};

struct kobjop_desc bus_disable_intr_desc = {
	0, { &bus_disable_intr_desc, (kobjop_t)bus_generic_disable_intr }
};

struct kobjop_desc bus_set_resource_desc = {
	0, { &bus_set_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_get_resource_desc = {
	0, { &bus_get_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_delete_resource_desc = {
	0, { &bus_delete_resource_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_get_resource_list_desc = {
	0, { &bus_get_resource_list_desc, (kobjop_t)bus_generic_get_resource_list }
};

struct kobjop_desc bus_child_present_desc = {
	0, { &bus_child_present_desc, (kobjop_t)bus_generic_child_present }
};

struct kobjop_desc bus_child_pnpinfo_str_desc = {
	0, { &bus_child_pnpinfo_str_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_child_location_str_desc = {
	0, { &bus_child_location_str_desc, (kobjop_t)kobj_error_method }
};

struct kobjop_desc bus_config_intr_desc = {
	0, { &bus_config_intr_desc, (kobjop_t)bus_generic_config_intr }
};

