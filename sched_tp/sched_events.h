/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM sched

#if !defined(_SCHED_EVENTS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _SCHED_EVENTS_H

#define PATH_SIZE		128
#define SPAN_SIZE		128/4	/* assuming a max of 128 cpu system! */

#include <linux/version.h>
#include <linux/tracepoint.h>

TRACE_EVENT(sched_pelt_cfs,

	TP_PROTO(int cpu, char *path, const struct sched_avg *avg),

	TP_ARGS(cpu, path, avg),

	TP_STRUCT__entry(
		__field(	int,		cpu			)
		__array(	char,		path,	PATH_SIZE	)
		__field(	unsigned long,	load			)
		__field(	unsigned long,	rbl_load		)
		__field(	unsigned long,	util			)
	),

	TP_fast_assign(
		__entry->cpu		= cpu;
		strlcpy(__entry->path, path, PATH_SIZE);
		__entry->load		= avg->load_avg;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,7,0)
		__entry->rbl_load	= avg->runnable_load_avg;
#else
		__entry->rbl_load	= avg->runnable_avg;
#endif
		__entry->util		= avg->util_avg;
	),

	TP_printk("cpu=%d path=%s load=%lu rbl_load=%lu util=%lu",
		  __entry->cpu, __entry->path, __entry->load,
		  __entry->rbl_load,__entry->util)
);

DECLARE_EVENT_CLASS(sched_pelt_rq_template,

	TP_PROTO(int cpu, const struct sched_avg *avg),

	TP_ARGS(cpu, avg),

	TP_STRUCT__entry(
		__field(	int,		cpu			)
		__field(	unsigned long,	load			)
		__field(	unsigned long,	rbl_load		)
		__field(	unsigned long,	util			)
	),

	TP_fast_assign(
		__entry->cpu		= cpu;
		__entry->load		= avg->load_avg;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,7,0)
		__entry->rbl_load	= avg->runnable_load_avg;
#else
		__entry->rbl_load	= avg->runnable_avg;
#endif
		__entry->util		= avg->util_avg;
	),

	TP_printk("cpu=%d load=%lu rbl_load=%lu util=%lu",
		  __entry->cpu, __entry->load,
		  __entry->rbl_load,__entry->util)
);

DEFINE_EVENT(sched_pelt_rq_template, sched_pelt_rt,
	TP_PROTO(int cpu, const struct sched_avg *avg),
	TP_ARGS(cpu, avg));

DEFINE_EVENT(sched_pelt_rq_template, sched_pelt_dl,
	TP_PROTO(int cpu, const struct sched_avg *avg),
	TP_ARGS(cpu, avg));

DEFINE_EVENT(sched_pelt_rq_template, sched_pelt_irq,
	TP_PROTO(int cpu, const struct sched_avg *avg),
	TP_ARGS(cpu, avg));

TRACE_EVENT(sched_pelt_se,

	TP_PROTO(int cpu, char *path, char *comm, int pid, const struct sched_avg *avg),

	TP_ARGS(cpu, path, comm, pid, avg),

	TP_STRUCT__entry(
		__field(	int,		cpu			)
		__array(	char,		path,	PATH_SIZE	)
		__array(	char,		comm,	TASK_COMM_LEN	)
		__field(	int,		pid			)
		__field(	unsigned long,	load			)
		__field(	unsigned long,	rbl_load		)
		__field(	unsigned long,	util			)
	),

	TP_fast_assign(
		__entry->cpu		= cpu;
		strlcpy(__entry->path, path, PATH_SIZE);
		strlcpy(__entry->comm, comm, TASK_COMM_LEN);
		__entry->pid		= pid;
		__entry->load		= avg->load_avg;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,7,0)
		__entry->rbl_load	= avg->runnable_load_avg;
#else
		__entry->rbl_load	= avg->runnable_avg;
#endif
		__entry->util		= avg->util_avg;
	),

	TP_printk("cpu=%d path=%s comm=%s pid=%d load=%lu rbl_load=%lu util=%lu",
		  __entry->cpu, __entry->path, __entry->comm, __entry->pid,
		  __entry->load, __entry->rbl_load,__entry->util)
);

TRACE_EVENT(sched_overutilized,

	TP_PROTO(int overutilized, char *span),

	TP_ARGS(overutilized, span),

	TP_STRUCT__entry(
		__field(	int,		overutilized		)
		__array(	char,		span,	SPAN_SIZE	)
	),

	TP_fast_assign(
		__entry->overutilized	= overutilized;
		strlcpy(__entry->span, span, SPAN_SIZE);
	),

	TP_printk("overutilized=%d span=0x%s",
		  __entry->overutilized, __entry->span)
);

TRACE_EVENT(sched_update_nr_running,

	    TP_PROTO(int cpu, int change, unsigned int nr_running),

	    TP_ARGS(cpu, change, nr_running),

	    TP_STRUCT__entry(
			     __field(         int,        cpu           )
			     __field(         int,        change        )
			     __field(unsigned int,        nr_running    )
			     ),

	    TP_fast_assign(
			   __entry->cpu        = cpu;
			   __entry->change     = change;
			   __entry->nr_running = nr_running;
			   ),

	    TP_printk("cpu=%d change=%d nr_running=%d", __entry->cpu, __entry->change, __entry->nr_running)
	    );

#endif /* _SCHED_EVENTS_H */

/* This part must be outside protection */
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE sched_events
#include <trace/define_trace.h>
