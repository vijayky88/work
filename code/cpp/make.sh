#posix
rm -Rf posix_shm posix_shm.o ; g++ posix_shm.cpp -pthread -lrt -o posix_shm
#sysv
rm -Rf queuee queuee.o ; queuee g++ queuee.cpp -o queuee
rm -Rf systemv_shm systemv_shm.o ; g++ systemv_shm.cpp -o systemv_shm
rm -Rf shmtool shmtool.o ; g++ shmtool.cpp -o shmtool
rm -Rf msgtool msgtool.o ; g++ msgtool.cpp -o msgtool
rm -Rf semtool semtool.o ; g++ semtool.cpp -o semtool
rm -Rf shmget  shmget.o  ; gcc shmget.c  -o shmget
rm -Rf shmctl  shmctl.o  ; gcc shmctl.c  -o shmctl
rm -Rf shmop   shmop.o   ; gcc shmop.c   -o shmop
rm -Rf cook    cook.o    ; gcc cook.c -pthread -lrt -o cook
rm -Rf waiter  waiter.o  ; gcc waiter.c -pthread -lrt -o waiter
rm -Rf mutex   mutex.o   ; gcc mutex.c -pthread -lrt -o  mutex
rm -Rf threads  threads.o  ; gcc -o threads threads.cpp -lpthread
#generic
rm -Rf signal signal.o ; g++ signal.cpp -o signal
rm -Rf timer  timer.o  ; gcc timer.c    -o timer
rm -Rf timer1  timer1.o  ; gcc timer1.c    -o timer1
rm -Rf timer_unix1 timer_unix1.o ; gcc timer_unix1.c   -o timer_unix1
rm -Rf timer_unix2 timer_unix2.o ; gcc timer_unix2.c   -o timer_unix2 -lrt
  
