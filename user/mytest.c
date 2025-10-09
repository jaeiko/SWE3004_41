#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
waitpid_test()
{
  int pids[2];
  int i;

  printf(">>> Testing waitpid:\n");
  printf("wait\n");
  
  for(i = 0; i < 2; i++){
    pids[i] = fork();
    if(pids[i] < 0){
      printf("fork failed\n");
      exit(1);
    }
    if(pids[i] == 0){
      setnice(getpid(), 10);
      printf("start%d\n", i + 1);
      pause(20 + i * 5);
      printf("end%d\n", i + 1);
      exit(0);
    }
  }

  for(i = 0; i < 2; i++){
    int child_pid = pids[i];
    waitpid(child_pid);
    printf("done%d %d %d\n", i + 1, child_pid, 10);
  }
}

int
main(void)
{
  int pid = getpid();
  
  printf(">>>Testing getnice and setnice:\n");
  printf("initial nice value: %d\n", getnice(pid));
  setnice(pid, 10);
  printf("nice value after setting: %d\n", getnice(pid));

  printf(">>>Testing ps:\n");
  ps(0);

  printf(">>>Testing meminfo:\n");
  printf("available memory: %d bytes\n", meminfo());
  
  waitpid_test();
  
  exit(0);
}
