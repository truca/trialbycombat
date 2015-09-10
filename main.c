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
struct tabu_list{
    int* list;
    int length;
};
struct tabu_list tabu;
struct time_container{
    int limit;
    clock_t start;
};
struct time_container tiempo;

void read_resources(FILE *file){
    int resources_amount, i, num, length;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
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
    ssize_t read;
    
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
        var.machines[i].capacities = malloc(sizeof(int)*var.resources_amount);
        var.machines[i].resources_used = malloc(sizeof(int)*var.resources_amount);
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
    ssize_t read;
    
    getline(&line, &len, file);
    printf("services: %s", line);
    services_amount = atoi(line);
    var.services_amount = services_amount;
    
    var.services = malloc(sizeof(struct service)*services_amount);
    
    for(i=0; i < services_amount; i++){
        //getline(&line, &len, file);
        getline(&line, &len, file);
        var.services[i].spread_min = atoi(line);
        //printf("spread_min: %s \n", line);
        getline(&line, &len, file);
        //printf("dependencies: %s \n", line);
        var.services[i].amount_of_dependencies = atoi(line);
        var.services[i].dependent_on_service = malloc(sizeof(int)*var.services[i].amount_of_dependencies);
        for(j=0; j < var.services[i].amount_of_dependencies; j++){
            getline(&line, &len, file);
            //printf("dependency %d: %s \n", j, line);
            var.services[i].dependent_on_service[j] = atoi(line);
        }
    }
}
void read_processes(FILE *file){
    int processes_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
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
        getline(&line, &len, file);
        //printf("Belongs to service: %s\n", line);
        var.processes[i].service = atoi(line);
        //getting requirements
        getline(&line, &len, file);
        var.processes[i].requirements = malloc(sizeof(int)*var.resources_amount);
        //printf("Requirements:\n");
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            var.processes[i].requirements[j]=num;
            //printf( "Requirement of resource %d is %d\n", j, num );
            line += length;
        }
        //getting move_cost
        getline(&line, &len, file);
        //printf("process move cost: %s\n", line);
        var.processes[i].process_move_cost = atoi(line);
    }
}
void read_balance_cost(FILE *file){
    int balance_costs_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
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
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    getline(&line, &len, file);
    printf("process move cost: %s", line);
    var.process_move_cost = atoi(line);
    
    getline(&line, &len, file);
    printf("service move cost: %s", line);
    var.service_move_cost = atoi(line);
    
    getline(&line, &len, file);
    printf("machine move cost: %s", line);
    var.machine_move_cost = atoi(line);
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
    ssize_t read;
    
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
            printf("The process %d is assigned to the machine %d\n", j, num);
            sol.process_asignations[j]=num;
            sol.initial_solution[j]=num;
            line += length;
        }
        //sol.process_asignations = process_asignations;
        printf("Ended reading initial solution\n\n");
    }
}
void write_solution(char *filename){}

int max(int value1, int value2){
    value1 > value2? value1 : value2;
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
/*int solution_qualifies(struct solution solution){
    int i,j, k;
    //verify capacity constraints
    for(i=0; i < var.machines_amount; i++){
        //printf("maquina %d\n",i);
        for(j=0; j < var.resources_amount; j++){
            int capacity_of_resource_j = var.machines[i].capacities[j];
            int requirements_of_resource_j = 0;
            for(k=0; k < var.processes_amount; k++){
                if(solution.process_asignations[k]==i){
                    requirements_of_resource_j += var.processes[k].requirements[j];
                }    
            }
            if(requirements_of_resource_j > capacity_of_resource_j){
                printf("No hay suficiente capacidad del recurso %d en la máquina %d\n", j, i);
                return 0;  
            } 
        }
    }
    printf("capacity constraint approved");
    //verify conflict constraints
    int processes_same_service_per_machine[var.services_amount][var.machines_amount];
    int service, machine;
    for(i=0; i < var.processes_amount; i++){
        service = var.processes[i].service;
        machine = solution.process_asignations[i];
        processes_same_service_per_machine[service][machine]++;
        if(processes_same_service_per_machine[service][machine] > 1 ){
            printf("There are 2 processes of service %d in the machine %d", service, machine);
        }
    }
    
    //verify services constraints
    for(i=0; i<var.services_amount; i++){
        int spread_min = var.services[i].spread_min;
        int spread_location[var.machines_amount]; //worst case scenario: each machine belongs to a different location
        int spread_amount = 0;
        for(j=0; j < var.processes_amount; j++ ){
            if(solution.process_asignations[j] == i){
                int location = var.machines[solution.process_asignations[j]].location;
                for(k=0; k < spread_amount; k++){
                    if(location == spread_location[k]) break;
                }
                if(k == spread_amount){
                    spread_amount++;
                }
            }
        }
        if(spread_amount < spread_min ){
            printf("El servicio %d no cumple con spreadmin, spread: %d, spread_min: %d", i, spread_amount, spread_min);
            return 0;
        }
    }
    printf("spread constraint approved");
    
    //verify dependency constraints
    
    //verify transient constraints
    
    return 1;
}*/
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

double evaluate_solution(struct solution sol){
    double pmc = 0, smc = 0, mmc = 0, lc = 0, bc = 0;
    int i, j, k;
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
    printf("New solution value: %lf\n", sol.value);
    return var.process_move_cost*pmc+var.service_move_cost*smc+var.machine_move_cost*mmc+lc+bc;
}
void verify_best_solution(struct solution solution){
    int i;
    //printf("Verifying...\n");
    if(solution.value < sol.best_value){
        //printf("entered");
        sol.best_value = solution.value;
        //printf("Better solution found!\nCost: %lf", sol.best_value);
        for(i=0; i < var.processes_amount; i++){
            sol.best_process_asignations[i] = solution.process_asignations[i];
        }
        //printf("almost done");
    }
    //printf("End of Verify\n");
}
void evaluate_and_verify_solution(struct solution sol){
    evaluate_solution(sol);
    verify_best_solution(sol);
}

void print_solution(){
    int i;
    printf("Greedy Proccess Asignations\n");
    for(i=0; i<var.processes_amount; i++){
        printf("Process %d assigned to machine %d\n", i, sol.process_asignations[i]);
    }
    printf("End of Proccess Asignations\n\n");
}
struct possible_moves get_possible_moves(int process_number){
    struct possible_moves moves;
    moves.possible_moves = malloc(var.machines_amount*sizeof(int));
    moves.length = var.machines_amount;
    int i, j, k;
    
    //create initial array
    for(i=0; i < moves.length; i++){
        moves.possible_moves[i] = i;
    }
    
    //filtering by capacity
    for(i=0; i < var.machines_amount; i++){
        for(j=0; j < var.processes_amount; j++){
            if(available_resources(i,j) < var.processes[process_number].requirements[j]){
                moves.possible_moves[i] = -1;
                break;
            }
        }
    }
    moves = resizeOptions(moves);
    return moves;
    
    //filtering by service constraint
    /*int service = var.processes[process_number].service;
    for(i=0; i < process_number; i++){
        if(i != process_number){
            moves.possible_moves[sol.process_asignations[i]] = -1;
        }
    }
    moves = resizeOptions(moves);
    
    //filtering by dependency
    //revisar todas las maquinas que cuenten con mis dependencias y que no queden dependientes de mi no cubiertos
    service = var.processes[process_number].service;
    for(i=0; i < var.machines_amount; i++){
        
        
    }
    //elegir una maquina, sacar su ubicacion, obtener todas 
    
    
    //filtering spread
    
    moves = resizeOptions(moves);
    return moves;*/
}

void pick_best_option(struct possible_moves moves, int process){
    if(moves.length){
        int aux = sol.process_asignations[process], r, machine, i;
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
    int i,j,k, proccesses_resources[var.processes_amount][2];
    
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
    tabu.list = malloc(tabu_list_size*sizeof(int));
    tabu.length = tabu_list_size;
    int i;
    for(i=0; i < tabu.length; i++){
        tabu.list[i] = -1;
    }
}
int is_tabu(int process){
    int i;
    for(i = 0; i < tabu.length; i++){
        if(tabu.list[i] == process){ return 1; }
    }
    return 0;
}
int add_tabu(int process){
    int i;
    for(i=1; i < tabu.length; i++){
        tabu.list[i-1] = tabu.list[i];
    }
    tabu.list[tabu.length-1] = process;
}
void tabu_search(){
    //printf("Entro a TS!");
    int i, process;
    struct possible_moves moves;
    //elegir un proceso random
    
    //while(((float)(clock() - tiempo.start))/CLOCKS_PER_SEC < tiempo.limit){
    //for(i=0; i < 2; i++){
        
        process = rand() % var.processes_amount;
        while(is_tabu(process)){
            process = rand() % var.processes_amount;    
        }
        moves = get_possible_moves(process);
        
        pick_option(moves, process);
        add_tabu(process);
        free(moves.possible_moves);
    //}
}




int main( int argc, char *argv[] ){
    tiempo.start = clock();
    tiempo.limit = strtol(argv[2], NULL, 10);
    
    srand(strtol(argv[10], NULL, 10));
    read_file(argv[4]);
    read_initial_solution(argv[6]);
    evaluate_and_verify_solution(sol);
    initialize_tabu_list(1);
    //hasta aca va todo bien
        //falta que se filtre mejor las opciones 
        //y que el greedy elija las mejores opciones
    //printf("entrando a Greedy");
    greedy();
    //printf("Sali de Greedy!");
    tabu_search();
    
    //print solution
    print_solution();
    
    return 0;
}