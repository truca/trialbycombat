#llama a object para generar los .o antes unir los objetos en el ejecutable "InstanceChecker"
all: 
	g++ -c  solution_checker.cc solution_checker_run.cc
	g++ -o instancechecker  solution_checker_run.o solution_checker.o 

#define clean
clean:
#-r: recursive -f force *.o todos los .o *~ borrar todos los temporales, tambien borra el ejecutable
	rm -rf *.o *~ instancechecker 
