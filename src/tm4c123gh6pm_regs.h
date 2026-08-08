// tm4c123gh6pm_regs.h
// Minimal register-level definitions for TM4C123GH6PM.
// Self-contained (no TivaWare dependency) so the project builds with just
// the standard Keil ARM core support + TM4C123 device pack for the startup file.
//
// Only the registers actually used by this project are defined here
// (SYSCTL clock gating, GPIO A/B/F, I2C0, UART0, SysTick).

#ifndef TM4C123GH6PM_REGS_H
#define TM4C123GH6PM_REGS_H

#include <stdint.h>

#define REG32(addr) (*((volatile uint32_t *)(addr)))

/* ---------------- System Control (SYSCTL) ---------------- */
#define SYSCTL_RCGCGPIO_R   REG32(0x400FE608)  // GPIO run mode clock gating
#define SYSCTL_RCGCI2C_R    REG32(0x400FE620)  // I2C run mode clock gating
#define SYSCTL_RCGCUART_R   REG32(0x400FE618)  // UART run mode clock gating
#define SYSCTL_PRGPIO_R     REG32(0x400FEA08)  // GPIO peripheral ready
#define SYSCTL_PRI2C_R      REG32(0x400FEA20)  // I2C peripheral ready
#define SYSCTL_PRUART_R     REG32(0x400FEA18)  // UART peripheral ready

/* ---------------- GPIO base addresses (APB aperture) ---------------- */
#define GPIO_PORTA_BASE     0x40004000
#define GPIO_PORTB_BASE     0x40005000
#define GPIO_PORTF_BASE     0x40025000

/* GPIO register offsets (same layout for every port) */
#define GPIO_O_DATA_ALL     0x3FC   // full 8-bit data access (mask = 0xFF)
#define GPIO_O_DIR          0x400
#define GPIO_O_AFSEL        0x420
#define GPIO_O_ODR          0x50C
#define GPIO_O_DEN          0x51C
#define GPIO_O_LOCK         0x520
#define GPIO_O_CR           0x524
#define GPIO_O_AMSEL        0x528
#define GPIO_O_PCTL         0x52C

/* Port A */
#define GPIO_PORTA_DATA_R   REG32(GPIO_PORTA_BASE + GPIO_O_DATA_ALL)
#define GPIO_PORTA_DIR_R    REG32(GPIO_PORTA_BASE + GPIO_O_DIR)
#define GPIO_PORTA_AFSEL_R  REG32(GPIO_PORTA_BASE + GPIO_O_AFSEL)
#define GPIO_PORTA_DEN_R    REG32(GPIO_PORTA_BASE + GPIO_O_DEN)
#define GPIO_PORTA_PCTL_R   REG32(GPIO_PORTA_BASE + GPIO_O_PCTL)

/* Port B */
#define GPIO_PORTB_DATA_R   REG32(GPIO_PORTB_BASE + GPIO_O_DATA_ALL)
#define GPIO_PORTB_DIR_R    REG32(GPIO_PORTB_BASE + GPIO_O_DIR)
#define GPIO_PORTB_AFSEL_R  REG32(GPIO_PORTB_BASE + GPIO_O_AFSEL)
#define GPIO_PORTB_ODR_R    REG32(GPIO_PORTB_BASE + GPIO_O_ODR)
#define GPIO_PORTB_DEN_R    REG32(GPIO_PORTB_BASE + GPIO_O_DEN)
#define GPIO_PORTB_PCTL_R   REG32(GPIO_PORTB_BASE + GPIO_O_PCTL)

/* Port F */
#define GPIO_PORTF_DATA_R   REG32(GPIO_PORTF_BASE + GPIO_O_DATA_ALL)
#define GPIO_PORTF_DIR_R    REG32(GPIO_PORTF_BASE + GPIO_O_DIR)
#define GPIO_PORTF_AFSEL_R  REG32(GPIO_PORTF_BASE + GPIO_O_AFSEL)
#define GPIO_PORTF_DEN_R    REG32(GPIO_PORTF_BASE + GPIO_O_DEN)
#define GPIO_PORTF_LOCK_R   REG32(GPIO_PORTF_BASE + GPIO_O_LOCK)
#define GPIO_PORTF_CR_R     REG32(GPIO_PORTF_BASE + GPIO_O_CR)

/* ---------------- I2C0 ---------------- */
#define I2C0_BASE           0x40020000  // FIX: was 0x40002000 (digits transposed) --
                                         // real I2C0 sits right before GPIO Port E/F
                                         // (0x40024000 / 0x40025000) in the memory map.
#define I2C0_MSA_R          REG32(I2C0_BASE + 0x000)  // Master Slave Address
#define I2C0_MCS_R          REG32(I2C0_BASE + 0x004)  // Master Control/Status
#define I2C0_MDR_R          REG32(I2C0_BASE + 0x008)  // Master Data
#define I2C0_MTPR_R         REG32(I2C0_BASE + 0x00C)  // Master Timer Period
#define I2C0_MCR_R          REG32(I2C0_BASE + 0x020)  // Master Configuration

#define I2C_MCS_ACK         0x00000008
#define I2C_MCS_STOP        0x00000004
#define I2C_MCS_START       0x00000002
#define I2C_MCS_RUN         0x00000001
#define I2C_MCS_ERROR       0x00000002  // read: transmit/receive error
#define I2C_MCS_ADRACK      0x00000004  // read: address ack error
#define I2C_MCS_DATACK      0x00000008  // read: data ack error
#define I2C_MCS_ARBLST      0x00000010  // read: arbitration lost
#define I2C_MCS_BUSY        0x00000001  // read: busy
#define I2C_MCR_MFE         0x00000010  // master function enable

/* ---------------- UART0 ---------------- */
#define UART0_BASE          0x4000C000
#define UART0_DR_R          REG32(UART0_BASE + 0x000)
#define UART0_FR_R          REG32(UART0_BASE + 0x018)
#define UART0_IBRD_R        REG32(UART0_BASE + 0x024)
#define UART0_FBRD_R        REG32(UART0_BASE + 0x028)
#define UART0_LCRH_R        REG32(UART0_BASE + 0x02C)
#define UART0_CTL_R         REG32(UART0_BASE + 0x030)
#define UART0_CC_R          REG32(UART0_BASE + 0xFC8)

#define UART_FR_TXFF        0x00000020  // transmit FIFO full
#define UART_FR_RXFE        0x00000010  // receive FIFO empty
#define UART_LCRH_WLEN_8    0x00000060  // 8-bit word length
#define UART_LCRH_FEN       0x00000010  // FIFO enable
#define UART_CTL_UARTEN     0x00000001
#define UART_CTL_TXE        0x00000100
#define UART_CTL_RXE        0x00000200

/* ---------------- SysTick (ARM core) ---------------- */
#define NVIC_ST_CTRL_R      REG32(0xE000E010)
#define NVIC_ST_RELOAD_R    REG32(0xE000E014)
#define NVIC_ST_CURRENT_R   REG32(0xE000E018)
#define NVIC_ST_CTRL_COUNT  0x00010000
#define NVIC_ST_CTRL_CLK_SRC 0x00000004
#define NVIC_ST_CTRL_ENABLE 0x00000001

#endif