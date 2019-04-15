#include <linux/module.h>

#include <linux/sched.h>
#include <trace/events/pelt.h>

static void pelt_se_probe(void *data, struct sched_entity *se)
{
	trace_printk("pelt_se: util_avg=%lu load_avg=%lu\n", se->avg.util_avg, se->avg.load_avg);
}

static int probe_tp_pelt_se_init(void)
{
	register_trace_pelt_se(pelt_se_probe, NULL);

	return 0;
}

void probe_tp_pelt_se_finish(void)
{
	unregister_trace_pelt_se(pelt_se_probe, NULL);
}


module_init(probe_tp_pelt_se_init);
module_exit(probe_tp_pelt_se_finish);

MODULE_LICENSE("GPL");
