/* SPDX-License-Identifier: GPL-2.0 */
#ifndef SCHED_TP_HELPERS_H
#define SCHED_TP_HELPERS_H

/* Required for struct irq_work which is defined in struct root_domain */
#include <linux/irq_work.h>

#include "vmlinux_deps.h"
#include "vmlinux.h"

#define cpu_of(rq)	rq->cpu
#define rq_of(cfs_rq)	cfs_rq->rq

static inline const struct sched_avg *sched_tp_cfs_rq_avg(struct cfs_rq *cfs_rq)
{
#ifdef CONFIG_SMP
	return cfs_rq ? &cfs_rq->avg : NULL;
#else
	return NULL;
#endif
}

static inline char *sched_tp_cfs_rq_path(struct cfs_rq *cfs_rq, char *str, int len)
{
	if (!cfs_rq) {
		if (str)
			strlcpy(str, "(null)", len);
		else
			return NULL;
	}

	cfs_rq_tg_path(cfs_rq, str, len);
	return str;
}

static inline int sched_tp_cfs_rq_cpu(struct cfs_rq *cfs_rq)
{
	return cfs_rq ? cpu_of(rq_of(cfs_rq)) : -1;
}

static inline const struct sched_avg *sched_tp_rq_avg_rt(struct rq *rq)
{
#ifdef CONFIG_SMP
	return rq ? &rq->avg_rt : NULL;
#else
	return NULL;
#endif
}

static inline const struct sched_avg *sched_tp_rq_avg_dl(struct rq *rq)
{
#ifdef CONFIG_SMP
	return rq ? &rq->avg_dl : NULL;
#else
	return NULL;
#endif
}

static inline const struct sched_avg *sched_tp_rq_avg_irq(struct rq *rq)
{
#if defined(CONFIG_SMP) && defined(CONFIG_HAVE_SCHED_AVG_IRQ)
	return rq ? &rq->avg_irq : NULL;
#else
	return NULL;
#endif
}

static inline int sched_tp_rq_cpu(struct rq *rq)
{
	return rq ? cpu_of(rq) : -1;
}

static inline int sched_tp_rq_cpu_capacity(struct rq *rq)
{
	return rq ?
#ifdef CONFIG_SMP
		rq->cpu_capacity
#else
		SCHED_CAPACITY_SCALE
#endif
		: -1;
}

static inline const struct cpumask *sched_tp_rd_span(struct root_domain *rd)
{
#ifdef CONFIG_SMP
	return rd ? rd->span : NULL;
#else
	return NULL;
#endif
}

static inline int sched_tp_rq_nr_running(struct rq *rq)
{
	return rq ? rq->nr_running : -1;
}

#endif /* SCHED_TP_HELPERS */
