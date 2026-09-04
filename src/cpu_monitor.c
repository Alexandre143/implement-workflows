#include "cpu_monitor.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>

#ifdef _WIN32
static ULONGLONG filetime_to_uint64(const FILETIME *filetime)
{
    ULARGE_INTEGER value;
    value.LowPart = filetime->dwLowDateTime;
    value.HighPart = filetime->dwHighDateTime;
    return value.QuadPart;
}
#endif

double cpu_monitor_get_usage_percent(void)
{
#ifdef _WIN32
    static ULONGLONG previous_idle = 0;
    static ULONGLONG previous_total = 0;
    FILETIME idle_time;
    FILETIME kernel_time;
    FILETIME user_time;

    if (!GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
        return -1.0;
    }

    ULONGLONG idle = filetime_to_uint64(&idle_time);
    ULONGLONG total = filetime_to_uint64(&kernel_time) +
                      filetime_to_uint64(&user_time);

    if (previous_total == 0 || total <= previous_total || idle < previous_idle) {
        previous_idle = idle;
        previous_total = total;
        return 0.0;
    }

    ULONGLONG idle_delta = idle - previous_idle;
    ULONGLONG total_delta = total - previous_total;
    double usage = (1.0 - ((double)idle_delta / (double)total_delta)) * 100.0;

    previous_idle = idle;
    previous_total = total;

    if (usage < 0.0) {
        usage = 0.0;
    } else if (usage > 100.0) {
        usage = 100.0;
    }

    return usage;
#else
    double loadavg[1];
    if (getloadavg(loadavg, 1) != 1) {
        return -1.0;
    }

    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores < 1) {
        num_cores = 1;
    }

    double usage = (loadavg[0] / (double)num_cores) * 100.0;

    if (usage < 0.0) {
        usage = 0.0;
    } else if (usage > 100.0) {
        usage = 100.0;
    }

    return usage;
#endif
}
