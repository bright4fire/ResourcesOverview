#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

long get_cpu(void);
void get_ram(void);

int main(void)
{
    long fresh_measure = get_cpu();
    long mid_measure = get_cpu();
    long persistent_measure = get_cpu();
    long new_measure;

    if ((fresh_measure && mid_measure && persistent_measure) == 0)
    {
        return 1;
    }

    int count = 0;
    while (count <= 600)
    {
        sleep(1);
        new_measure = get_cpu();
        printf("CPU (1 sec avg): %ld\n", new_measure - fresh_measure);
        fresh_measure = new_measure;
        if (count != 0)
        {
            if (count % 60 == 0)
            {
                long new_measure = get_cpu();
                printf("CPU (1 min avg): %ld\n", new_measure - mid_measure);
                mid_measure = new_measure;
            }
            if (count % 600 == 0)
            {
                long new_measure = get_cpu();
                printf("CPU (10min avg): %ld\n", new_measure - persistent_measure);
                persistent_measure = new_measure;
            }
        }
        count++;
    }
}

long get_cpu(void)
{
    char buffer[256];
    FILE *process;
    process = fopen("/proc/stat", "r");
    if (process == NULL)
    {
        perror("Erro ao abrir arquivo.");
        return 0;
    }
    if (fgets(buffer, sizeof(buffer), process) == NULL)
    {
        perror("Erro ao ler arquivo.");
        return 0;
    }
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    sscanf(buffer, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
    long cpu_active = user + nice + system + irq + softirq + steal + guest + guest_nice;
    long cpu_idle = idle + iowait;
    fclose(process);
    return cpu_active;
}