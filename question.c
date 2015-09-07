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
struct variables{
    int resources_amount;
    int machines_amount;
    
    struct machine* machines;
    struct resource* resources;
};
struct variables var;

void read_resources(FILE *file){
    int resources_amount, i;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    getline(&line, &len, file);
    //printf("resources: %s", line);
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
    //printf("machines: %d\n", atoi(line));
    machines_amount = atoi(line);
    
    struct machine machines[machines_amount];
    
    printf("Printing array values:\n");
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
        
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            capacities[j]=num;
            printf("Machine %d, resource %d: %d\n", i, j, capacities[j]);
            line += length;
        }
        //HERE IS THE ERROR I GUESS!!
        machines[i].capacities = capacities;
        getline(&line, &len, file);
        int safety_capacities[var.resources_amount];
        for(j=0; j < var.resources_amount; j++){
            sscanf( line, "%d%n", &num, &length);
            safety_capacities[j]=num;
            //printf( "Number %d is %d\n", j, num );
            line += length;
        }
        machines[i].safety_capacities = safety_capacities;
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
    printf("Printing pointer values:\n");
    for(i=0; i < machines_amount; i++){
        for(j=0; j < var.resources_amount; j++){
            printf("Machine %d, resource %d: %d\n", i, j, machines[i].capacities[j]);
        }
    }
    
    var.machines = machines;
    var.machines_amount = machines_amount;
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
    }
}

int main( int argc, char *argv[] ){
    read_file(argv[4]);
    
    return 0;
}