#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

struct machine {
    int neighborhood;
    int location;
    int* capacities;
    int* safety_capacities;
    int* resources_used;
    //int* transient_resources_used;
    int* machine_move_costs;
};
struct resource{
    int transient;
    int weight_load_cost;
};
struct service{
    int spread_min;
    int amount_of_dependencies;
    int* dependent_on_service;
    int* locations;
};
struct process{
    int service;
    int* requirements;
    int process_move_cost;
};
struct balance_cost{
    int resource_one;
    int resource_two;
    int target;
    int weight;
};
struct variables{
    int resources_amount;
    int machines_amount;
    int services_amount;
    int processes_amount;
    int balance_costs_amount;
    
    int process_move_cost;
    int service_move_cost;
    int machine_move_cost;
    
    //struct machine* machines;
    struct resource* resources;
    struct machine* machines;
    struct service* services;
    struct process* processes;
    struct balance_cost* balance_costs;
};
struct variables var;
struct solution{
    double best_value;
    double value;
    int* best_process_asignations;
    int* process_asignations;
    int* initial_solution;
};
struct solution sol;
struct possible_moves{
    int* possible_moves;
    int length;
};
struct tabu_item{
	int machine;
	int process;	
};
struct tabu_list{
    struct tabu_item* list;
    int length;
};
struct tabu_list tabu;
struct time_container{
    int limit;
    clock_t start;
};
struct time_container tiempo;

void print_solution(int* sol);
void update_used_resources();
void evaluate_and_verify_solution();

void read_resources(FILE *file){
    int resources_amount, i, num, length;
    char * line = NULL;
    size_t len = 0;
    
    getline(&line, &len, file);
    printf("resources: %s", line);
    resources_amount = atoi(line);
    
    var.resources = malloc(sizeof(struct resource)*resources_amount);
    var.resources_amount = resources_amount;
    
    //getline(&line, &len, file);
    for(i = 0; i<resources_amount; i++){
        getline(&line, &len, file);
        //getting transient
        sscanf( line, "%d%n", &num, &length);
        var.resources[i].transient = num;
        line += length;
        //printf("transient: %d\n", num);
        
        //getting weight load cost
        sscanf( line, "%d%n", &num, &length);
        var.resources[i].weight_load_cost = num;
        line += length;
        //printf("load_cost: %d\n", num);
    }
}
void read_machines(FILE *file){
    int machines_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    
    getline(&line, &len, file);
    printf("machines: %d\n", atoi(line));
    machines_amount = atoi(line);
    
    var.machines = malloc(sizeof(struct machine)*machines_amount);
    var.machines_amount = machines_amount;
    
    for(i=0; i<machines_amount; i++){
        //blank line
        getline(&line, &len, file);
        //getting neighborhood
        sscanf( line, "%d%n", &num, &length);
        var.machines[i].neighborhood = num;
        //getting location
        sscanf( line, "%d%n", &num, &length);
        var.machines[i].location = num;
        //getting capacities
        //printf("getting capacities\n");
        var.machines[i].capacities = malloc(sizeof(int)*var.resources_amount);
        var.machines[i].resources_used = malloc(sizeof(int)*var.resources_amount);
        //printf("ra alone: %d\n", var.resources_amount);
        //var.machines[i].transient_resources_used = malloc(sizeof(int)*var.resources_amount);
        //printf("Capacity:\n");
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            var.machines[i].capacities[j]=num;
            var.machines[i].resources_used[j] = 0;
            //var.machines[i].transient_resources_used[j] = 0;
            //printf( "Capacity %d is %d\n", j, num );
            line += length;
        }
        //getting safety capacities
        var.machines[i].safety_capacities = malloc(sizeof(int)*var.resources_amount);
        //printf("Safety Capacities:\n");
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            var.machines[i].safety_capacities[j]=num;
            //printf( "Number %d is %d\n", j, num );
            line += length;
        }
        //getting moving costs
        var.machines[i].machine_move_costs = malloc(sizeof(int)*machines_amount);
        //printf("Moving Costs:\n");
        for(j=0; j < machines_amount; j++){
            //sscanf( line, "%d%n", &num, &length);
            var.machines[i].machine_move_costs[j]=num;
            //printf( "Number %d is %d\n", j, num );
            line += length;
        }
    }
}
void read_services(FILE *file){
    int services_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    
    getline(&line, &len, file);
    printf("services: %s", line);
    services_amount = atoi(line);
    var.services_amount = services_amount;
    
    var.services = malloc(sizeof(struct service)*services_amount);
    
    for(i=0; i < services_amount; i++){
        getline(&line, &len, file);
        //getline(&line, &len, file);
        sscanf( line, "%d%n", &num, &length);
        var.services[i].spread_min = num;
        //printf("spread_min: %d \n", num);
        line += length;
        sscanf( line, "%d%n", &num, &length);
        //printf("dependencies: %d \n", num);
        var.services[i].amount_of_dependencies = num;
        var.services[i].dependent_on_service = malloc(sizeof(int)*var.services[i].amount_of_dependencies);
        line += length;
        for(j=0; j < var.services[i].amount_of_dependencies; j++){
            sscanf( line, "%d%n", &num, &length);
            //printf("dependency %d: %d \n", j, num);
            var.services[i].dependent_on_service[j] = num;
            line += length;
        }
    }
}
void read_processes(FILE *file){
    int processes_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    
    getline(&line, &len, file);
    printf("processes: %s", line);
    processes_amount = atoi(line);
    var.processes_amount = processes_amount;
    
    var.processes = malloc(sizeof(struct process)*processes_amount);
    
    for(i=0; i<processes_amount; i++){
        //printf("process %d: \n", i);
        //blank line
        getline(&line, &len, file);
        //getting service
        sscanf( line, "%d%n", &num, &length);
        //printf("Belongs to service: %d\n", num);
        var.processes[i].service = num;
        line += length;
        //getting requirements
        var.processes[i].requirements = malloc(sizeof(int)*var.resources_amount);
        //printf("Requirements:\n");
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            var.processes[i].requirements[j]=num;
            //printf( "Requirement of resource %d is %d\n", j, num );
            line += length;
        }
        //getting move_cost
        sscanf( line, "%d%n", &num, &length);
        //printf("process move cost: %d\n", num);
        var.processes[i].process_move_cost = num;
        line += length;
    }
}
void read_balance_cost(FILE *file){
    int balance_costs_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    
    getline(&line, &len, file);
    printf("balance costs: %s\n", line);
    balance_costs_amount = atoi(line);
    var.balance_costs_amount = balance_costs_amount;
    
    var.balance_costs = malloc(sizeof(struct balance_cost)*balance_costs_amount);
    
    for(i=0; i<balance_costs_amount; i++){
        //printf("balance cost %d: \n", i);
        //getting balance cost
        getline(&line, &len, file);
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            if(j==0){
                //printf("resource one: %d\n", num);
                var.balance_costs[i].resource_one = num;
            }
            else if(j==1){
                //printf("resource two: %d\n", num);
                var.balance_costs[i].resource_two = num;
            }
            else if(j==2){
                //printf("target: %d\n", num);
                var.balance_costs[i].target = num;
            }
            line += length;
        }
        //getting weight
        getline(&line, &len, file);
        //printf("weight: %s\n", line);
        var.balance_costs[i].weight = atoi(line);
    }
}
void read_costs(FILE *file){
    int num, length;
    char * line = NULL;
    size_t len = 0;
    
    getline(&line, &len, file);
    
    sscanf( line, "%d%n", &num, &length);
    //getline(&line, &len, file);
    printf("process move cost: %d\n", num);
    var.process_move_cost = num;
    line += length;
    
    sscanf( line, "%d%n", &num, &length);
    //getline(&line, &len, file);
    printf("service move cost: %d\n", num);
    var.service_move_cost = num;
    line += length;
    
    sscanf( line, "%d%n", &num, &length);
    //getline(&line, &len, file);
    printf("machine move cost: %d\n", num);
    var.machine_move_cost = num;
    //line += length;
}

void read_file(char *filename){
    FILE *file;
    file = fopen(filename, "r");
    
    if ( file == 0 ){
        printf( "Could not open Settings File\n" );
    }
    else{
        read_resources(file);
        read_machines(file);
        read_services(file);
        read_processes(file);
        read_balance_cost(file);
        read_costs(file);
    }
}
void read_initial_solution(char *filename){
    FILE *file;
    file = fopen(filename, "r");
    int j, num, length;
    char * line = NULL;
    size_t len = 0;
    
    if ( file == 0 ){
        printf( "\nCould not open Initial Solution File\n" );
    }
    else{
        sol.best_value = DBL_MAX;
        sol.value = DBL_MAX;
        sol.best_process_asignations = malloc(sizeof(int)*var.processes_amount);
        sol.process_asignations = malloc(sizeof(int)*var.processes_amount);
        sol.initial_solution = malloc(sizeof(int)*var.processes_amount);
        
        printf("\nReading initial solution\n");
        getline(&line, &len, file);
        for(j=0; j < var.processes_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            //printf("The process %d is assigned to the machine %d\n", j, num);
            sol.process_asignations[j]=num;
            sol.initial_solution[j]=num;
            line += length;
        }
        //sol.process_asignations = process_asignations;
        update_used_resources();
        evaluate_and_verify_solution();
        printf("Ended reading initial solution\n\n");
    }
}
void write_solution(char *filename){}

int max(int value1, int value2){
    return value1 > value2? value1 : value2;
}
void update_used_resources(){
    int i, j, k, sum;
    
    for(i=0; i< var.machines_amount; i++){
        for(j=0; j< var.resources_amount; j++){
            for(k=0; k < var.processes_amount; k++){
                if(sol.process_asignations[k] == i){ sum += var.processes[k].requirements[j]; }
                else if(sol.initial_solution[k] == i && var.resources[k].transient == 1){ sum += var.processes[k].requirements[j]; }
            }
            var.machines[i].resources_used[j] = sum;
        }
    }
}
int available_resources(int machine, int resource){
    return var.machines[machine].capacities[resource]-var.machines[machine].resources_used[resource];
}
struct possible_moves resizeOptions(struct possible_moves moves) {
    int i,j;
    
    //printf("Size of options before realloc %d\n", length); 
    //printf("\nMoves Length: %d\n", moves.length);
    int resta = 0;
    for(i = 0; i < moves.length; i++){
        //printf("Moves %d value: %d, ", i, moves.possible_moves[i]);
        if(moves.possible_moves[i] == -1){
            resta++;
        }
    }
    //printf("\n");
    
    for(i = 0; i < moves.length; i++){
        for(j = i + 1; j < moves.length; j++){
            if(moves.possible_moves[j] > moves.possible_moves[i]){
                int aux1 = moves.possible_moves[i];
                int aux2 = moves.possible_moves[j];
                moves.possible_moves[i] = aux2;
                moves.possible_moves[j] = aux1;
            }
        }
    }
    moves.length = moves.length - resta;
    
    //printf("length of moves array: %d\n", moves.length);
    if(moves.length > 0){
        int *tmp = realloc(moves.possible_moves, moves.length * sizeof(int));
        if (tmp == NULL){
            printf("Fail realloc");
        }else{
            *(moves.possible_moves) = *tmp;
        }
    }
    
    return moves;
    //printf("Size of options after realloc %lu\n", sizeof(&options)/sizeof(int));
}

double evaluate_solution(){
    double pmc = 0, smc = 0, mmc = 0, lc = 0, bc = 0;
    int i, j;
    //process move costs
    for(i=0; i < var.processes_amount; i++){
        if(sol.initial_solution[i] != sol.process_asignations[i]){
            pmc += var.processes[i].process_move_cost;
        }
    }
    //service move costs
    int aux_smc = 0;
    for(i=0; i < var.services_amount; i++){
        aux_smc = 0;
        for(j=0; j < var.processes_amount; j++){
            if(var.processes[j].service == i && sol.initial_solution[j] != sol.process_asignations[j]){
                aux_smc++;
            }
        }
        if(aux_smc > smc) smc = aux_smc;
    }
    //machine move costs
    for(i=0; i < var.processes_amount; i++){
        if(sol.initial_solution[i] != sol.process_asignations[i]){
            mmc += var.machines[sol.initial_solution[i]].machine_move_costs[sol.process_asignations[i]];
        }
    }
    //load costs
    for(i=0; i < var.machines_amount; i++){
        for(j=0; j < var.resources_amount; j++){
            //printf("ma: %d, ra: %d, wlc: %d, sc: %d\n", var.machines_amount, var.resources_amount, var.resources[j].weight_load_cost, var.machines[i].safety_capacities[0]);
            lc += var.resources[j].weight_load_cost*max(0, (var.machines[i].capacities[j]-var.machines[i].safety_capacities[j]));
        }
    }
    //balance costs
    int resource_one, resource_two, target, weight;
    for(i=0; i < var.balance_costs_amount; i++){
        for(j=0; j < var.machines_amount; j++){
            resource_one = var.balance_costs[i].resource_one;
            resource_two = var.balance_costs[i].resource_two;
            target = var.balance_costs[i].target;
            weight = var.balance_costs[i].weight;
            bc += weight*max(0,target*available_resources(j, resource_two)-available_resources(j, resource_one));
        }
    }
    sol.value = var.process_move_cost*pmc+var.service_move_cost*smc+var.machine_move_cost*mmc+lc+bc;
    
    printf("Actual Solution: %f ", sol.value);
    print_solution(sol.process_asignations);
    return var.process_move_cost*pmc+var.service_move_cost*smc+var.machine_move_cost*mmc+lc+bc;
}
void verify_best_solution(){
    int i;
    //printf("Verifying...\n");
    if(sol.value < sol.best_value){
        //printf("entered");
        sol.best_value = sol.value;
        //printf("Better solution found!\nCost: %lf", sol.best_value);
        for(i=0; i < var.processes_amount; i++){
            sol.best_process_asignations[i] = sol.process_asignations[i];
        }
        printf("Best solution value: %lf\n", sol.value);
        //printf("almost done");
    }
    //printf("End of Verify\n");
}
void evaluate_and_verify_solution(){
    //printf("start evaluating");
    evaluate_solution();
    //printf("end evaluating and starting verify");
    verify_best_solution();
}

void print_solution(int* sol){
    int i;
    for(i=0; i < var.processes_amount; i++){
		printf(" %d", sol[i]);
	}
	printf("\n");
}
void print_solution_to_file(int* sol){
    FILE *file;
    file = fopen("final_solution.txt", "w");
    int i;
    fprintf(file, "%d", sol[0]);
    for(i=1; i < var.processes_amount; i++){
		fprintf(file, " %d", sol[i]);
	}
}

struct possible_moves get_possible_moves(int process_number){
    struct possible_moves moves;
    moves.possible_moves = malloc(var.machines_amount*sizeof(int));
    moves.length = var.machines_amount;
    int i, j, k, l;
    
    //create initial array
    for(i=0; i < moves.length; i++){
        moves.possible_moves[i] = i;
    }
    
    //filtering by capacity : revisado
    for(i=0; i < moves.length; i++){
        for(j=0; j < var.resources_amount; j++){
			if(available_resources(moves.possible_moves[i],j) < var.processes[process_number].requirements[j]){
				moves.possible_moves[i] = -1;
				break;
			}
        }
    }
    moves = resizeOptions(moves);
    printf("Filtered by capacity\n");
    //return moves;
    
    //filtering by service constraint
    int service = var.processes[process_number].service;
    for(i = 0; i < process_number; i++){
		for(j = 0; j < moves.length; j++){
			if(i != process_number && var.processes[i].service == service && sol.process_asignations[i] == moves.possible_moves[j]){
				moves.possible_moves[j] = -1;
			}
		}
    }
    moves = resizeOptions(moves);
    printf("Filtered by service\n");
    
    //filtering by dependency
    //revisar todas las maquinas en el vecindario que cuenten con mis dependencias y que no queden dependientes de mi no cubiertos
    int* dependencies = var.services[service].dependent_on_service;
    int dependencies_amount = var.services[service].amount_of_dependencies;
    int remaining_dependencies = dependencies_amount;
	for(i=0; i< moves.length && remaining_dependencies > 0; i++){
		int neighborhood = var.machines[moves.possible_moves[i]].neighborhood;
		for(j=0; j < var.machines_amount && remaining_dependencies == 0; j++){
			if( var.machines[j].neighborhood == neighborhood ){
				for(k = 0; k < var.processes_amount && remaining_dependencies > 0; k++){
					if(sol.process_asignations[k] == j){
						for(l = 0; l < dependencies_amount || remaining_dependencies > 0; l++){
							if(dependencies[l] == var.processes[k].service){
								dependencies[l] == -1;
								remaining_dependencies--;
								if(remaining_dependencies == 0){
									//todas las depepndencias estan cubiertas :D
								}
							}
						}
					}
				}
			}
		}
	}
	moves = resizeOptions(moves);
	printf("Filtered by dependencies\n");
	
	//en el peor caso, habrá como máximo una location por maquina
	int locations_for_service[var.machines_amount], locations_amount = 0;
	for(i=0; i < var.machines_amount; i++){
		locations_for_service[i] = -1;
	}
    
    //filtering spread
    int aux = sol.process_asignations[process_number];
    for(k=0; k < moves.length; k++){
		sol.process_asignations[process_number] = moves.possible_moves[k];
		for(i=0; i < var.processes_amount && locations_amount < var.services[service].spread_min; i++){
			if(var.processes[i].service == service){
				int location = var.machines[sol.process_asignations[i]].location;
				for(j=0; j < var.machines_amount && locations_amount < var.services[service].spread_min; j++){
					if(locations_for_service[j] == location) break;
					else if(locations_for_service[j] == -1){
						locations_for_service[j] = location;
						locations_amount++;
					}
				}
			}
		}
		if(locations_amount < var.services[service].spread_min){
			moves.possible_moves[k] = -1;
		}
		moves = resizeOptions(moves);
	}
	sol.process_asignations[process_number] = aux;
	printf("Filtered by spread\n");
	
    
    moves = resizeOptions(moves);
    return moves;
}

void pick_best_option(struct possible_moves moves, int process){
    if(moves.length){
        int r, machine, i;
        double min_value = -1, result;
        for(i=0; i < moves.length; i++){
            r = rand() % moves.length;
            sol.process_asignations[process] = moves.possible_moves[r];
            update_used_resources();
            result = evaluate_solution(sol);
            if(min_value > result || min_value == -1){
                min_value = result;
                machine = moves.possible_moves[r];
            }
        }
        sol.process_asignations[process] = moves.possible_moves[machine];
        update_used_resources();
        evaluate_and_verify_solution(sol);
    }
}
void pick_option(struct possible_moves moves, int process){
    if(moves.length){
        int r = rand() % moves.length;
        //printf("random value: %d\n\n", r);
        //r = Max + rand() / (RAND_MAX / (0 - Max + 1) + 1) - 1;
        //return r; 
        sol.process_asignations[process] = moves.possible_moves[r];
        update_used_resources();
        evaluate_and_verify_solution(sol);
    }
}
void greedy(){
    //guardar la suma de recursos que necesitan todos los procesos
    int i,j, proccesses_resources[var.processes_amount][2];
    
    for(i=0; i < var.processes_amount; i++){
        proccesses_resources[i][0] = i;
        for(j=0; j < var.resources_amount; j++){
            proccesses_resources[i][1] += var.processes[i].requirements[j];
        }
    }
    //ordenar todos los recursos deacuerdo con la suma de recursos que necesitan
    //usando burbuja para ordenar a estas alturas...really? xD
    for(i=0; i < var.processes_amount; i++){
        for(j = i + 1; j < var.processes_amount; j++){
            if(proccesses_resources[j][1] > proccesses_resources[i][1]){
                int *aux = proccesses_resources[i];
                *proccesses_resources[i] = *proccesses_resources[j];
                *proccesses_resources[j] = *aux;
            }
        }
    }
    
    //asignandole a las máquinas solo los recursos transientes
    for(i=0; i < var.resources_amount; i++){
        for(j=0; j < var.machines_amount; j++){
            var.machines[j].resources_used[i] = 0;
        }
        if(var.resources[i].transient == 1){
            for(j=0; j < var.processes_amount; j++){
                var.machines[sol.initial_solution[j]].resources_used[i] += var.processes[j].requirements[i];
            }
        }
    }
    
    //asignar los procesos
    for(i=0; i < var.processes_amount; i++){
        struct possible_moves moves = get_possible_moves(i);
        
        /*printf("Possible Moves start\n");
        for(j=0; j< moves.length; j++){
            printf("Possible Moves value on %d position: %d\n", j, moves.possible_moves[j]);
        }
        printf("Possible Moves end\n\n");
        printf("possible_moves_amount: %d\n\n", moves.length);*/
        pick_best_option(moves, i);
        //free(possible_moves);
    }
}

void initialize_tabu_list(int tabu_list_size){
    tabu.list = malloc(tabu_list_size*sizeof(struct tabu_item));
    tabu.length = tabu_list_size;
    int i;
    struct tabu_item item;
    item.process = -1;
    item.machine = -1;
    for(i=0; i < tabu.length; i++){
        tabu.list[i] = item;
    }
}
int is_tabu(int process, int machine){
    int i;
    for(i = 0; i < tabu.length; i++){
        if(tabu.list[i].process == process && tabu.list[i].machine == machine){ return 1; }
    }
    return 0;
}
int is_valid(int process, int machine){
	sol.process_asignations[process] = machine;
	update_used_resources();
	//capacities
	
	//servicios
	int service = var.processes[process].service, i;
	for(i=0; i<var.processes_amount; i++){
		if(i != process && sol.process_asignations[i] == machine && var.processes[i].service == service) return 0;
	}
	//spread min
	//service dependencies
	//transient
	
	
	return 1;
}
void add_tabu(int process, int machine){
    int i;
    for(i=1; i < tabu.length; i++){
        tabu.list[i-1] = tabu.list[i];
    }
    struct tabu_item item;
    item.process = process;
    item.machine = machine;
    tabu.list[tabu.length-1] = item;
}
void tabu_search(){
    //printf("Entro a TS!");
    int process, machine;
    //elegir un proceso random
    
    while(((float)(clock() - tiempo.start))/CLOCKS_PER_SEC * 100 < tiempo.limit){
		//printf("tiempo pasado: %f, tiempo limite: %d\n", ((float)(clock() - tiempo.start))/CLOCKS_PER_SEC * 100, tiempo.limit); 
    //for(i=0; i < 2; i++){
        
        process = rand() % var.processes_amount;
        machine = rand() % var.machines_amount;
        
        //printf("TS while start\n");
        while(is_tabu(process, machine) || !is_valid(process, machine)){
			//printf("tabu: %d, valid: %d\n", is_tabu(process, machine), is_valid(process, machine));
            process = rand() % var.processes_amount;
			machine = rand() % var.machines_amount;    
			//printf("TS while loop\n");
        }
        //printf("TS while end\n");
        
        sol.process_asignations[process] = machine;
        update_used_resources();
        evaluate_and_verify_solution();
        add_tabu(process, machine);
    }
}

int main( int argc, char *argv[] ){
    tiempo.start = clock();
    tiempo.limit = strtol(argv[2], NULL, 10);
    
    srand(strtol(argv[10], NULL, 10));
    read_file(argv[4]);
    read_initial_solution(argv[6]);
    //printf("End reading solution;\n");
    //hasta aca va todo bien
    evaluate_and_verify_solution();
    initialize_tabu_list(2);
    
        //falta que se filtre mejor las opciones 
        //y que el greedy elija las mejores opciones
    //printf("entrando a Greedy");
    greedy();
    printf("\nGreedy Ended\n");
    tabu_search();
    printf("TS Ended\n");
    
    //print solution
    printf("Best Solution: %f ", sol.best_value);
    print_solution(sol.best_process_asignations);
    print_solution_to_file(sol.best_process_asignations);
    
    return 0;
}
