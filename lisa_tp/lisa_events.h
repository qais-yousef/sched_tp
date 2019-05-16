/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM lisa

#if !defined(_LISA_EVENTS_H) || defined(TRACE_HEADER_MULTI_READ)
#define _LISA_EVENTS_H

#define PATH_SIZE		128

#include <linux/tracepoint.h>

TRACE_EVENT(sched_load_cfs_rq,

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
		__entry->rbl_load	= avg->runnable_load_avg;
		__entry->util		= avg->util_avg;
	),

	TP_printk("cpu=%d path=%s load=%lu rbl_load=%lu util=%lu",
		  __entry->cpu, __entry->path, __entry->load,
		  __entry->rbl_load,__entry->util)
);

#endif /* _LISA_EVENTS_H */

/* This part must be outside protection */
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE lisa_events
#include <trace/define_trace.h>
