#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void get_cpu(long *a, long *b);
long get_ram(void);

int main(void)
{
    long idle_cpu;
    long active_cpu;

    long quick_idle;
    long mid_idle;
    long persistent_idle;
    long quick_active;
    long mid_active;
    long persistent_active;

    long *Pidle_cpu = &idle_cpu;
    long *Pactive_cpu = &active_cpu;
    
    long *Pquick_idle = &quick_idle;
    long *Pmid_idle = &mid_idle;
    long *Ppersistent_idle = &persistent_idle;
    long *Pquick_active = &quick_active;
    long *Pmid_active = &mid_active;
    long *Ppersistent_active = &persistent_active;

    get_cpu(Pquick_idle, Pquick_active);
    mid_idle = quick_idle;
    mid_active = quick_active;
    persistent_idle = mid_idle;
    persistent_active = mid_active;

    long ram_log[600];

    int count = 0;
    while (count <= 600)
    {
        sleep(1);

        ram_log[count] = get_ram();
        printf("RAM (1 sec avg): %ld%%\n", ram_log[count]);
        get_cpu(Pidle_cpu, Pactive_cpu);
        printf("CPU (1 sec avg): %ld%%\n", ((active_cpu - quick_active) * 100) / ((active_cpu - quick_active) + (idle_cpu - quick_idle)));
        quick_active = active_cpu;
        quick_idle = idle_cpu;
        if (count != 0)
        {
            if (count % 60 == 0)
            {
                printf("CPU (1 min avg): %ld%%\n", ((active_cpu - mid_active) * 100) / ((active_cpu - mid_active) + (idle_cpu - mid_idle)));
                mid_active = active_cpu;
                mid_idle = idle_cpu;
                long total = 0;
                for (int i = 0; i < count; i++)
                {
                    total += ram_log[i];
                }
                printf("RAM (1min avg): %ld%%\n", total / count);
            }
            if (count % 600 == 0)
            {
                printf("CPU (10min avg): %ld%%\n", ((active_cpu - persistent_active) * 100) / ((active_cpu - persistent_active) + (idle_cpu - persistent_idle)));
                persistent_active = active_cpu;
                persistent_idle = idle_cpu;
                long total = 0;
                for (int i = 0; i < count; i++)
                {
                    total += ram_log[i];
                }
                printf("RAM 10min avg): %ld%%\n", total / count);
            }
        }
        count++;
    }
}

void get_cpu(long *a, long *b)
{
    char buffer[256];
    FILE *process;
    process = fopen("/proc/stat", "r");
    if (process == NULL)
    {
        perror("Erro ao abrir arquivo.");
        return;
    }
    if (fgets(buffer, sizeof(buffer), process) == NULL)
    {
        perror("Erro ao ler arquivo.");
        return;
    }
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    sscanf(buffer, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
    long cpu_active = user + nice + system + irq + softirq + steal + guest + guest_nice;
    long cpu_idle = idle + iowait;
    fclose(process);

    *b = cpu_active;
    *a = cpu_idle;
    return;
}

long get_ram(void)
{
    char buffer1[256];
    char buffer2[256];
    char buffer3[256];
    long total;
    long available;
    FILE *meminfo;
    meminfo = fopen("/proc/meminfo", "r");

    fgets(buffer1, sizeof(buffer1), meminfo);
    fgets(buffer2, sizeof(buffer2), meminfo);
    fgets(buffer3, sizeof(buffer3), meminfo);

    sscanf(buffer1, "MemTotal:       %ld kB", &total);
    sscanf(buffer3, "MemAvailable:   %ld kB", &available);

    long utilized = total - available;

    fclose(meminfo);

    return (utilized * 100) / total;
}