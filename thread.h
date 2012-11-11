#ifndef THREAD_H
#define THREAD_H

struct periodic_info
{
        int timer_fd;
        unsigned long long wakeups_missed;
};

int makePeriodic (unsigned int period, struct periodic_info *info);
void waitPeriod (struct periodic_info *info);

#endif // THREAD_H
