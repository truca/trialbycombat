mrp: main.c
	@gcc -Wall -o mrp main.c

run: mrp
	@./mrp -t 300 -p model_a1_1.txt -i assignment_a1_1.txt -o salida.txt -s 100                                                                    
