#ifndef __REG_H__
#define __REG_H__
uint32_t reg_rd_u32(uint8_t reg_addr);

void reg_wr_u32(uint8_t reg_addr, uint32_t u32);

uint8_t reg_get_msg_len(reg_addr_et reg_addr, master_slave_direction_et master_slave);

void reg_action_on_receive(reg_addr_et reg_addr);

void reg_time_machine(void);

#endif