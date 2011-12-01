/*
    Copyright © 2010-2011, The AROS Development Team. All rights reserved
    $Id$
*/

#define DEBUG 1
#include <aros/debug.h>

#include <proto/exec.h>

#include "ahci_header.h"

//#include LC_LIBDEFS_FILE

/*
    Call every HBA function with pointer to ahci_hba_chip structure as a first argument
*/

static void ahci_hba_Interrupt(HIDDT_IRQ_Handler *irq, HIDDT_IRQ_HwInfo *hw) {
    struct ahci_hba_chip *hba_chip = (struct ahci_hba_chip *)irq->h_Data;

    struct ahci_hwhba *hwhba;
    hwhba = hba_chip->abar;

    /* On my P45DE (ICH10 based) Aros box we share the same interrupt with one of the USB controllers (IRQ rerouting...)*/
    if(hwhba->is) {
        HBAHW_D("IS Register %x!\n", hwhba->is);
    }
}

BOOL ahci_create_interrupt(struct ahci_hba_chip *hba_chip) {

    OOP_Object *irqhidd;

    irqhidd = OOP_NewObject(NULL, CLID_Hidd_IRQ, NULL);
    if (irqhidd) {

        hba_chip->IntHandler = (HIDDT_IRQ_Handler *)AllocVec(sizeof(HIDDT_IRQ_Handler), MEMF_CLEAR|MEMF_PUBLIC);
        if (hba_chip->IntHandler) {

            hba_chip->IntHandler->h_Node.ln_Pri = 10;
            hba_chip->IntHandler->h_Node.ln_Name = "HBA-chip irq";
            hba_chip->IntHandler->h_Code = ahci_hba_Interrupt;
            hba_chip->IntHandler->h_Data = hba_chip;

            HIDD_IRQ_AddHandler(irqhidd, hba_chip->IntHandler, hba_chip->irq);
            OOP_DisposeObject(irqhidd);
            HBAHW_D("IntHandler @ %p\n", hba_chip->IntHandler);
            return TRUE;
        }
        OOP_DisposeObject(irqhidd);
    }

    return FALSE;
}

void ahci_taskcode_hba(struct ahci_hba_chip *hba_chip, struct Task *parent) {
    HBATASK_D("HBA task running...\n");

    hba_chip->mp_io = CreateMsgPort();
    if(hba_chip->mp_io) {

        hba_chip->mp_timer = CreateMsgPort();
        if(hba_chip->mp_timer) {

            hba_chip->tr = (struct timerequest *)CreateIORequest(hba_chip->mp_timer, sizeof(struct timerequest));
            if (hba_chip->tr) {
                FreeSignal(hba_chip->mp_timer->mp_SigBit);

                if (!OpenDevice((STRPTR)"timer.device", UNIT_MICROHZ, (struct IORequest *)hba_chip->tr, 0)) {

                    HBATASK_D("Signaling parent %08x\n", parent);
                    Signal(parent, SIGBREAKF_CTRL_C);

                    if(ahci_init_hba(hba_chip)) {
                        HBATASK_D("HBA initialized\n");
                    }else{
                        HBATASK_D("HBA initialisation failed!\n");

                    /* TODO:
                        Get the semaphore for chip list and remove us from the list,
                        this will be done when init code of the device releases it and then die (check allocated resources if any and release them before...)
                    */

                    }

                    Wait(0);
                }
                hba_chip->mp_timer->mp_SigBit = AllocSignal(-1);
                DeleteIORequest((struct IORequest *)hba_chip->tr);
            }

            DeleteMsgPort(hba_chip->mp_timer);
        }

    }

    /* FIXME: message port creation failed not ahci_init */
    /*
        Something failed while setting up the HW part of the HBA
        Release all allocated memory and other resources for this HBA
        and remove us from the list (no ports for example... should not happen)
    */

    HBAHW_D("ahci_init_hba failed!\n");
}

BOOL ahci_create_hbatask(struct ahci_hba_chip *hba_chip) {
    HBAHW_D("Creating HBA task\n");

    struct Task     *t;
    struct MemList  *ml;
    UBYTE *sp;

    struct TagItem tags[] = {
        { TASKTAG_ARG1, (IPTR)hba_chip },
        { TASKTAG_ARG2, (IPTR)FindTask(0) },
        { TAG_DONE,     0 }
    };

	t=(struct Task *)AllocMem(sizeof(struct Task), MEMF_PUBLIC|MEMF_CLEAR);
	sp = (UBYTE *)AllocMem(HBA_TASK_STACKSIZE, MEMF_PUBLIC|MEMF_CLEAR);
    ml = AllocMem(sizeof(struct MemList) + sizeof(struct MemEntry), MEMF_PUBLIC | MEMF_CLEAR);

	if(t && sp && ml) {
        ml->ml_NumEntries = 2;
        ml->ml_ME[0].me_Addr = t;
        ml->ml_ME[0].me_Length = sizeof(struct Task);
        ml->ml_ME[1].me_Addr = sp;
        ml->ml_ME[1].me_Length = HBA_TASK_STACKSIZE;

        NEWLIST(&t->tc_MemEntry);
        t->tc_Node.ln_Type=NT_TASK;
        t->tc_Node.ln_Pri=HBA_TASK_PRI;
        t->tc_Node.ln_Name="AHCI task #";
        t->tc_SPLower=sp;
        t->tc_SPUpper=sp+HBA_TASK_STACKSIZE;
        t->tc_SPReg=(UBYTE *)t->tc_SPUpper-SP_OFFSET;
        AddHead(&t->tc_MemEntry, &ml->ml_Node);

        NewAddTask(t, ahci_taskcode_hba, NULL, tags);
        Wait(SIGBREAKF_CTRL_C);

        return TRUE;
	}
    return FALSE;
}

/*
    Set the AHCI HBA to a minimally initialized state.

        * Indicate that the system is AHCI aware by setting bit GHC_AE in HBA's GHC register (done in reset code)
        * Determine number of ports implemented by the HBA
        * Ensure that all implemented ports are in IDLE state
        * Determine how many command slots the HBA supports, by reading CAP.NCS
        * Set CAP.S64A to ‘0’, if not compiled for x86_64...
        * For each implemented port, allocate memory for and program registers:
            - PxCLB (and PxCLBU=0, upper 32bits of 64bit address space)
            - PxFB (and PxFBU=0, upper 32bits of 64bit address space)
        * For each implemented port, clear the PxSERR register
        * Allocate and initialize interrupts, and set each implemented port’s PxIE
            register with the appropriate enables. To enable the HBA to generate interrupts, system
            software must also set GHC.IE to a ‘1’.
*/

BOOL ahci_init_hba(struct ahci_hba_chip *hba_chip) {
    HBAHW_D("HBA-init...\n");

    struct ahci_hwhba *hwhba;
    hwhba = hba_chip->abar;

    int i,p=0;

    /* BIOS handoff if HBA supports it and BIOS is the current owner of HBA */
    hba_chip->Version = hwhba->vs;
    HBAHW_D("Controller version %d%d.%d%d\n", ((hba_chip->Version>>24)&0xff), ((hba_chip->Version>>16)&0xff), ((hba_chip->Version>>8)&0xff), (hba_chip->Version&0xff) );

    if ( (hba_chip->Version >= AHCI_VERSION_1_20) ) {
        if( (hwhba->cap2 & CAP2_BOH) ) {
            HBAHW_D("HBA supports BIOS/OS handoff\n");
            if( (hwhba->bohc & BOHC_BOS) ) {
                hwhba->bohc |= BOHC_OOS;
                /* Spin on BOHC_BOS bit FIXME: Possible dead lock. No maximum time given on AHCI1.3 specs... */
                while( (hwhba->bohc & BOHC_BOS) );
                delay_ms(hba_chip, 25);
                /* If after 25ms BOHC_BB bit is still set give bios a minimum of 2 seconds more time to run */
                if( (hwhba->bohc & BOHC_BB) ) {
                    delay_ms(hba_chip, 2500);
                }
            }
        }
    }

    /* Reset the HBA */
    if ( ahci_reset_hba(hba_chip) ) {

        HBAHW_D("Reset done\n");

    /* FIXME: Is there something better for this? */
    #if defined(__i386__)
        hwhba->cap &= ~CAP_S64A;
    #endif

        hba_chip->CommandSlotCount = 1 + ((hwhba->cap >> CAP_NCS_SHIFT) & CAP_NCS_MASK);

	    hba_chip->PortMask = hwhba->pi;
	    if (hba_chip->PortMask == 0) {
            /* Maybe do a probe on ports, though it's explicitly prohibited on unimplemented port */
		    HBAHW_D("You have a buggy BIOS, no way to find out which port is implemented!\n");
            return FALSE;
	    }

        if( !ahci_create_interrupt(hba_chip) )
            return FALSE;

        /*
            Get the pointer to previous HBA-chip struct in the list (if any)
            Port numbering starts from 0 if no previous HBA exist else the port number will be
            the last assigned unit number of previous HBA-chip +1
        */
        struct ahci_hba_chip *prev_hba_chip = (struct ahci_hba_chip*) GetPred(hba_chip);

        if ( prev_hba_chip != NULL ) {
            hba_chip->UnitMin = (prev_hba_chip->UnitMax) + 1;
        }else{
            hba_chip->UnitMin = 0;
        }

        ObtainSemaphore(&hba_chip->port_list_lock);

        /*
            Maximum number of ports per controller is 32 (AHCI1.3)
            As I understand the documentation there may be caps of unimplemented ports between valid ports,
            e.g ports 0,1 and 3 are implemented but not port number 2

            FIXME: If port failes to be initialized we just skip it, maybe it would be better to fail then for all? 
        */
        for (i = 0; i <= 31; i++) {
    		if (hba_chip->PortMask & (1 << i)) {
                if( ahci_add_port(hba_chip, hba_chip->UnitMin+p, i) ) {
                    p++;
#if DEBUG
                }else{
                    HBAHW_D("Port %d(unit to be %d) initialization failed!\n", i, p);
#endif
                }
//          }else{
//              HBAHW_D("Port %d(unit to be %d) unimplemented\n", i, p);
            }
    	}

        ReleaseSemaphore(&hba_chip->port_list_lock);

        if (!p) {
            HBAHW_D("Units assigned %d-%d\n", hba_chip->UnitMin, hba_chip->UnitMax);

    	    /*
                Enable interrupts for this HBA
                Interrupts that are enabled are selected in ahci_init_port per port bases
            */
    	    hwhba->ghc |= GHC_IE;

            return TRUE;
        }

        /*
            Something failed while setting up the ports
        */
        HBAHW_D("Port initialization failed on all ports!\n");
    }

    return FALSE;
}

BOOL ahci_reset_hba(struct ahci_hba_chip *hba_chip) {
    HBAHW_D("HBA-reset...\n");

    struct ahci_hwhba *hwhba;
    hwhba = hba_chip->abar;

    uint32_t Timeout;

	uint32_t saveCaps = hwhba->cap & (CAP_SMPS | CAP_SSS | CAP_SPM | CAP_EMS | CAP_SXS);
	uint32_t savePI = hwhba->pi;

    /* Enable AHCI mode of communication to the HBA */
	ahci_enable_hba(hba_chip);

    /*
        Reset HBA
        -If the HBA has not cleared GHC.HR to ‘0’ within 1 second of software setting GHC.HR to ‘1’, the HBA is in a hung or locked state.
    */
    hwhba->ghc |= GHC_HR;

    Timeout = 1000;
    while( (hwhba->ghc & GHC_HR) ) {
        if( (--Timeout == 0) ) {
            HBAHW_D("Resetting HBA timed out!\n");
            return FALSE;
        }
        delay_ms(hba_chip, 1);
    }

    /* Re-enable AHCI mode */
    ahci_enable_hba(hba_chip);

    /* Write back values of CAP & PI that were saved before reset, maybe useless */
    hwhba->cap |= saveCaps;
    hwhba->pi = savePI;

    return TRUE;
}

void ahci_enable_hba(struct ahci_hba_chip *hba_chip) {
    HBAHW_D("HBA-enable...\n");

    struct ahci_hwhba *hwhba;
    hwhba = hba_chip->abar;

    /*
        Check if HBA supports legacy mode, otherwise GHC_AE(31) may be RO and the bit GHC_AE(31) is set
        Leave bits MRSM(2), IE(1) and HR(0) untouched
        Rest of the GHC register is RO (AHCI v1.3) do not set/clr other bits
    */
    if( !(hwhba->ghc & CAP_SAM) ){
        hwhba->ghc |= GHC_AE;
    }

}

BOOL ahci_disable_hba(struct ahci_hba_chip *hba_chip) {
    HBAHW_D("HBA-disable...\n");

    struct ahci_hwhba *hwhba;
    hwhba = hba_chip->abar;

    /*
        Check first if bit GHC_AE(31) is set and if so then check whether it is RO or RW
        If bit CAP_SAM(18) is not set then HBA can be set to AHCI or legacy mode
        If bit GHC_AE(31) is RW then clear the bit along with the other bits (no bits are allowed to be set at the same time)
    */
    if( (hwhba->ghc & GHC_AE) ){
        if( !(hwhba->ghc & CAP_SAM) ){
            hwhba->ghc = 0x00000000;
            return TRUE;
        }
    }

    return FALSE;
}

/* Ensure that the port is in idle state (It REALLY should be as we are just out of reset) */
BOOL ahci_init_port(struct ahci_hba_chip *hba_chip, uint32_t port_hba_num) {
    HBAHW_D("HBA-init_port...\n");

    struct ahci_hwhba *hwhba;
    hwhba = hba_chip->abar;

    /* These bits in port command register should all be cleared if the port is in idle state */
    HBAHW_D("P%dCMD = %.08x\n", port_hba_num, hwhba->port[port_hba_num].cmd);
    HBAHW_D("Start DMA? %s\n",              (hwhba->port[port_hba_num].cmd & PORT_CMD_ST) ? "yes" : "no");
    HBAHW_D("Command List Running? %s\n",   (hwhba->port[port_hba_num].cmd & PORT_CMD_CR) ? "yes" : "no");
    HBAHW_D("FIS Receive Enable? %s\n",     (hwhba->port[port_hba_num].cmd & PORT_CMD_FRE) ? "yes" : "no");
    HBAHW_D("FIS Receive Running? %s\n",    (hwhba->port[port_hba_num].cmd & PORT_CMD_FR) ? "yes" : "no");

    if ( (hwhba->port[port_hba_num].cmd & PORT_CMD_ST) ) {
    	/* Port is not in idle state */
        HBAHW_D("Running of command list is enabled! Disabling it...\n");
	    hwhba->port[port_hba_num].cmd &= ~PORT_CMD_ST;
	    if ( !(sleep2_bitmask_clr(hba_chip, &hwhba->port[port_hba_num].cmd, PORT_CMD_CR, 500)) ) {
            HBAHW_D("ERROR, timeout!\n");
            return FALSE;
        }
    }

    if ( (hwhba->port[port_hba_num].cmd & PORT_CMD_FRE) ) {
    	/* Port is not in idle state */
        HBAHW_D("FIS receive is enabled! Disabling it...\n");
	    hwhba->port[port_hba_num].cmd &= ~PORT_CMD_FRE;
	    if ( !(sleep2_bitmask_clr(hba_chip, &hwhba->port[port_hba_num].cmd, PORT_CMD_FR, 500)) ) {
            HBAHW_D("ERROR, timeout!\n");
            return FALSE;
        }
    }

    uint32_t tmp = hwhba->port[port_hba_num].serr;
    HBAHW_D("P%dERR = %.08x\n",port_hba_num, tmp);
	/* Clear all implemented bits by setting them to '1' */
    hwhba->port[port_hba_num].serr = tmp;

    /*
        FIXME: Does not clear interrupts from unimplemented ports as they are not parsed here
                also does not enable appropriate interrupts 
    */

    hwhba->port[port_hba_num].is = (PORT_INT_DHR + PORT_INT_PS + PORT_INT_DS + PORT_INT_SDB + PORT_INT_UF + PORT_INT_DP);
    hwhba->is = (1 << port_hba_num);

    return TRUE;
}

/*
    Add a port to the HBA-port list.
    Physical port number is "port_hba_num" (0-31) and it is added as exec unit number "port_unit_num" to the system
    Make sure the port in question is free and ready for use, if not make it so
*/
BOOL ahci_add_port(struct ahci_hba_chip *hba_chip, uint32_t port_unit_num, uint32_t port_hba_num) {
    HBAHW_D("HBA-add_port...\n");

    HBAHW_D("add HBA-port %d as UNIT:%d\n",port_hba_num, port_unit_num);

    struct ahci_hba_port *hba_port;
    if( (hba_port = (struct ahci_hba_port*) AllocVec(sizeof(struct ahci_hba_port), MEMF_CLEAR|MEMF_PUBLIC)) ) {
        HBAHW_D("hba_port struct @ %p\n",hba_port);

        struct ahci_hwhba *hwhba;
        hwhba = hba_chip->abar;

        hba_port->port_unit.parent_hba = hba_chip;
        hba_port->port_unit.Port_HBA_Number = port_hba_num;
        hba_port->port_unit.Port_Unit_Number = port_unit_num;
        hba_chip->UnitMax = port_unit_num;

        if( ahci_init_port(hba_chip, port_hba_num) ) {
            /* HBA-port list is protected for us in ahci_init_hba */
            AddTail((struct List*)&hba_chip->port_list, (struct Node*)hba_port);
            return TRUE;
        }else{
            FreeVec(hba_port);
        }
    }

    /* No memory for port...? or initialization failed, skipping system unit number, e.g. there is no exec unit for this port (port_unit_num) */
    return FALSE;
}

