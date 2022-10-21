INPUTS =  1 2 3 4 5
PROCESSES = 4

build:
	gcc rot13_serial.c -o rot13_serial -g
	gcc rot13_openmp.c -o rot13_openmp -g -fopenmp
	gcc rot13_pthread.c -o rot13_pthread -lpthread -g
	mpicc rot13_mpi.c -o rot13_mpi -g
	mpicc rot13_mpi_openmp.c -o rot13_mpi_openmp -g -fopenmp
	mpicc rot13_mpi_pthread.c -o rot13_mpi_pthread -g -lpthread

	for I in $(INPUTS) ; do \
		for P in $(PROCESSES) ; do \
            mkdir -p out/in$$I/P$$P ; \
        done  \
    done	

run:
	make run_tests

collect:
	make collect_tests

run_tests:
	for I in $(INPUTS) ; do \
		make run_serial INPUT=$$I ; \
		for P in $(PROCESSES) ; do \
            make run_all_tests P=$$P INPUT=$$I ; \
        done  \
    done

collect_tests:
	for I in $(INPUTS) ; do \
		make collect_serial INPUT=$$I ; \
		for P in $(PROCESSES) ; do \
            make collect_all_tests P=$$P INPUT=$$I ; \
        done  \
    done

run_all_tests:
	make run_openmp
	make run_pthreads
	make run_mpi
	make run_mpi_openmp
	make run_mpi_pthread

collect_all_tests:
	make collect_openmp
	make collect_pthreads
	make collect_mpi
	make collect_mpi_openmp
	make collect_mpi_pthread

run_serial:
	./rot13_serial $(INPUT) 

collect_serial:
	collect -o serial.er -d Profiling/Results/in$(INPUT) ./rot13_serial $(INPUT)

run_openmp:
	./rot13_openmp $(INPUT) $(P)

collect_openmp:
	collect -o openmp.er -d Profiling/Results/in$(INPUT)/P$(P) ./rot13_openmp $(INPUT) $(P)

run_pthreads:
	./rot13_pthread $(INPUT) $(P)

collect_pthreads:
	collect -o pthreads.er -d Profiling/Results/in$(INPUT)/P$(P) ./rot13_pthread $(INPUT) $(P)

run_mpi:
	mpirun -np $(P) ./rot13_mpi $(INPUT) $(P)

collect_mpi:
	collect -o mpi.er -d Profiling/Results/in$(INPUT)/P$(P) mpirun -np $(P) ./rot13_mpi $(INPUT) $(P)

run_mpi_openmp:
	mpirun -np $(P) ./rot13_mpi_openmp $(INPUT) $(P)

collect_mpi_openmp:
	collect -o mpi_openmp.er -d Profiling/Results/in$(INPUT)/P$(P) mpirun -np $(P) ./rot13_mpi_openmp $(INPUT) $(P)

run_mpi_pthread:
	mpirun -np $(P) ./rot13_mpi_pthread $(INPUT) $(P)

collect_mpi_pthread:
	collect -o mpi_pthread.er -d Profiling/Results/in$(INPUT)/P$(P) mpirun -np $(P) ./rot13_mpi_pthread $(INPUT) $(P)

diff:
	for I in $(INPUTS) ; do \
		for P in $(PROCESSES) ; do \
            ./diff.sh $$I $$P ; \
        done  \
    done

create_solaris_dir:
	for I in $(INPUTS) ; do \
		for P in $(PROCESSES) ; do \
            mkdir -p Profiling/Results/in$$I/P$$P ; \
			touch Profiling/Results/in$$I/P$$P/.gitkeep ; \
        done  \
    done

delete_solaris_dir:
	for I in $(INPUTS) ; do \
		for P in $(PROCESSES) ; do \
            rm -fr Profiling/Results/in$$I/P$$P/ ; \
        done ; \
		rm -fr Profiling/Results/in$$I ; \
    done

clean:
	rm -f rot13_serial rot13_openmp rot13_pthread rot13_mpi rot13_mpi_openmp rot13_mpi_pthread
	rm -fr out
#	rm -fr test.*
	rm -f log_run.txt log_collect.txt
	rm -f core.* run.sh.*

