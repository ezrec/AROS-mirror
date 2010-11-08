#ifndef AHI_Drivers_misc_h
#define AHI_Drivers_misc_h

#include <config.h>

#include <devices/ahi.h>
#include <DriverData.h>
#include <stddef.h>


#define udelay micro_delay


struct CardData* AllocDriverData(APTR dev, struct DriverBase* AHIsubBase);
void FreeDriverData(struct CardData* dd, struct DriverBase*  AHIsubBase);

void *pci_alloc_consistent(size_t size, APTR *NonAlignedAddress, unsigned int boundary);
void pci_free_consistent(void* addr);

void micro_delay(unsigned int val);

// returns the RIRBWP just before sending the command. Used for 12-bit verbs
ULONG send_command_12(UBYTE codec, UBYTE node, UWORD verb, UBYTE payload, struct CardData *card);
ULONG send_command_4(UBYTE codec, UBYTE node, UBYTE verb, UWORD payload, struct CardData *card);
ULONG get_parameter(UBYTE node, UBYTE parameter, struct CardData *card);

void set_monitor_volumes(struct CardData *card, double dB);
void set_adc_input(struct CardData *card);
void set_adc_gain(struct CardData *card, double dB);
void set_dac_gain(struct CardData *card, double dB);

void switch_nid_to_input(struct CardData *card, UBYTE NID);
void switch_nid_to_output(struct CardData *card, UBYTE NID);

void codec_discovery(struct CardData *card);

// when this function is called, the chip is queried for its NID's etc. instead of looking at the hardcoded values
void setForceQuery();

// when set, will call codec_discovery, dumping all information about all NID's
void setDumpAll();

// when set, it uses this as speaker NID instead of querying the chip
void setForceSpeaker(int speaker_nid);

BOOL is_jack_connected(struct CardData *card, UBYTE NID);
void detect_headphone_change(struct CardData *card);

#endif /* AHI_Drivers_misc_h */
