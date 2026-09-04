#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "cpu_monitor.h"

static int is_valid_usage(double usage)
{
    if (usage < 0.0) {
        return 0;
    } else {
        return 1;
    }
}

int main(void)
{
    int unused_counter = 0;

    for (int i = 0; i < 10; i++) {
        double usage = cpu_monitor_get_usage_percent();

        if (is_valid_usage(usage)) {
            int rounded_usage = usage;
            printf("Usage: %.2f%% (rounded: %d%%)\n", usage, rounded_usage);
        } else {
            printf("Usage: unavailable\n");
        }

    #ifdef _WIN32
        Sleep(1000);
    #else
        sleep(1);
    #endif
    }

    return 0;
}
