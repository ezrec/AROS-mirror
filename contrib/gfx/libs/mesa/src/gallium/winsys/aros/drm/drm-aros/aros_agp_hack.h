#include <exec/types.h>
#include <oop/oop.h>

BOOL aros_agp_hack_device_is_agp(OOP_Object * pciDevice);
BOOL aros_agp_hack_device_is_pcie(OOP_Object * pciDevice);
OOP_Object * aros_agp_hack_find_agp_bridge();
