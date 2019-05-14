#include <linux/module.h>

#include <linux/sched.h>
#include <trace/events/sched.h>

#define PATH_SIZE		128

static inline struct cfs_rq *sched_trace_group_cfs_rq(struct sched_entity *se)
{
#ifdef CONFIG_FAIR_GROUP_SCHED
       return se->my_q;
#else
       return NULL;
#endif
}

static void sched_load_cfs_rq(void *data, struct cfs_rq *cfs_rq)
{
	const struct sched_avg *avg;
	char path[PATH_SIZE];
	int cpu;

	avg = sched_trace_cfs_rq_avg(cfs_rq);
	sched_trace_cfs_rq_path(cfs_rq, path, PATH_SIZE);
	cpu = sched_trace_cfs_rq_cpu(cfs_rq);

	trace_printk("cpu=%d path=%s load=%lu rbl_load=%lu util=%lu\n",
		     cpu, path, avg->load_avg, avg->runnable_load_avg, avg->util_avg);
}

static void sched_load_se(void *data, struct sched_entity *se)
{
	void *gcfs_rq = sched_trace_group_cfs_rq(se);
	void *cfs_rq = se->cfs_rq;
	struct task_struct *p;
	char path[PATH_SIZE];
	char *comm;
	pid_t pid;
	int cpu;

	sched_trace_cfs_rq_path(gcfs_rq, path, PATH_SIZE);
	cpu = sched_trace_cfs_rq_cpu(cfs_rq);

	p = gcfs_rq ? NULL : container_of(se, struct task_struct, se);
	comm = p ? p->comm : "(null)";
	pid = p ? p->pid : -1;

	trace_printk("cpu=%d path=%s comm=%s pid=%d load=%lu rbl_load=%lu util=%lu\n",
		     cpu, path, comm, pid,
		     se->avg.load_avg, se->avg.runnable_load_avg, se->avg.util_avg);
}

static void sched_overutilized(void *data, int overutilized, struct root_domain *rd)
{
	char span[PATH_SIZE];

	cpumap_print_to_pagebuf(false, span, sched_trace_rd_span(rd));

	trace_printk("overutilized=%d, span=%s\n", overutilized, span);
}

static int lisa_tp_init(void)
{
	register_trace_pelt_cfs(sched_load_cfs_rq, NULL);
	//register_trace_pelt_rt(sched_load_rt, NULL);
	//register_trace_pelt_dl(sched_load_dl, NULL);
	//register_trace_pelt_irq(sched_load_irq, NULL);
	register_trace_pelt_se(sched_load_se, NULL);
	register_trace_sched_overutilized(sched_overutilized, NULL);

	return 0;
}

void lisa_tp_finish(void)
{
	unregister_trace_pelt_cfs(sched_load_cfs_rq, NULL);
	//unregister_trace_pelt_rt(sched_load_rt, NULL);
	//unregister_trace_pelt_dl(sched_load_dl, NULL);
	//unregister_trace_pelt_irq(sched_load_irq, NULL);
	unregister_trace_pelt_se(sched_load_se, NULL);
	unregister_trace_sched_overutilized(sched_overutilized, NULL);
}


module_init(lisa_tp_init);
module_exit(lisa_tp_finish);

MODULE_LICENSE("GPL");
