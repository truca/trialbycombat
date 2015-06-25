#include <stdio.h>
#include <stdlib.h>

struct machine {
    int neighborhood;
    int location;
    int* capacities;
    int* safety_capacities;
    int* moving_costs;
};
struct resource{
    int transient;
    int weight_load_cost;
};
struct service{
    int spread_min;
    int amount_of_dependencies;
    int* dependent_on_service;
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
    
    struct machine* machines;
    struct resource* resources;
    struct service* services;
    struct process* processes;
    struct balance_cost* balance_costs;
};
struct variables var;

void read_resources(FILE *file){
    int resources_amount, i;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    getline(&line, &len, file);
    printf("resources: %s", line);
    resources_amount = atoi(line);
    
    struct resource resources[resources_amount];
    
    for(i = 0; i<resources_amount; i++){
        getline(&line, &len, file);
        getline(&line, &len, file);
        //printf("transient: %s\n", line);
        resources[i].transient = atoi(line);
        getline(&line, &len, file);
        //printf("load_cost: %s\n", line);
        resources[i].weight_load_cost = atoi(line);
    }
    var.resources = resources;
    var.resources_amount = resources_amount;
}
void read_machines(FILE *file){
    int machines_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    getline(&line, &len, file);
    printf("machines: %s", line);
    machines_amount = atoi(line);
    
    struct machine machines[machines_amount];
    
    for(i=0; i<machines_amount; i++){
        //blank line
        getline(&line, &len, file);
        //getting neighborhood
        getline(&line, &len, file);
        machines[i].neighborhood = atoi(line);
        //getting location
        getline(&line, &len, file);
        machines[i].location = atoi(line);
        //getting capacities
        getline(&line, &len, file);
        int capacities[var.resources_amount];
        //printf("Capacity:\n");
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            capacities[j]=num;
            //printf( "Number %d is %d\n", j, num );
            line += length;
        }
        machines[i].capacities = capacities;
        //getting safety capacities
        getline(&line, &len, file);
        int safety_capacities[var.resources_amount];
        //printf("Safety Capacities:\n");
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            safety_capacities[j]=num;
            //printf( "Number %d is %d\n", j, num );
            line += length;
        }
        machines[i].safety_capacities = safety_capacities;
        //getting moving costs
        getline(&line, &len, file);
        int moving_costs[machines_amount];
        //printf("Moving Costs:\n");
        for(j=0; j < machines_amount; j++){
            //sscanf( line, "%d%n", &num, &length);
            moving_costs[j]=num;
            //printf( "Number %d is %d\n", j, num );
            line += length;
        }
        machines[i].moving_costs = moving_costs;
    }
    var.machines = machines;
    var.machines_amount = machines_amount;
}
void read_services(FILE *file){
    int services_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    getline(&line, &len, file);
    printf("services: %s", line);
    services_amount = atoi(line);
    
    struct service services[services_amount];
    
    for(i=0; i < services_amount; i++){
        getline(&line, &len, file);
        getline(&line, &len, file);
        services[i].spread_min = atoi(line);
        //printf("spread_min: %s \n", line);
        getline(&line, &len, file);
        //printf("dependencies: %s \n", line);
        services[i].amount_of_dependencies = atoi(line);
        int dependent_on_service[services[i].amount_of_dependencies];
        for(j=0; j < services[i].amount_of_dependencies; j++){
            getline(&line, &len, file);
            //printf("dependency %d: %s \n", j, line);
            dependent_on_service[j] = atoi(line);
        }
        services[i].dependent_on_service = dependent_on_service;
    }
    var.services = services;
    var.services_amount = services_amount;
}
void read_processes(FILE *file){
    int processes_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    getline(&line, &len, file);
    printf("processes: %s", line);
    processes_amount = atoi(line);
    
    struct process processes[processes_amount];
    
    for(i=0; i<processes_amount; i++){
        //printf("process %d: \n", i);
        //blank line
        getline(&line, &len, file);
        //getting service
        getline(&line, &len, file);
        //printf("Belongs to service: %s\n", line);
        processes[i].service = atoi(line);
        //getting requirements
        getline(&line, &len, file);
        int requirements[var.resources_amount];
        //printf("Requirements:\n");
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            requirements[j]=num;
            //printf( "Requirement of resource %d is %d\n", j, num );
            line += length;
        }
        processes[i].requirements = requirements;
        //getting move_cost
        getline(&line, &len, file);
        //printf("process move cost: %s\n", line);
        processes[i].process_move_cost = atoi(line);
        
    }
    var.processes = processes;
    var.processes_amount = processes_amount;
}
void read_balance_cost(FILE *file){
    int balance_costs_amount, i, j, num, length;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    getline(&line, &len, file);
    printf("balance costs: %s\n", line);
    balance_costs_amount = atoi(line);
    
    struct balance_cost balance_costs[balance_costs_amount];
    
    for(i=0; i<balance_costs_amount; i++){
        //printf("balance cost %d: \n", i);
        //getting balance cost
        getline(&line, &len, file);
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            if(j==0){
                //printf("resource one: %d\n", num);
                balance_costs[i].resource_one = num;
            }
            else if(j==1){
                //printf("resource two: %d\n", num);
                balance_costs[i].resource_two = num;
            }
            else if(j==2){
                //printf("target: %d\n", num);
                balance_costs[i].target = num;
            }
            line += length;
        }
        //getting weight
        getline(&line, &len, file);
        //printf("weight: %s\n", line);
        balance_costs[i].weight = atoi(line);
    }
    var.balance_costs = balance_costs;
    var.balance_costs_amount = balance_costs_amount;
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
    struct variables var;
    FILE *file;
    file = fopen(filename, "r");
    
    if ( file == 0 ){
        printf( "Could not open file\n" );
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

int main( int argc, char *argv[] ){
    read_file(argv[4]);
    //printf("%s \n", argv[4]);
    return 0;
}