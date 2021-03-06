#ifndef _ASM_S390_TOPOLOGY_H
#define _ASM_S390_TOPOLOGY_H

#include <linux/cpumask.h>

#define mc_capable()	(1)

cpumask_t cpu_coregroup_map(unsigned int cpu);

extern cpumask_t cpu_core_map[NR_CPUS];

#define topology_core_siblings(cpu)	(cpu_core_map[cpu])

int topology_set_cpu_management(int fc);
void topology_schedule_update(void);

#define POLARIZATION_UNKNWN	(-1)
#define POLARIZATION_HRZ	(0)
#define POLARIZATION_VL		(1)
#define POLARIZATION_VM		(2)
#define POLARIZATION_VH		(3)

#ifdef CONFIG_SMP
void s390_init_cpu_topology(void);
#else
static inline void s390_init_cpu_topology(void)
{
};
#endif

#include <asm-generic/topology.h>

#endif /* _ASM_S390_TOPOLOGY_H */
