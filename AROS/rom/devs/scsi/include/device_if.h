/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   /home/jmcmullan/private/dragonfly/sys/kern/device_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */

#ifndef _device_if_h_
#define _device_if_h_

extern struct kobjop_desc device_probe_desc;
typedef int device_probe_t(device_t dev);
static __inline int DEVICE_PROBE(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, device_probe);
	return ((device_probe_t *) _m)(dev);
}

extern struct kobjop_desc device_identify_desc;
typedef int device_identify_t(driver_t *driver, device_t parent);
static __inline int DEVICE_IDENTIFY(driver_t *driver, device_t parent)
{
	kobjop_t _m;
	KOBJOPLOOKUP(driver->ops, device_identify);
	return ((device_identify_t *) _m)(driver, parent);
}

extern struct kobjop_desc device_attach_desc;
typedef int device_attach_t(device_t dev);
static __inline int DEVICE_ATTACH(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, device_attach);
	return ((device_attach_t *) _m)(dev);
}

extern struct kobjop_desc device_detach_desc;
typedef int device_detach_t(device_t dev);
static __inline int DEVICE_DETACH(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, device_detach);
	return ((device_detach_t *) _m)(dev);
}

extern struct kobjop_desc device_shutdown_desc;
typedef int device_shutdown_t(device_t dev);
static __inline int DEVICE_SHUTDOWN(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, device_shutdown);
	return ((device_shutdown_t *) _m)(dev);
}

extern struct kobjop_desc device_suspend_desc;
typedef int device_suspend_t(device_t dev);
static __inline int DEVICE_SUSPEND(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, device_suspend);
	return ((device_suspend_t *) _m)(dev);
}

extern struct kobjop_desc device_resume_desc;
typedef int device_resume_t(device_t dev);
static __inline int DEVICE_RESUME(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops, device_resume);
	return ((device_resume_t *) _m)(dev);
}

#endif /* _device_if_h_ */
