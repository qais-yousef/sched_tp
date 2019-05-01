#include <linux/module.h>

#include <linux/sched.h>
#include <trace/events/sched.h>

static inline struct cfs_rq *__trace_sched_group_cfs_rq(struct sched_entity *se)
{
#ifdef CONFIG_FAIR_GROUP_SCHED
       return se->my_q;
#else
       return NULL;
#endif
}

static void sched_load_rq(void *data, int cpu, const char* path, struct sched_avg *avg)
{
	if (!path)
		path = "(null)";

	trace_printk("cpu=%d path=%s load=%lu rbl_load=%lu util=%lu\n",
		     cpu, path, avg->load_avg, avg->runnable_load_avg, avg->util_avg);
}

static void sched_load_se(void *data, int cpu, const char* path, struct sched_entity *se)
{
	void *cfs_rq = __trace_sched_group_cfs_rq(se);

	struct task_struct *p = cfs_rq ? NULL : container_of(se, struct task_struct, se);
	char *comm = p ? p->comm : "(null)";
	pid_t pid = p ? p->pid : -1;

	if (!path)
		path = "(null)";

	trace_printk("cpu=%d path=%s comm=%s pid=%d load=%lu rbl_load=%lu util=%lu\n",
		     cpu, path, comm, pid,
		     se->avg.load_avg, se->avg.runnable_load_avg, se->avg.util_avg);
}

static void sched_overutilized(void *data, int overutilized)
{
	trace_printk("overutilized=%d\n", overutilized);
}

static int lisa_tp_init(void)
{
	register_trace_sched_load_rq(sched_load_rq, NULL);
	register_trace_sched_load_se(sched_load_se, NULL);
	register_trace_sched_overutilized(sched_overutilized, NULL);

	return 0;
}

void lisa_tp_finish(void)
{
	unregister_trace_sched_load_rq(sched_load_rq, NULL);
	unregister_trace_sched_load_se(sched_load_se, NULL);
	unregister_trace_sched_overutilized(sched_overutilized, NULL);
}


module_init(lisa_tp_init);
module_exit(lisa_tp_finish);

MODULE_LICENSE("GPL");
