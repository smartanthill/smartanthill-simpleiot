/*******************************************************************************
Copyright (C) 2015 OLogN Technologies AG

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*******************************************************************************/


#if !defined __SIOT_M_PROTOCOL_H__
#define __SIOT_M_PROTOCOL_H__

#include "siot_common.h"
#include "../simpleiot_hal/siot_mem_mngmt.h"
#include <hal_time_provider.h>
#include "../simpleiot_hal/hal_waiting.h"

extern uint16_t DEVICE_SELF_ID;

#define SIOT_MESH_ANY_PACKET 0 // (used for other purposes) 	Samp-Unicast-Data-Packet
#define SIOT_MESH_FROM_SANTA_DATA_PACKET 1 // 	Samp-From-Santa-Data-Packet
#define SIOT_MESH_TO_SANTA_DATA_OR_ERROR_PACKET 2 // 	Samp-To-Santa-Data-Packet
#define SIOT_MESH_FORWARD_TO_SANTA_DATA_OR_ERROR_PACKET 3 // 	Samp-Forward-To-Santa-Data-Or-Error-Packet
#define SIOT_MESH_ROUTING_ERROR_PACKET 4 // 	Samp-Routing-Error-Packet
#define SIOT_MESH_ACK_NACK_PACKET 5 // 	Samp-Ack-Nack-Packet

#define SIOT_MESH_GENERIC_EXTRA_HEADER_FLAGS 0
#define SIOT_MESH_GENERIC_EXTRA_HEADER_COLLISION_DOMAIN 1
#define SIOT_MESH_UNICAST_EXTRA_HEADER_LOOP_ACK 2
#define SIOT_MESH_TOSANTA_EXTRA_HEADER_LAST_INCOMING_HOP 3

// Route table MODIFICATIONS-LIST entry types
#define ADD_OR_MODIFY_LINK_ENTRY 0
#define DELETE_LINK_ENTRY 1
#define ADD_OR_MODIFY_ROUTE_ENTRY 2
#define DELETE_ROUTE_ENTRY 3

// SIOT_MESH data structures

// link table item
typedef struct _SIOT_MESH_LINK
{
	uint16_t LINK_ID; // type is inspired in section "Communicating Routing Table Information over SACCP" by "Encoded-Unsigned-Int<max=2> bitfield substrate ... bits[4..] equal to LINK-ID"
	uint16_t NEXT_HOP; // note: this link will be common for all targets that are reachable through a device with NEXT_HOP device ID
	uint16_t BUS_ID; // type is inspired in section "Communicating Routing Table Information over SACCP" by "BUS-ID is an Encoded-Unsigned-Int<max=2> field"
	uint8_t INTRA_BUS_ID; // INTRA-BUS-ID=NULL means that the entry is for an incoming link. Incoming link entries are relatiely rare, and are used to specify LINK-DELAYs.
	uint8_t NEXT_HOP_ACKS; // NEXT-HOP-ACKS is a flag which is set if the nearest hop (over (BUS-ID,INTRA-BUS-ID)) is known to be able not only to receive packets, but to send ACKs back
	uint16_t LINK_DELAY_UNIT; // type is inspired: same section as above
	uint16_t LINK_DELAY; // type is inspired: same section as above
	uint16_t LINK_DELAY_ERROR; // type is inspired: same section as above
} SIOT_MESH_LINK;

// rout table item
typedef struct _SIOT_MESH_ROUTE
{
	uint16_t TARGET_ID;
	uint16_t LINK_ID;
} SIOT_MESH_ROUTE;

typedef struct _SIOT_MESH_RETRANSM_COMMON_DATA
{
	uint8_t MAX_TTL;
	uint8_t FORWARD_TO_SANTA_DELAY_UNIT;
	uint8_t FORWARD_TO_SANTA_DELAY;
	uint8_t MAX_FORWARD_TO_SANTA_DELAY; // (using same units as FORWARD-TO-SANTA-DELAY); indicates maximum "forward to santa" delay for all Retransmitting Devices in the PAN.
	uint8_t NODE_MAX_RANDOM_DELAY_UNIT;
	uint8_t NODE_MAX_RANDOM_DELAY;
} SIOT_MESH_RETRANSM_COMMON_DATA;


// SIOT_MESH ret codes
#define SIOT_MESH_RET_OK 0
#define SIOT_MESH_RET_ERROR_ANY 1
#define SIOT_MESH_RET_GARBAGE_RECEIVED 2
#define SIOT_MESH_RET_NOT_FOR_THIS_DEV_RECEIVED 3
#define SIOT_MESH_RET_PASS_TO_PROCESS 4
#define SIOT_MESH_RET_PASS_TO_SEND 5
// internal errors (TODO: should not be exposed)
#define SIOT_MESH_RET_ERROR_NOT_FOUND 6
#define SIOT_MESH_RET_ERROR_OUT_OF_RANGE 7


#ifdef USED_AS_MASTER
uint8_t handler_siot_mesh_receive_packet( MEMORY_HANDLE mem_h, uint8_t conn_quality );
uint8_t handler_siot_mesh_send_packet( MEMORY_HANDLE mem_h, uint16_t target_id, uint16_t* link_id );
uint8_t handler_siot_mesh_timer( sa_time_val* currt, waiting_for* wf, MEMORY_HANDLE mem_h );

#ifdef __cplusplus
extern "C" {
#endif

uint8_t siot_mesh_at_root_target_to_link_id( uint16_t target_id, uint16_t* link_id );
void siot_mesh_at_root_add_last_hop_in_data( uint16_t src_id, uint16_t last_hop_id, uint16_t last_hop_bus_id, uint8_t conn_q );
uint16_t siot_mesh_at_root_find_best_route( uint16_t target_id, uint16_t* bus_id_at_target, uint16_t* id_prev, uint16_t* bus_id_at_prev, uint16_t* id_next );
uint16_t siot_mesh_at_root_remove_last_hop_data( uint16_t target_id );
uint8_t siot_mesh_at_root_add_updates_for_device( uint16_t id_target, uint16_t bus_to_send_from_target, uint16_t id_prev, uint16_t bust_to_send_from_prev, uint16_t id_next /*more data may be required*/ );
void siot_mesh_at_root_add_last_hop_out_data( uint16_t src_id, uint16_t bus_id_at_src, uint16_t first_receiver_id, uint8_t conn_q );

#ifdef __cplusplus
}
#endif

#else // USED_AS_MASTER

uint8_t handler_siot_mesh_receive_packet( MEMORY_HANDLE mem_h, uint8_t* mesh_val, uint8_t signal_level, uint8_t error_cnt );
uint8_t handler_siot_mesh_timer( sa_time_val* currt, waiting_for* wf, MEMORY_HANDLE mem_h );
uint8_t handler_siot_mesh_packet_rejected_broken( /*MEMORY_HANDLE mem_h, */uint8_t* mesh_val );
uint8_t handler_siot_mesh_send_packet( MEMORY_HANDLE mem_h, uint8_t* mesh_val, uint16_t target_id, uint16_t* link_id );

#endif // USED_AS_MASTER

void handler_siot_process_route_update_request( parser_obj* po, MEMORY_HANDLE reply );

void siot_mesh_init_tables();  // TODO: this call reflects current development stage and may or may not survive in the future


#endif // __SIOT_M_PROTOCOL_H__
