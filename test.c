#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdlib.h>
//#include <perfmon/pfmlib_perf_event.h>
#include <sched.h>
#include <syscall.h>
#include <linux/futex.h>
#include <linux/kvm.h>
//edit local /usr/include/linux/kvm.h to add self-defined MACROs

#ifdef DEBUG
#define PRINT_RAW_DATA
#define debug_print(...) fprintf (stderr, __VA_ARGS__)
#else
#define debug_print(...)
#endif

static unsigned long __inline__ rdtsc(void)
{
	  unsigned int tickl, tickh;
	    __asm__ __volatile__("rdtscp":"=a"(tickl),"=d"(tickh)::"%ecx");
		  return ((uint64_t)tickh << 32)|tickl;
}

int main(int argc, char **argv)
{
	int fd = open("/dev/kvm", O_RDONLY| O_CLOEXEC);
	if (fd < 0){
		fprintf(stderr, "Can't open /dev/kvm\n");
		exit(0);
	}
	
	//while(1){
		//usleep(500); //500us
		//unsigned long begin = rdtsc();
	//ioctl(fd, KVM_GET_MEMSLOTS, NULL);
	//ioctl(fd, KVM_GET_FAULTS, NULL);
	ioctl(fd, KVM_SET_UNPRESET, NULL);
		//unsigned long end = rdtsc();
		//printf("napping %ld cycles\n", end - begin);
	//}
}
