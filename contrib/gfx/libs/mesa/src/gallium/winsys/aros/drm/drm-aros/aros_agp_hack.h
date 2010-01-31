#include <exec/types.h>
#include <oop/oop.h>

BOOL aros_agp_hack_device_is_agp(OOP_Object * pciDevice);
BOOL aros_agp_hack_device_is_pcie(OOP_Object * pciDevice);
OOP_Object * aros_agp_hack_get_agp_bridge();
BOOL aros_agp_hack_is_agp_bridge_present();
ULONG aros_agp_hack_get_bridge_mode();
IPTR aros_agp_hack_get_bridge_aperture_base();
ULONG aros_agp_hack_get_bridge_aperture_size();
