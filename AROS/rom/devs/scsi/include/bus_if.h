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

#ifndef _bus_if_h_
#define _bus_if_h_

extern struct kobjop_desc bus_print_child_desc;
typedef int bus_print_child_t(device_t dev, device_t child);
static __inline int BUS_PRINT_CHILD(device_t dev, device_t child)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_print_child);
	return ((bus_print_child_t *) _m)(dev, child);
}

extern struct kobjop_desc bus_probe_nomatch_desc;
typedef void bus_probe_nomatch_t(device_t dev, device_t child);
static __inline void BUS_PROBE_NOMATCH(device_t dev, device_t child)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_probe_nomatch);
	((bus_probe_nomatch_t *) _m)(dev, child);
}

extern struct kobjop_desc bus_read_ivar_desc;
typedef int bus_read_ivar_t(device_t dev, device_t child, int index,
                            uintptr_t *result);
static __inline int BUS_READ_IVAR(device_t dev, device_t child, int index,
                                  uintptr_t *result)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_read_ivar);
	return ((bus_read_ivar_t *) _m)(dev, child, index, result);
}

extern struct kobjop_desc bus_write_ivar_desc;
typedef int bus_write_ivar_t(device_t dev, device_t child, int index,
                             uintptr_t value);
static __inline int BUS_WRITE_IVAR(device_t dev, device_t child, int index,
                                   uintptr_t value)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_write_ivar);
	return ((bus_write_ivar_t *) _m)(dev, child, index, value);
}

extern struct kobjop_desc bus_child_detached_desc;
typedef void bus_child_detached_t(device_t dev, device_t child);
static __inline void BUS_CHILD_DETACHED(device_t dev, device_t child)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_child_detached);
	((bus_child_detached_t *) _m)(dev, child);
}

extern struct kobjop_desc bus_driver_added_desc;
typedef void bus_driver_added_t(device_t dev, driver_t *driver);
static __inline void BUS_DRIVER_ADDED(device_t dev, driver_t *driver)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_driver_added);
	((bus_driver_added_t *) _m)(dev, driver);
}

extern struct kobjop_desc bus_add_child_desc;
typedef device_t bus_add_child_t(device_t bus, device_t parent, int order,
                                 const char *name, int unit);
static __inline device_t BUS_ADD_CHILD(device_t bus, device_t parent, int order,
                                       const char *name, int unit)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)bus)->ops, bus_add_child);
	return ((bus_add_child_t *) _m)(bus, parent, order, name, unit);
}

extern struct kobjop_desc bus_alloc_resource_desc;
typedef struct resource * bus_alloc_resource_t(device_t dev, device_t child,
                                               int type, int *rid, u_long start,
                                               u_long end, u_long count,
                                               u_int flags, int cpuid);
static __inline struct resource * BUS_ALLOC_RESOURCE(device_t dev,
                                                     device_t child, int type,
                                                     int *rid, u_long start,
                                                     u_long end, u_long count,
                                                     u_int flags, int cpuid)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_alloc_resource);
	return ((bus_alloc_resource_t *) _m)(dev, child, type, rid, start, end, count, flags, cpuid);
}

extern struct kobjop_desc bus_activate_resource_desc;
typedef int bus_activate_resource_t(device_t dev, device_t child, int type,
                                    int rid, struct resource *r);
static __inline int BUS_ACTIVATE_RESOURCE(device_t dev, device_t child,
                                          int type, int rid, struct resource *r)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_activate_resource);
	return ((bus_activate_resource_t *) _m)(dev, child, type, rid, r);
}

extern struct kobjop_desc bus_deactivate_resource_desc;
typedef int bus_deactivate_resource_t(device_t dev, device_t child, int type,
                                      int rid, struct resource *r);
static __inline int BUS_DEACTIVATE_RESOURCE(device_t dev, device_t child,
                                            int type, int rid,
                                            struct resource *r)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_deactivate_resource);
	return ((bus_deactivate_resource_t *) _m)(dev, child, type, rid, r);
}

extern struct kobjop_desc bus_release_resource_desc;
typedef int bus_release_resource_t(device_t dev, device_t child, int type,
                                   int rid, struct resource *res);
static __inline int BUS_RELEASE_RESOURCE(device_t dev, device_t child, int type,
                                         int rid, struct resource *res)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_release_resource);
	return ((bus_release_resource_t *) _m)(dev, child, type, rid, res);
}

extern struct kobjop_desc bus_setup_intr_desc;
typedef int bus_setup_intr_t(device_t dev, device_t child, struct resource *irq,
                             int flags, driver_intr_t *intr, void *arg,
                             void **cookiep, lwkt_serialize_t serializer,
                             const char *desc);
static __inline int BUS_SETUP_INTR(device_t dev, device_t child,
                                   struct resource *irq, int flags,
                                   driver_intr_t *intr, void *arg,
                                   void **cookiep, lwkt_serialize_t serializer,
                                   const char *desc)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_setup_intr);
	return ((bus_setup_intr_t *) _m)(dev, child, irq, flags, intr, arg, cookiep, serializer, desc);
}

extern struct kobjop_desc bus_teardown_intr_desc;
typedef int bus_teardown_intr_t(device_t dev, device_t child,
                                struct resource *irq, void *cookie);
static __inline int BUS_TEARDOWN_INTR(device_t dev, device_t child,
                                      struct resource *irq, void *cookie)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_teardown_intr);
	return ((bus_teardown_intr_t *) _m)(dev, child, irq, cookie);
}

extern struct kobjop_desc bus_enable_intr_desc;
typedef void bus_enable_intr_t(device_t dev, device_t child, void *cookie);
static __inline void BUS_ENABLE_INTR(device_t dev, device_t child, void *cookie)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_enable_intr);
	((bus_enable_intr_t *) _m)(dev, child, cookie);
}

extern struct kobjop_desc bus_disable_intr_desc;
typedef int bus_disable_intr_t(device_t dev, device_t child, void *cookie);
static __inline int BUS_DISABLE_INTR(device_t dev, device_t child, void *cookie)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_disable_intr);
	return ((bus_disable_intr_t *) _m)(dev, child, cookie);
}

extern struct kobjop_desc bus_set_resource_desc;
typedef int bus_set_resource_t(device_t dev, device_t child, int type, int rid,
                               u_long start, u_long count, int cpuid);
static __inline int BUS_SET_RESOURCE(device_t dev, device_t child, int type,
                                     int rid, u_long start, u_long count,
                                     int cpuid)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_set_resource);
	return ((bus_set_resource_t *) _m)(dev, child, type, rid, start, count, cpuid);
}

extern struct kobjop_desc bus_get_resource_desc;
typedef int bus_get_resource_t(device_t dev, device_t child, int type, int rid,
                               u_long *startp, u_long *countp);
static __inline int BUS_GET_RESOURCE(device_t dev, device_t child, int type,
                                     int rid, u_long *startp, u_long *countp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_get_resource);
	return ((bus_get_resource_t *) _m)(dev, child, type, rid, startp, countp);
}

extern struct kobjop_desc bus_delete_resource_desc;
typedef void bus_delete_resource_t(device_t dev, device_t child, int type,
                                   int rid);
static __inline void BUS_DELETE_RESOURCE(device_t dev, device_t child, int type,
                                         int rid)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, bus_delete_resource);
	((bus_delete_resource_t *) _m)(dev, child, type, rid);
}

extern struct kobjop_desc bus_get_resource_list_desc;
typedef struct resource_list * bus_get_resource_list_t(device_t _dev,
                                                       device_t _child);
static __inline struct resource_list * BUS_GET_RESOURCE_LIST(device_t _dev,
                                                             device_t _child)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_dev)->ops, bus_get_resource_list);
	return ((bus_get_resource_list_t *) _m)(_dev, _child);
}

extern struct kobjop_desc bus_child_present_desc;
typedef int bus_child_present_t(device_t _dev, device_t _child);
static __inline int BUS_CHILD_PRESENT(device_t _dev, device_t _child)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_dev)->ops, bus_child_present);
	return ((bus_child_present_t *) _m)(_dev, _child);
}

extern struct kobjop_desc bus_child_pnpinfo_str_desc;
typedef int bus_child_pnpinfo_str_t(device_t _dev, device_t _child, char *_buf,
                                    size_t _buflen);
static __inline int BUS_CHILD_PNPINFO_STR(device_t _dev, device_t _child,
                                          char *_buf, size_t _buflen)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_dev)->ops, bus_child_pnpinfo_str);
	return ((bus_child_pnpinfo_str_t *) _m)(_dev, _child, _buf, _buflen);
}

extern struct kobjop_desc bus_child_location_str_desc;
typedef int bus_child_location_str_t(device_t _dev, device_t _child, char *_buf,
                                     size_t _buflen);
static __inline int BUS_CHILD_LOCATION_STR(device_t _dev, device_t _child,
                                           char *_buf, size_t _buflen)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_dev)->ops, bus_child_location_str);
	return ((bus_child_location_str_t *) _m)(_dev, _child, _buf, _buflen);
}

extern struct kobjop_desc bus_config_intr_desc;
typedef int bus_config_intr_t(device_t _dev, device_t _child, int _irq,
                              enum intr_trigger _trig, enum intr_polarity _pol);
static __inline int BUS_CONFIG_INTR(device_t _dev, device_t _child, int _irq,
                                    enum intr_trigger _trig,
                                    enum intr_polarity _pol)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)_dev)->ops, bus_config_intr);
	return ((bus_config_intr_t *) _m)(_dev, _child, _irq, _trig, _pol);
}

#endif /* _bus_if_h_ */
