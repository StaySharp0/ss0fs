/*
 * Reference: include/uapi/linux/oom.h
 */
#ifndef __OOM_H__
#define __OOM_H__

/*
 * /proc/<pid>/oom_score_adj set to OOM_SCORE_ADJ_MIN disables oom killing for
 * pid.
 */
#define OOM_SCORE_ADJ "/proc/self/oom_score_adj"
#define OOM_SCORE_ADJ_MIN (-1000)
#define OOM_SCORE_ADJ_MIN_STR "-1000"

/*
 * /proc/<pid>/oom_adj set to -17 protects from the oom killer for legacy
 * purposes.
 */
#define OOM_ADJ "/proc/self/oom_adj"
#define OOM_DISABLE (-17)
#define OOM_DISABLE_STR "-17"

#endif // !__OOM_H__
