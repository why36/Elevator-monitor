.PHONY:all
all:elevator elevator_floor1 elevator_floor2 elevator_floor3 elevator_control
 
elevator_floor1:elevator_floor1.c comm.c
	gcc -o $@ $^ -pthread
elevator_floor2:elevator_floor2.c comm.c
	gcc -o $@ $^ -pthread
elevator_floor3:elevator_floor3.c comm.c
	gcc -o $@ $^ -pthread
elevator_control:elevator_control.c comm.c
	gcc -o $@ $^ -pthread
elevator:elevator.c comm.c
	gcc -o $@ $^ -pthread
 
.PHONY:clean
clean:
	rm -f elevator elevator_floor1 elevator_floor2 elevator_floor3 elevator_control