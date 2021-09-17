#ifndef RISCV_MMIO_H
#define RISCV_MMIO_H
static __attribute__((always_inline)) void writeb(uint8_t v, volatile void *addr) { *(volatile uint8_t *)addr = v; }
static __attribute__((always_inline)) void writew(uint16_t v, volatile void *addr) { *(volatile uint16_t *)addr = v; }
static __attribute__((always_inline)) void writel(uint32_t v, volatile void *addr) { *(volatile uint32_t *)addr = v; }
static __attribute__((always_inline)) void writeq(uint64_t v, volatile void *addr) { *(volatile uint64_t *)addr = v; }
static __attribute__((always_inline)) uint8_t readb(const volatile void *addr) { return *(const volatile uint8_t *)addr; }
static __attribute__((always_inline)) uint16_t readw(const volatile void *addr) { return *(const volatile uint16_t *)addr; }
static __attribute__((always_inline)) uint32_t readl(const volatile void *addr) { return *(const volatile uint32_t *)addr; }
static __attribute__((always_inline)) uint64_t readq(const volatile void *addr) { return *(const volatile uint64_t *)addr; }
#endif
