// SPDX-License-Identifier: GPL-2.0-only
/*
 * Low-level idle sequences
 */

#include <linux/cpu.h>
#include <linux/irqflags.h>

#include <asm/barrier.h>
#include <asm/cpuidle.h>
#include <asm/cpufeature.h>
#include <asm/sysreg.h>

/*
 *	cpu_do_idle()
 *
 *	Idle the processor (wait for interrupt).
 *
 *	If the CPU supports priority masking we must do additional work to
 *	ensure that interrupts are not masked at the PMR (because the core will
 *	not wake up if we block the wake up signal in the interrupt controller).
 */
void __cpuidle cpu_do_idle(void)
{
	struct arm_cpuidle_irq_context context;

	arm_cpuidle_save_irq_context(&context);

	dsb(sy);
	wfi();

	arm_cpuidle_restore_irq_context(&context);
}

/*
 * This is our default idle handler.
 */
void __cpuidle arch_cpu_idle(void)
{
	/* gts9wifi: pet Gunyah-virtualized WDT before idling.
	 * SMC 0x86000007 from EL1 traps to EL2 (Gunyah).
	 */
	register unsigned long x0 asm("x0") = 0x86000007UL;
	asm volatile ("smc #0" : "+r" (x0)
			       : : "x1", "x2", "x3", "x4", "x5", "x6", "x7",
			           "x8", "x9", "x10", "x11", "x12", "x13",
			           "x14", "x15", "x16", "x17", "memory");
	/*
	 * This should do all the clock switching and wait for interrupt
	 * tricks
	 */
	cpu_do_idle();
}
