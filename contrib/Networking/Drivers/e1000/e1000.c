/*
 * $Id: e1000_osdep.h 24814 2006-09-16 22:08:33Z NicJA $
 */
/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA.
*/

#include <exec/types.h>
#include <exec/resident.h>
#include <exec/io.h>
#include <exec/ports.h>

#include <aros/libcall.h>
#include <aros/macros.h>
#include <aros/io.h>

#include <oop/oop.h>

#include <devices/sana2.h>
#include <devices/sana2specialstats.h>

#include <utility/utility.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>

#include <hidd/pci.h>
#include <hidd/irq.h>

#include <proto/oop.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/battclock.h>

#include <stdlib.h>

#include "e1000_osdep.h"
#include "e1000.h"
#include "e1000_defines.h"
#include "e1000_api.h"

#include "unit.h"
#include LC_LIBDEFS_FILE

/* A bit fixed linux stuff here :) */

#undef LIBBASE
#define LIBBASE (dev->e1ku_device)

void e1000_usec_delay(struct net_device *dev, ULONG usec)
{
#if defined(E1000_TIMER)
    if ((dev != NULL) && (dev->e1ku_DelayReq != NULL))
    {
        dev->e1ku_DelayReq->tr_node.io_Command = TR_ADDREQUEST;
        dev->e1ku_DelayReq->tr_time.tv_micro = usec % 1000000;
        dev->e1ku_DelayReq->tr_time.tv_secs = usec / 1000000;
        
        DoIO((struct IORequest *)dev->e1ku_DelayReq);
    }
#endif
}

void e1000_msec_delay(struct net_device *dev, LONG msec)
{
    e1000_usec_delay(dev, 1000 * msec);
}

void e1000_msec_delay_irq(struct net_device *dev, LONG msec)
{
    e1000_usec_delay(dev, 1000 * msec);
}

static BOOL e1000func_check_64k_bound(struct net_device *dev,
                                       void *start, unsigned long len)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	unsigned long begin = (unsigned long) start;
	unsigned long end = begin + len;

	/* First rev 82545 and 82546 need to not allow any memory
	 * write location to cross 64k boundary due to errata 23 */
	if (hw->mac.type == e1000_82545 ||
	    hw->mac.type == e1000_82546) {
		return ((begin ^ (end - 1)) >> 16) != 0 ? FALSE : TRUE;
	}

	return TRUE;
}

void e1000func_irq_disable(struct net_device *dev)
{
	E1000_WRITE_REG((struct e1000_hw *)dev->e1ku_Private00, E1000_IMC, ~0);
	E1000_WRITE_FLUSH((struct e1000_hw *)dev->e1ku_Private00);
//	synchronize_irq(dev->pdev->irq);
}

void e1000func_irq_enable(struct net_device *dev)
{
	E1000_WRITE_REG((struct e1000_hw *)dev->e1ku_Private00, E1000_IMS, IMS_ENABLE_MASK);
	E1000_WRITE_FLUSH((struct e1000_hw *)dev->e1ku_Private00);
}

static void e1000func_enter_82542_rst(struct net_device *dev)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	ULONG rctl;

	if (hw->mac.type != e1000_82542)
		return;
	if (hw->revision_id != E1000_REVISION_2)
		return;

	e1000_pci_clear_mwi(hw);

	rctl = E1000_READ_REG(hw, E1000_RCTL);
	rctl |= E1000_RCTL_RST;
	E1000_WRITE_REG(hw, E1000_RCTL, rctl);
	E1000_WRITE_FLUSH(hw);

    e1000_msec_delay(dev, 5);

//	if (netif_running(netdev))
//		e1000_clean_all_rx_rings(adapter);
}

static void e1000func_leave_82542_rst(struct net_device *dev)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	ULONG rctl;

	if (hw->mac.type != e1000_82542)
		return;
	if (hw->revision_id != E1000_REVISION_2)
		return;

	rctl = E1000_READ_REG(hw, E1000_RCTL);
	rctl &= ~E1000_RCTL_RST;
	E1000_WRITE_REG(hw, E1000_RCTL, rctl);
	E1000_WRITE_FLUSH(hw);

    e1000_msec_delay(dev, 5);

	if (hw->bus.pci_cmd_word & CMD_MEM_WRT_INVALIDATE)
		e1000_pci_set_mwi(hw);

//	if (netif_running(netdev)) {
//		/* No need to loop, because 82542 supports only 1 queue */
//		struct e1000_rx_ring *ring = &adapter->rx_ring[0];
//		e1000_configure_rx(adapter);
//		adapter->alloc_rx_buf(adapter, ring, E1000_DESC_UNUSED(ring));
//	}
}

static void e1000func_startrx(struct net_device *dev)
{
D(bug("[%s]: e1000func_startrx\n", dev->e1ku_name));
#warning "TODO: Handle starting/stopping Rx"
}

static void e1000func_stoprx(struct net_device *dev)
{
D(bug("[%s]: e1000func_stoprx\n", dev->e1ku_name));
#warning "TODO: Handle starting/stopping Rx"
}

static void e1000func_starttx(struct net_device *dev)
{
D(bug("[%s]: e1000func_starttx()\n", dev->e1ku_name));
#warning "TODO: Handle starting/stopping Tx"
}

static void e1000func_stoptx(struct net_device *dev)
{
D(bug("[%s]: e1000func_stoptx()\n", dev->e1ku_name));
#warning "TODO: Handle starting/stopping Tx"
}

static void e1000func_txrxreset(struct net_device *dev)
{
D(bug("[%s]: e1000func_txrxreset()\n", dev->e1ku_name));
}

static void e1000func_configure_tx(struct net_device *dev)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
    ULONG tdlen, tctl, tipg;
	ULONG ipgr1, ipgr2;
    UQUAD tdba;
	int i;

D(bug("[%s]: e1000func_configure_tx()\n", dev->e1ku_name));

	/* Setup the HW Tx Head and Tail descriptor pointers */
	for (i = 0; i < dev->e1ku_txRing_QueueSize; i++) {
		tdba = dev->e1ku_txRing[i].dma;
		tdlen = dev->e1ku_txRing[i].count * sizeof(struct e1000_tx_desc);
D(bug("[%s]: e1000func_configure_tx: Tx Queue %d Ring Descriptor DMA @ %p, len %d\n", dev->e1ku_name, i, tdba, tdlen));
		E1000_WRITE_REG(hw, E1000_TDBAL(i), (tdba & 0x00000000ffffffffULL));
		E1000_WRITE_REG(hw, E1000_TDBAH(i), (tdba >> 32));
		E1000_WRITE_REG(hw, E1000_TDLEN(i), tdlen);
		E1000_WRITE_REG(hw, E1000_TDH(i), 0);
		E1000_WRITE_REG(hw, E1000_TDT(i), 0);
		dev->e1ku_txRing[i].tdh = E1000_REGISTER(hw, E1000_TDH(i));
		dev->e1ku_txRing[i].tdt = E1000_REGISTER(hw, E1000_TDT(i));
D(bug("[%s]: e1000func_configure_tx: Tx Queue %d rdh=%d, rdt=%d\n", dev->e1ku_name, i, dev->e1ku_txRing[i].tdh, dev->e1ku_txRing[i].tdt));
	}

	/* Set the default values for the Tx Inter Packet Gap timer */
	if (hw->mac.type <= e1000_82547_rev_2 &&
	    (hw->phy.media_type == e1000_media_type_fiber ||
	     hw->phy.media_type == e1000_media_type_internal_serdes))
		tipg = DEFAULT_82543_TIPG_IPGT_FIBER;
	else
		tipg = DEFAULT_82543_TIPG_IPGT_COPPER;

	switch (hw->mac.type) {
	case e1000_82542:
		tipg = DEFAULT_82542_TIPG_IPGT;
		ipgr1 = DEFAULT_82542_TIPG_IPGR1;
		ipgr2 = DEFAULT_82542_TIPG_IPGR2;
		break;
	default:
		ipgr1 = DEFAULT_82543_TIPG_IPGR1;
		ipgr2 = DEFAULT_82543_TIPG_IPGR2;
		break;
	}
	tipg |= ipgr1 << E1000_TIPG_IPGR1_SHIFT;
	tipg |= ipgr2 << E1000_TIPG_IPGR2_SHIFT;
	E1000_WRITE_REG(hw, E1000_TIPG, tipg);

	/* Set the Tx Interrupt Delay register */
	E1000_WRITE_REG(hw, E1000_TIDV, 0);
//	if (dev->flags & E1000_FLAG_HAS_INTR_MODERATION)
//		E1000_WRITE_REG(hw, E1000_TADV, dev->tx_abs_int_delay);

	/* Program the Transmit Control Register */
	tctl = E1000_READ_REG(hw, E1000_TCTL);
	tctl &= ~E1000_TCTL_CT;
	tctl |= E1000_TCTL_PSP | E1000_TCTL_RTLC | E1000_TCTL_EN |
		(E1000_COLLISION_THRESHOLD << E1000_CT_SHIFT);

	e1000_config_collision_dist(hw);

	/* Setup Transmit Descriptor Settings for eop descriptor */
	dev->txd_cmd = E1000_TXD_CMD_EOP | E1000_TXD_CMD_IFCS;

	/* only set IDE if we are delaying interrupts using the timers */
//	if (dev->tx_int_delay)
//		dev->txd_cmd |= E1000_TXD_CMD_IDE;

	if (hw->mac.type < e1000_82543)
		dev->txd_cmd |= E1000_TXD_CMD_RPS;
	else
		dev->txd_cmd |= E1000_TXD_CMD_RS;

	/* Cache if we're 82544 running in PCI-X because we'll
	 * need this to apply a workaround later in the send path. */
//	if (hw->mac.type == e1000_82544 &&
//	    hw->bus.type == e1000_bus_type_pcix)
//		adapter->pcix_82544 = 1;

	E1000_WRITE_REG(hw, E1000_TCTL, tctl);
}

static void e1000func_setup_rctl(struct net_device *dev)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	ULONG rctl;

D(bug("[%s]: e1000func_setup_rctl()\n", dev->e1ku_name));
    
	rctl = E1000_READ_REG(hw, E1000_RCTL);

	rctl &= ~(3 << E1000_RCTL_MO_SHIFT);

	rctl |= E1000_RCTL_EN | E1000_RCTL_BAM |
		E1000_RCTL_LBM_NO | E1000_RCTL_RDMTS_HALF |
		(hw->mac.mc_filter_type << E1000_RCTL_MO_SHIFT);

	/* disable the stripping of CRC because it breaks
	 * BMC firmware connected over SMBUS
	if (adapter->hw.mac.type > e1000_82543)
		rctl |= E1000_RCTL_SECRC;
	*/

	if (e1000_tbi_sbp_enabled_82543(hw))
		rctl |= E1000_RCTL_SBP;
	else
		rctl &= ~E1000_RCTL_SBP;

	if (dev->e1ku_mtu <= ETH_DATA_LEN)
		rctl &= ~E1000_RCTL_LPE;
	else
		rctl |= E1000_RCTL_LPE;

	/* Setup buffer sizes */
	rctl &= ~E1000_RCTL_SZ_4096;
	rctl |= E1000_RCTL_BSEX;
	switch (dev->rx_buffer_len) {
		case E1000_RXBUFFER_256:
			rctl |= E1000_RCTL_SZ_256;
			rctl &= ~E1000_RCTL_BSEX;
			break;
		case E1000_RXBUFFER_512:
			rctl |= E1000_RCTL_SZ_512;
			rctl &= ~E1000_RCTL_BSEX;
			break;
		case E1000_RXBUFFER_1024:
			rctl |= E1000_RCTL_SZ_1024;
			rctl &= ~E1000_RCTL_BSEX;
			break;
		case E1000_RXBUFFER_2048:
		default:
			rctl |= E1000_RCTL_SZ_2048;
			rctl &= ~E1000_RCTL_BSEX;
			break;
		case E1000_RXBUFFER_4096:
			rctl |= E1000_RCTL_SZ_4096;
			break;
		case E1000_RXBUFFER_8192:
			rctl |= E1000_RCTL_SZ_8192;
			break;
		case E1000_RXBUFFER_16384:
			rctl |= E1000_RCTL_SZ_16384;
			break;
	}

	E1000_WRITE_REG(hw, E1000_RCTL, rctl);
}

static void e1000func_configure_rx(struct net_device *dev)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	ULONG rdlen, rctl, rxcsum;
	UQUAD rdba;
	int i;

D(bug("[%s]: e1000func_configure_rx()\n", dev->e1ku_name));
    
	{
		//adapter->clean_rx = e1000func_clean_rx_irq;
		//adapter->alloc_rx_buf = e1000func_alloc_rx_buffers;
	}

	/* disable receivers while setting up the descriptors */
	rctl = E1000_READ_REG(hw, E1000_RCTL);
	E1000_WRITE_REG(hw, E1000_RCTL, rctl & ~E1000_RCTL_EN);
	E1000_WRITE_FLUSH(hw);

    e1000_msec_delay(dev, 10);

	/* set the Receive Delay Timer Register */
	E1000_WRITE_REG(hw, E1000_RDTR, 0);

//	if (adapter->flags & E1000_FLAG_HAS_INTR_MODERATION) {
//		E1000_WRITE_REG(hw, E1000_RADV, adapter->rx_abs_int_delay);
//		if (adapter->itr_setting != 0)
//			E1000_WRITE_REG(hw, E1000_ITR,
//				1000000000 / (adapter->itr * 256));
//	}

	/* Setup the HW Rx Head and Tail Descriptor Pointers and
	 * the Base and Length of the Rx Descriptor Ring */
	for (i = 0; i < dev->e1ku_rxRing_QueueSize; i++) {
		rdlen = dev->e1ku_rxRing[i].count *
			sizeof(struct e1000_rx_desc);
		rdba = dev->e1ku_rxRing[i].dma;
D(bug("[%s]: e1000func_configure_rx: Rx Queue %d Ring Descriptor DMA @ %p, len %d\n", dev->e1ku_name, i, rdba, rdlen));
		E1000_WRITE_REG(hw, E1000_RDBAL(i), (rdba & 0x00000000ffffffffULL));
		E1000_WRITE_REG(hw, E1000_RDBAH(i), (rdba >> 32));
		E1000_WRITE_REG(hw, E1000_RDLEN(i), rdlen);
		E1000_WRITE_REG(hw, E1000_RDH(i), 0);
		E1000_WRITE_REG(hw, E1000_RDT(i), 0);
		dev->e1ku_rxRing[i].rdh = E1000_REGISTER(hw, E1000_RDH(i));
		dev->e1ku_rxRing[i].rdt = E1000_REGISTER(hw, E1000_RDT(i));
D(bug("[%s]: e1000func_configure_rx: Rx Queue %d rdh=%d, rdt=%d\n", dev->e1ku_name, i, dev->e1ku_rxRing[i].rdh, dev->e1ku_rxRing[i].rdt));
	}

D(bug("[%s]: e1000func_configure_rx: Configuring checksum Offload..\n", dev->e1ku_name));
	if (hw->mac.type >= e1000_82543) {
		/* Enable 82543 Receive Checksum Offload for TCP and UDP */
		rxcsum = E1000_READ_REG(hw, E1000_RXCSUM);
//		if (dev->rx_csum == TRUE) {
//			rxcsum |= E1000_RXCSUM_TUOFL;
//		} else {
			rxcsum &= ~E1000_RXCSUM_TUOFL;
			/* don't need to clear IPPCSE as it defaults to 0 */
//		}
		E1000_WRITE_REG(hw, E1000_RXCSUM, rxcsum);
	}

D(bug("[%s]: e1000func_configure_rx: Enable Receivers..\n", dev->e1ku_name));
	/* Enable Receivers */
	E1000_WRITE_REG(hw, E1000_RCTL, rctl);
}

void e1000func_reset(struct net_device *dev)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	struct e1000_mac_info *mac = &hw->mac;
	struct e1000_fc_info *fc = &hw->fc;
	u32 pba = 0, tx_space, min_tx_space, min_rx_space;
	bool legacy_pba_adjust = FALSE;
	u16 hwm;

D(bug("[%s]: e1000func_reset()\n", dev->e1ku_name));

	/* Repartition Pba for greater than 9k mtu
	 * To take effect CTRL.RST is required.
	 */

	switch (mac->type) {
	case e1000_82542:
	case e1000_82543:
	case e1000_82544:
	case e1000_82540:
	case e1000_82541:
	case e1000_82541_rev_2:
		legacy_pba_adjust = TRUE;
		pba = E1000_PBA_48K;
		break;
	case e1000_82545:
	case e1000_82545_rev_3:
	case e1000_82546:
	case e1000_82546_rev_3:
		pba = E1000_PBA_48K;
		break;
	case e1000_82547:
	case e1000_82547_rev_2:
		legacy_pba_adjust = TRUE;
		pba = E1000_PBA_30K;
		break;
	case e1000_undefined:
	case e1000_num_macs:
		break;
	}

	if (legacy_pba_adjust == TRUE) {
		if (dev->e1ku_frame_max > E1000_RXBUFFER_8192)
			pba -= 8; /* allocate more FIFO for Tx */

		if (mac->type == e1000_82547) {
			dev->e1ku_tx_fifo_head = 0;
			dev->e1ku_tx_head_addr = pba << E1000_TX_HEAD_ADDR_SHIFT;
			dev->e1ku_tx_fifo_size =
				(E1000_PBA_40K - pba) << E1000_PBA_BYTES_SHIFT;
//			atomic_set(&dev->tx_fifo_stall, 0);
		}
	} else if (dev->e1ku_frame_max > ETH_MAXPACKETSIZE) {
		/* adjust PBA for jumbo frames */
		E1000_WRITE_REG(hw, E1000_PBA, pba);

		/* To maintain wire speed transmits, the Tx FIFO should be
		 * large enough to accommodate two full transmit packets,
		 * rounded up to the next 1KB and expressed in KB.  Likewise,
		 * the Rx FIFO should be large enough to accommodate at least
		 * one full receive packet and is similarly rounded up and
		 * expressed in KB. */
		pba = E1000_READ_REG(hw, E1000_PBA);
		/* upper 16 bits has Tx packet buffer allocation size in KB */
		tx_space = pba >> 16;
		/* lower 16 bits has Rx packet buffer allocation size in KB */
		pba &= 0xffff;
		/* the tx fifo also stores 16 bytes of information about the tx
		 * but don't include ethernet FCS because hardware appends it */
		min_tx_space = (dev->e1ku_frame_max +
		                sizeof(struct e1000_tx_desc) -
		                ETH_CRCSIZE) * 2;
		min_tx_space = ALIGN(min_tx_space, 1024);
		min_tx_space >>= 10;
		/* software strips receive CRC, so leave room for it */
		min_rx_space = dev->e1ku_frame_max;
		min_rx_space = ALIGN(min_rx_space, 1024);
		min_rx_space >>= 10;

		/* If current Tx allocation is less than the min Tx FIFO size,
		 * and the min Tx FIFO size is less than the current Rx FIFO
		 * allocation, take space away from current Rx allocation */
		if (tx_space < min_tx_space &&
		    ((min_tx_space - tx_space) < pba)) {
			pba = pba - (min_tx_space - tx_space);

			/* PCI/PCIx hardware has PBA alignment constraints */
			switch (mac->type) {
			case e1000_82545 ... e1000_82546_rev_3:
				pba &= ~(E1000_PBA_8K - 1);
				break;
			default:
				break;
			}

			/* if short on rx space, rx wins and must trump tx
			 * adjustment or use Early Receive if available */
			if (pba < min_rx_space) {
				pba = min_rx_space;
			}
		}
	}

	E1000_WRITE_REG(hw, E1000_PBA, pba);

	/* flow control settings */
	/* The high water mark must be low enough to fit one full frame
	 * (or the size used for early receive) above it in the Rx FIFO.
	 * Set it to the lower of:
	 * - 90% of the Rx FIFO size, and
	 * - the full Rx FIFO size minus the early receive size (for parts
	 *   with ERT support assuming ERT set to E1000_ERT_2048), or
	 * - the full Rx FIFO size minus one full frame */
	hwm = min(((pba << 10) * 9 / 10),
		  ((pba << 10) - dev->e1ku_frame_max));

	fc->high_water = hwm & 0xFFF8;	/* 8-byte granularity */
	fc->low_water = fc->high_water - 8;

	fc->pause_time = E1000_FC_PAUSE_TIME;
	fc->send_xon = 1;
	fc->type = fc->original_type;

	/* Allow time for pending master requests to run */
	e1000_reset_hw(hw);

	if (mac->type >= e1000_82544)
		E1000_WRITE_REG(hw, E1000_WUC, 0);

	if (e1000_init_hw(hw))
    {
D(bug("[%s]: e1000func_reset: Hardware Error\n", dev->e1ku_name));
    }
	/* if (dev->hwflags & HWFLAGS_PHY_PWR_BIT) { */
	if (mac->type >= e1000_82544 &&
	    mac->type <= e1000_82547_rev_2 &&
	    mac->autoneg == 1 &&
	    hw->phy.autoneg_advertised == ADVERTISE_1000_FULL) {
		u32 ctrl = E1000_READ_REG(hw, E1000_CTRL);
		/* clear phy power management bit if we are in gig only mode,
		 * which if enabled will attempt negotiation to 100Mb, which
		 * can cause a loss of link at power off or driver unload */
		ctrl &= ~E1000_CTRL_SWDPIN3;
		E1000_WRITE_REG(hw, E1000_CTRL, ctrl);
	}

	/* Enable h/w to recognize an 802.1Q VLAN Ethernet packet */
	E1000_WRITE_REG(hw, E1000_VET, ETHERNET_IEEE_VLAN_TYPE);

	e1000_reset_adaptive(hw);
	e1000_get_phy_info(hw);

//	e1000_release_manageability(dev);
}

int e1000func_set_mac(struct net_device *dev)
{
    struct e1000_hw *hw = dev->e1ku_Private00;

D(bug("[%s]: e1000func_set_mac()\n", dev->e1ku_name));

	/* 82542 2.0 needs to be in reset to write receive address registers */
	if (hw->mac.type == e1000_82542)
    {
		e1000func_enter_82542_rst(dev);
    }

    memcpy(hw->mac.addr, dev->e1ku_dev_addr, ETH_ADDRESSSIZE);

	e1000_rar_set(hw, hw->mac.addr, 0);

	if (hw->mac.type == e1000_82542)
    {
		e1000func_leave_82542_rst(dev);
    }

	return 0;
}

void e1000func_set_multi(struct net_device *dev)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	struct e1000_mac_info *mac = &hw->mac;
    struct AddressRange *range;
	UBYTE  *mta_list;
	ULONG rctl, mc_count;
	int i = 0;

D(bug("[%s]: e1000func_set_multi()\n", dev->e1ku_name));

	/* Check for Promiscuous and All Multicast modes */

	rctl = E1000_READ_REG(hw, E1000_RCTL);

	if (dev->e1ku_ifflags & IFF_PROMISC) {
		rctl |= (E1000_RCTL_UPE | E1000_RCTL_MPE);
	} else if (dev->e1ku_ifflags & IFF_ALLMULTI) {
		rctl |= E1000_RCTL_MPE;
		rctl &= ~E1000_RCTL_UPE;
	} else {
		rctl &= ~(E1000_RCTL_UPE | E1000_RCTL_MPE);
	}

	E1000_WRITE_REG(hw, E1000_RCTL, rctl);

	/* 82542 2.0 needs to be in reset to write receive address registers */

	if (hw->mac.type == e1000_82542)
		e1000func_enter_82542_rst(dev);

    ListLength(&dev->e1ku_multicast_ranges, mc_count);
    
    if (mc_count > 0)
    {
        mta_list = AllocMem(mc_count * ETH_ADDRESSSIZE, MEMF_PUBLIC | MEMF_CLEAR );
        if (!mta_list)
            return;

        /* The shared function expects a packed array of only addresses. */
        ForeachNode(&dev->e1ku_multicast_ranges, range) {
            memcpy(mta_list + (i*ETH_ADDRESSSIZE), &range->lower_bound_left, ETH_ADDRESSSIZE);
            i++;
        }

        e1000_update_mc_addr_list(hw, mta_list, i, 1, mac->rar_entry_count);

        FreeMem(mta_list, mc_count * ETH_ADDRESSSIZE);
    }
	if (hw->mac.type == e1000_82542)
		e1000func_leave_82542_rst(dev);
}

static void e1000func_deinitialize(struct net_device *dev)
{
}

static void e1000func_drain_tx(struct net_device *dev)
{
    int i;
//    for (i = 0; i < TX_RING_SIZE; i++) {
#warning "TODO: e1000func_drain_tx does nothing atm."
//    }
}

static void e1000func_drain_rx(struct net_device *dev)
{
    int i;
//    for (i = 0; i < RX_RING_SIZE; i++) {
#warning "TODO: e1000func_drain_rx does nothing atm."
//    }
}


static void drain_ring(struct net_device *dev)
{
    e1000func_drain_tx(dev);
    e1000func_drain_rx(dev);
}

int request_irq(struct net_device *dev)
{
    OOP_Object *irq = OOP_NewObject(NULL, CLID_Hidd_IRQ, NULL);
    BOOL ret;

D(bug("[%s]: request_irq()\n", dev->e1ku_name));

    if (irq)
    {
        ret = HIDD_IRQ_AddHandler(irq, dev->e1ku_irqhandler, dev->e1ku_IRQ);
        HIDD_IRQ_AddHandler(irq, dev->e1ku_touthandler, vHidd_IRQ_Timer);

D(bug("[%s]: request_irq: IRQ Handlers configured\n", dev->e1ku_name));

        OOP_DisposeObject(irq);

        if (ret)
        {
            return 0;
        }
    }
    return 1;
}

static void free_irq(struct net_device *dev)
{
    OOP_Object *irq = OOP_NewObject(NULL, CLID_Hidd_IRQ, NULL);
    if (irq)
    {
        HIDD_IRQ_RemHandler(irq, dev->e1ku_irqhandler);
        HIDD_IRQ_RemHandler(irq, dev->e1ku_touthandler);
        OOP_DisposeObject(irq);
    }
}

static int e1000func_setup_tx_resources(struct net_device *dev,
                                    struct e1000_tx_ring *tx_ring)
{
	int size;

D(bug("[%s]: e1000func_setup_tx_resources()\n", dev->e1ku_name));
    
	size = sizeof(struct e1000_buffer) * tx_ring->count;

D(bug("[%s]: e1000func_setup_tx_resources: tx_ring->count = %d\n", dev->e1ku_name, tx_ring->count));
    
	if ((tx_ring->buffer_info = AllocMem(size, MEMF_PUBLIC | MEMF_CLEAR)) == NULL) {
D(bug("[%s]: e1000func_setup_tx_resources: Unable to allocate memory for the transmit descriptor ring\n", dev->e1ku_name));
		return -E1000_ERR_CONFIG;
	}

	/* round up to nearest 4K */

	tx_ring->size = tx_ring->count * sizeof(struct e1000_tx_desc);
	tx_ring->size = ALIGN(tx_ring->size, 4096);

	tx_ring->desc = HIDD_PCIDriver_AllocPCIMem(
                        dev->e1ku_PCIDriver,
                        tx_ring->size);

    tx_ring->dma = tx_ring->desc;
    
	if (tx_ring->desc == NULL) {
setup_tx_desc_die:
		FreeMem(tx_ring->buffer_info, size);
D(bug("[%s]: e1000func_setup_tx_resources: Unable to allocate memory for the transmit descriptor ring\n", dev->e1ku_name));
		return -E1000_ERR_CONFIG;
	}

	/* Fix for errata 23, can't cross 64kB boundary */
	if (!e1000func_check_64k_bound(dev, tx_ring->desc, tx_ring->size)) {
		void *olddesc = tx_ring->desc;
		APTR olddma = tx_ring->dma;
D(bug("[%s]: e1000func_setup_tx_resources: tx_ring align check failed: %u bytes at %p\n", dev->e1ku_name, tx_ring->size, tx_ring->desc));
		/* Try again, without freeing the previous */
		tx_ring->desc = HIDD_PCIDriver_AllocPCIMem(
                        dev->e1ku_PCIDriver,
                        tx_ring->size);

        tx_ring->dma = tx_ring->desc;

		/* Failed allocation, critical failure */
		if (!tx_ring->desc) {
            HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, olddesc);
			goto setup_tx_desc_die;
		}

		if (!e1000func_check_64k_bound(dev, tx_ring->desc,
		                           tx_ring->size)) {
			/* give up */
			HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, tx_ring->desc);
			HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, olddesc);
D(bug("[%s]: e1000func_setup_tx_resources: Unable to allocate aligned memory for the transmit descriptor ring\n", dev->e1ku_name));
			FreeMem(tx_ring->buffer_info, size);
			return -E1000_ERR_CONFIG;
		} else {
			/* Free old allocation, new allocation was successful */
			HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, olddesc);
		}
	}

	tx_ring->next_to_use = 0;
	tx_ring->next_to_clean = 0;

	return 0;
}

int e1000func_setup_all_tx_resources(struct net_device *dev)
{
	int i, err = 0;

	for (i = 0; i < dev->e1ku_txRing_QueueSize; i++) {
		err = e1000func_setup_tx_resources(dev, &dev->e1ku_txRing[i]);
		if (err) {
D(bug("[%s]: e1000func_setup_all_tx_resources: Allocation for Tx Queue %u failed\n", dev->e1ku_name, i));
			for (i-- ; i >= 0; i--)
				e1000func_free_tx_resources(dev,
							&dev->e1ku_txRing[i]);
			break;
		}
	}

	return err;
}

static int e1000func_setup_rx_resources(struct net_device *dev,
                                    struct e1000_rx_ring *rx_ring)
{
	int buffer_size;

D(bug("[%s]: e1000func_setup_rx_resources()\n", dev->e1ku_name));
    
	buffer_size = sizeof(struct e1000_rx_buffer) * rx_ring->count;

D(bug("[%s]: e1000func_setup_rx_resources: rx_ring->count = %d\n", dev->e1ku_name, rx_ring->count));

	if ((rx_ring->buffer_info = AllocMem(buffer_size, MEMF_PUBLIC | MEMF_CLEAR)) == NULL) {
D(bug("[%s]: e1000_setup_rx_resources: Unable to allocate memory for the receive descriptor ring\n", dev->e1ku_name));
		return -E1000_ERR_CONFIG;
	}

	/* Round up to nearest 4K */

	rx_ring->size = rx_ring->count * sizeof(struct e1000_rx_desc);
	rx_ring->size = ALIGN(rx_ring->size, 4096);

	if ((rx_ring->desc = HIDD_PCIDriver_AllocPCIMem(
                        dev->e1ku_PCIDriver,
                        rx_ring->size)) == NULL) {
D(bug("[%s]: e1000_setup_rx_resources: Unable to allocate memory for the receive descriptor ring\n", dev->e1ku_name));
setup_rx_desc_die:
		FreeMem(rx_ring->buffer_info, buffer_size);
		return -E1000_ERR_CONFIG;
	}

    rx_ring->dma = rx_ring->desc;

	/* Fix for errata 23, can't cross 64kB boundary */
	if (!e1000func_check_64k_bound(dev, rx_ring->desc, rx_ring->size)) {
		void *olddesc = rx_ring->desc;
		APTR olddma = rx_ring->dma;
D(bug("[%s]: e1000_setup_rx_resources: rx_ring align check failed: %u bytes at %p\n", dev->e1ku_name, rx_ring->size, rx_ring->desc));

		/* Try again, without freeing the previous */
		if ((rx_ring->desc = HIDD_PCIDriver_AllocPCIMem(
                            dev->e1ku_PCIDriver,
                            rx_ring->size)) == NULL) {
            /* Failed allocation, critical failure */
            HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, olddesc);
D(bug("[%s]: e1000_setup_rx_resources: Unable to allocate memory for the receive descriptor ring\n", dev->e1ku_name));
			goto setup_rx_desc_die;
		}

        rx_ring->dma = rx_ring->desc;

		if (!e1000func_check_64k_bound(dev, rx_ring->desc,
		                           rx_ring->size)) {
			/* give up */
            HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, rx_ring->desc);
            HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, olddesc);
D(bug("[%s]: e1000_setup_rx_resources: Unable to allocate aligned memory for the receive descriptor ring\n", dev->e1ku_name));
			goto setup_rx_desc_die;
		} else {
			/* Free old allocation, new allocation was successful */
            HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, olddesc);
		}
	}

	/* set up ring defaults */
	rx_ring->next_to_clean = 0;
	rx_ring->next_to_use = 0;

	return 0;
}

int e1000func_setup_all_rx_resources(struct net_device *dev)
{
	int i, err = 0;

	for (i = 0; i < dev->e1ku_rxRing_QueueSize; i++) {
		err = e1000func_setup_rx_resources(dev, &dev->e1ku_rxRing[i]);
		if (err) {
D(bug("[%s]: e1000func_setup_all_rx_resources: Allocation for Rx Queue %u failed\n", dev->e1ku_name, i));
			for (i-- ; i >= 0; i--)
				e1000func_free_rx_resources(dev,
							&dev->e1ku_rxRing[i]);
			break;
		}
	}

	return err;
}

void e1000func_unmap_and_free_tx_resource(struct net_device *dev,
                                             struct e1000_buffer *buffer_info)
{
/*	if (buffer_info->dma) {
		pci_unmap_page(dev->pdev,
				buffer_info->dma,
				buffer_info->length,
				PCI_DMA_TODEVICE);
		buffer_info->dma = 0;
	}
	if (buffer_info->skb) {
		dev_kfree_skb_any(buffer_info->skb);
		buffer_info->skb = NULL;
	}*/
	/* buffer_info must be completely set up in the transmit path */
}

void e1000func_clean_tx_ring(struct net_device *dev,
                                struct e1000_tx_ring *tx_ring)
{
	struct e1000_buffer *buffer_info;
    struct e1000_hw *hw = dev->e1ku_Private00;
	unsigned long size;
	unsigned int i;

	/* Free all the Tx ring buffers */

	for (i = 0; i < tx_ring->count; i++) {
		buffer_info = &tx_ring->buffer_info[i];
		e1000func_unmap_and_free_tx_resource(dev, buffer_info);
	}

	size = sizeof(struct e1000_buffer) * tx_ring->count;
	memset(tx_ring->buffer_info, 0, size);

	/* Zero out the descriptor ring */

	memset(tx_ring->desc, 0, tx_ring->size);

	tx_ring->next_to_use = 0;
	tx_ring->next_to_clean = 0;
//	tx_ring->last_tx_tso = 0;

	writel(0, hw->hw_addr + tx_ring->tdh);
	writel(0, hw->hw_addr + tx_ring->tdt);
}

void e1000func_free_tx_resources(struct net_device *dev,
                                    struct e1000_tx_ring *tx_ring)
{
	e1000func_clean_tx_ring(dev, tx_ring);

	FreeMem(tx_ring->buffer_info, tx_ring->size);
	tx_ring->buffer_info = NULL;

	HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, tx_ring->desc);
    tx_ring->dma =  tx_ring->desc = NULL;
}

void e1000func_clean_rx_ring(struct net_device *dev,
                                struct e1000_rx_ring *rx_ring)
{
	struct e1000_rx_buffer *buffer_info;
    struct e1000_hw *hw = dev->e1ku_Private00;
	unsigned long size;
	unsigned int i;

	/* Free all the Rx ring sk_buffs */
	for (i = 0; i < rx_ring->count; i++) {
		buffer_info = &rx_ring->buffer_info[i];
		/*if (buffer_info->dma &&
		    dev->clean_rx == e1000_clean_rx_irq) {
			//pci_unmap_single(pdev, buffer_info->dma,
			//                 dev->rx_buffer_len,
			//                 PCI_DMA_FROMDEVICE);
		}*/
		buffer_info->dma = 0;
	}

	size = sizeof(struct e1000_rx_buffer) * rx_ring->count;
	memset(rx_ring->buffer_info, 0, size);

	/* Zero out the descriptor ring */

	memset(rx_ring->desc, 0, rx_ring->size);

	rx_ring->next_to_clean = 0;
	rx_ring->next_to_use = 0;

	writel(0, hw->hw_addr + rx_ring->rdh);
	writel(0, hw->hw_addr + rx_ring->rdt);
}

void e1000func_free_rx_resources(struct net_device *dev,
                                    struct e1000_rx_ring *rx_ring)
{
	e1000func_clean_rx_ring(dev, rx_ring);

	FreeMem(rx_ring->buffer_info, rx_ring->size);
	rx_ring->buffer_info = NULL;

	HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, rx_ring->desc);
	rx_ring->dma = rx_ring->desc = NULL;
}

static int e1000func_close(struct net_device *dev)
{
    dev->e1ku_ifflags &= ~IFF_UP;

//    ObtainSemaphore(&np->lock);
//    np->in_shutdown = 1;
//    ReleaseSemaphore(&np->lock);

    dev->e1ku_toutNEED = FALSE;

//    netif_stop_queue(dev);
//    ObtainSemaphore(&np->lock);
    
    e1000func_deinitialize(dev);    // Stop the chipset and set it in 16bit-mode

//    ReleaseSemaphore(&np->lock);

    free_irq(dev);

    drain_ring(dev);

//    HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, np->rx_buffer);
//    HIDD_PCIDriver_FreePCIMem(dev->e1ku_PCIDriver, np->tx_buffer);

    ReportEvents(LIBBASE, dev, S2EVENT_OFFLINE);

    return 0;
}

void e1000func_alloc_rx_buffers(struct net_device *dev,
                                   struct e1000_rx_ring *rx_ring,
                                   int cleaned_count)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	struct e1000_rx_desc *rx_desc;
	struct e1000_rx_buffer *buffer_info;
	unsigned int i;

	i = rx_ring->next_to_use;
	buffer_info = &rx_ring->buffer_info[i];

	while (cleaned_count--) {
		if ((buffer_info->buffer = AllocMem(dev->rx_buffer_len, MEMF_PUBLIC|MEMF_CLEAR)) != NULL)
        {
D(bug("[%s]: e1000func_alloc_rx_buffers: Buffer %d Allocated @ %p\n", dev->e1ku_name, i, buffer_info->buffer));
            if ((buffer_info->dma = (UBYTE *)HIDD_PCIDriver_MapPCI(dev->e1ku_PCIDriver, (APTR)buffer_info->buffer, dev->rx_buffer_len)) == NULL)
            {
D(bug("[%s]: e1000func_alloc_rx_buffers: Failed to Map Buffer %d for DMA!!\n", dev->e1ku_name, i));
            }
D(bug("[%s]: e1000func_alloc_rx_buffers: Buffer %d DMA @ %p\n", dev->e1ku_name, i, buffer_info->dma));

            rx_desc = E1000_RX_DESC(*rx_ring, i);
//    		rx_desc->buffer_addr = cpu_to_le64(buffer_info->dma);
            rx_desc->buffer_addr = buffer_info->dma;
        }

		if (++i == rx_ring->count)
			i = 0;
		buffer_info = &rx_ring->buffer_info[i];
	}

	if (rx_ring->next_to_use != i) {
		rx_ring->next_to_use = i;
		if (i-- == 0)
			i = (rx_ring->count - 1);

		writel(i, hw->hw_addr + rx_ring->rdt);
	}
}

void e1000func_configure(struct net_device *dev)
{
	int i;

D(bug("[%s]: e1000func_configure()\n", dev->e1ku_name));
    
	e1000func_set_multi(dev);

	e1000func_configure_tx(dev);
	e1000func_setup_rctl(dev);
	e1000func_configure_rx(dev);
D(bug("[%s]: e1000func_configure: Tx/Rx Configured\n", dev->e1ku_name));
	/* call E1000_DESC_UNUSED which always leaves
	 * at least 1 descriptor unused to make sure
	 * next_to_use != next_to_clean */
	for (i = 0; i < dev->e1ku_rxRing_QueueSize; i++) {
        D(bug("[%s]: e1000func_configure: Allocating Rx Buffers for queue %d\n", dev->e1ku_name, i));
		struct e1000_rx_ring *ring = &dev->e1ku_rxRing[i];
		e1000func_alloc_rx_buffers(dev, ring,
		                      E1000_DESC_UNUSED(ring));
	}
D(bug("[%s]: e1000func_configure: Finished\n", dev->e1ku_name));
}

BOOL e1000func_clean_rx_irq(struct net_device *dev,
                                    struct e1000_rx_ring *rx_ring)
{
    struct e1000_hw *hw = dev->e1ku_Private00;
	struct e1000_rx_desc *rx_desc, *next_rxd;
	struct e1000_rx_buffer *buffer_info, *next_buffer;
    struct Opener *opener, *opener_tail;
    struct IOSana2Req *request, *request_tail;
    struct eth_frame *frame;

	unsigned int i, total_rx_bytes=0, total_rx_packets=0;
	int cleaned_count = 0;
    UBYTE status;
	ULONG length;
    BOOL accepted, is_orphan, cleaned = FALSE;

	i = rx_ring->next_to_clean;
	rx_desc = E1000_RX_DESC(*rx_ring, i);
	buffer_info = &rx_ring->buffer_info[i];

	while (rx_desc->status & E1000_RXD_STAT_DD) {
		cleaned = TRUE;
		status = rx_desc->status;
		length = AROS_LE2WORD(rx_desc->length);

		if (++i == rx_ring->count) i = 0;
		next_rxd = E1000_RX_DESC(*rx_ring, i);

		next_buffer = &rx_ring->buffer_info[i];

		cleaned_count++;

		/* !EOP means multiple descriptors were used to store a single
		 * packet, also make sure the frame isn't just CRC only */
		if (!(status & E1000_RXD_STAT_EOP) || (length <= ETH_CRCSIZE)) {
			/* All receives must fit into a single buffer */
D(bug("[%s]: e1000func_clean_rx_irq: Receive packet consumed multiple buffers\n", dev->e1ku_name));
			/* recycle */
			goto next_desc;
		}

        frame = rx_desc->buffer_addr;

		if (rx_desc->errors & E1000_RXD_ERR_FRAME_ERR_MASK){
			UBYTE last_byte = *(frame->eth_packet_data + length - 1);
			if (TBI_ACCEPT(hw, status,
			              rx_desc->errors, length, last_byte,
			              dev->e1ku_frame_min,
			              dev->e1ku_frame_max)) {

				e1000_tbi_adjust_stats_82543(hw,
				                      dev->e1ku_hw_stats,
				                      length, frame->eth_packet_data,
				                      dev->e1ku_frame_max);

				length--;
			} else {
				/* recycle */
				goto next_desc;
			}
		}

        /* got a valid packet - forward it to the network core */
        is_orphan = TRUE;

		/* adjust length to remove Ethernet CRC, this must be
		 * done after the TBI_ACCEPT workaround above */
		length -= ETH_CRCSIZE;

		/* probably a little skewed due to removing CRC */
		total_rx_bytes += length;
		total_rx_packets++;

		/* Receive Checksum Offload */
//		e1000func_rx_checksum(dev,
//				  (ULONG)(status) |
//				  ((ULONG)(rx_desc->errors) << 24),
//				  AROS_LE2WORD(rx_desc->csum), skb);
        frame->eth_packet_crc[0] = (AROS_LE2WORD(rx_desc->csum) & 0xff000000) >> 24;
        frame->eth_packet_crc[1] = (AROS_LE2WORD(rx_desc->csum) & 0xff0000) >> 16;
        frame->eth_packet_crc[2] = (AROS_LE2WORD(rx_desc->csum) & 0xff00) >> 8;
        frame->eth_packet_crc[3] = AROS_LE2WORD(rx_desc->csum) & 0xff;

        /* Dump contents of frame if DEBUG enabled */
#ifdef DEBUG
                {
                int j;
                    D(bug("[%s]: Rx Buffer %d Packet Dump -:", dev->e1ku_name, i));
                    for (j=0; j<64; j++) {
                        if ((j%16) == 0)
                            D(bug("\n[%s]:     %03x:", dev->e1ku_name, j));
                        D(bug(" %02x", ((unsigned char*)frame)[j]));
                    }
                    D(bug("\n"));
                }
#endif

        /* Check for address validity */
        if(AddressFilter(LIBBASE, dev, frame->eth_packet_dest))
        {
            /* Packet is addressed to this driver */
D(bug("[%s]: e1000func_clean_rx_irq: Packet IP accepted with type = %d, checksum = %x\n", dev->e1ku_name, AROS_BE2WORD(frame->eth_packet_type), AROS_LE2WORD(rx_desc->csum)));

            opener = (APTR)dev->e1ku_Openers.mlh_Head;
            opener_tail = (APTR)&dev->e1ku_Openers.mlh_Tail;

            /* Offer packet to every opener */
            while(opener != opener_tail)
            {
               request = (APTR)opener->read_port.mp_MsgList.lh_Head;
               request_tail = (APTR)&opener->read_port.mp_MsgList.lh_Tail;
               accepted = FALSE;

               /* Offer packet to each request until it's accepted */
               while((request != request_tail) && !accepted)
               {
                  if((request->ios2_PacketType == AROS_BE2WORD(frame->eth_packet_type))
                     || ((request->ios2_PacketType <= ETH_MTU)
                          && (AROS_BE2WORD(frame->eth_packet_type) <= ETH_MTU)))
                  {
D(bug("[%s]: e1000func_clean_rx_irq: copy packet for opener ..\n", dev->e1ku_name));
                     CopyPacket(LIBBASE, dev, request, length, AROS_BE2WORD(frame->eth_packet_type), frame);
                     accepted = TRUE;
                  }
                  request =
                     (struct IOSana2Req *)request->ios2_Req.io_Message.mn_Node.ln_Succ;
               }

               if(accepted)
                  is_orphan = FALSE;

               opener = (APTR)opener->node.mln_Succ;
            }

            /* If packet was unwanted, give it to S2_READORPHAN request */
            if(is_orphan)
            {
                dev->e1ku_stats.UnknownTypesReceived++;

                if(!IsMsgPortEmpty(dev->e1ku_request_ports[ADOPT_QUEUE]))
                {
                    CopyPacket(LIBBASE, dev,
                        (APTR)dev->e1ku_request_ports[ADOPT_QUEUE]->
                        mp_MsgList.lh_Head, length, AROS_BE2WORD(frame->eth_packet_type), frame);
D(bug("[%s]: e1000func_clean_rx_irq: packet copied to orphan queue\n", dev->e1ku_name));
                }
            }
        }

next_desc:
		rx_desc->status = 0;

		/* return some buffers to hardware, one at a time is too slow */
		if (cleaned_count >= E1000_RX_BUFFER_WRITE) {
//			adapter->alloc_rx_buf(adapter, rx_ring, cleaned_count);
			cleaned_count = 0;
		}

		/* use prefetched values */
		rx_desc = next_rxd;
		buffer_info = next_buffer;
	}
	rx_ring->next_to_clean = i;

	cleaned_count = E1000_DESC_UNUSED(rx_ring);
//	if (cleaned_count)
//		adapter->alloc_rx_buf(adapter, rx_ring, cleaned_count);

    dev->e1ku_stats.PacketsReceived += total_rx_packets;
	//adapter->total_rx_packets += total_rx_packets;
	//adapter->total_rx_bytes += total_rx_bytes;
	//adapter->net_stats.rx_bytes += total_rx_bytes;
	//adapter->net_stats.rx_packets += total_rx_packets;
D(bug("[%s]: e1000func_clean_rx_irq: Received %d packets (%d bytes)\n", dev->e1ku_name, total_rx_packets, total_rx_bytes));

	return cleaned;
}

/** OS SUPPORT CALLS FOR INTEL CODE **/

void e1000_pci_clear_mwi(struct e1000_hw *hw)
{
    struct e1000Unit *unit = (struct e1000Unit *)hw->back;
D(bug("[%s]: e1000_pci_clear_mwi()\n", unit->e1ku_name));
}

void e1000_pci_set_mwi(struct e1000_hw *hw)
{
    struct e1000Unit *unit = (struct e1000Unit *)hw->back;
D(bug("[%s]: e1000_pci_set_mwi()\n", unit->e1ku_name));
}

LONG  e1000_alloc_zeroed_dev_spec_struct(struct e1000_hw *hw, ULONG size)
{
    struct e1000Unit *unit = (struct e1000Unit *)hw->back;
D(bug("[%s]: e1000_alloc_zeroed_dev_spec_struct()\n", unit->e1ku_name));

    if ((hw->dev_spec = AllocMem(size, MEMF_PUBLIC | MEMF_CLEAR)) == NULL)
        return -E1000_ERR_CONFIG;

    return E1000_SUCCESS;
}

LONG  e1000_read_pcie_cap_reg(struct e1000_hw *hw, ULONG reg, UWORD *value)
{
    struct e1000Unit *unit = (struct e1000Unit *)hw->back;
D(bug("[%s]: e1000_read_pcie_cap_reg()\n", unit->e1ku_name));

    return 0;
}

void e1000_free_dev_spec_struct(struct e1000_hw *hw)
{
    struct e1000Unit *unit = (struct e1000Unit *)hw->back;
D(bug("[%s]: e1000_free_dev_spec_struct()\n", unit->e1ku_name));

    if (hw->dev_spec)
        FreeMem(hw->dev_spec, hw->dev_spec_size);
}

void e1000_read_pci_cfg(struct e1000_hw *hw, ULONG reg, UWORD *value)
{
    struct e1000Unit *unit = (struct e1000Unit *)hw->back;
D(bug("[%s]: e1000_read_pci_cfg()\n", unit->e1ku_name));
}

void e1000_write_pci_cfg(struct e1000_hw *hw, ULONG reg, UWORD *value)
{
    struct e1000Unit *unit = (struct e1000Unit *)hw->back;
D(bug("[%s]: e1000_write_pci_cfg()\n", unit->e1ku_name));
}
