#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct container{
    struct item *items;  
};
struct container cont;

struct item{
    int* prices;
};


create_items(int items_size, int prices_size){
    int i;
    
    cont.items = malloc(sizeof(struct item)*items_size);
    for(i=0; i < items_size; i++){
        struct item it;
        it.prices = malloc(sizeof(int)*prices_size);
        cont.items[i] = it;
    }
}

int main( int argc, char *argv[] ){
    
    clock_t t;
    int f;
    t = clock();
    //printf ("Calculating...\n");
    //f = frequency_of_primes (99999);
    //printf ("The number of primes lower than 100,000 is: %d\n",f);
    while(((float)(clock() - t))/CLOCKS_PER_SEC < 1){
        printf("Ha pasado: %f\n", ((float)(clock() - t))/CLOCKS_PER_SEC);
    }
    printf("Ended");
    //printf ("It took me %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
    return 0;
    /*create_items( 3, 3);
    cont.items[0].prices[0] = 4;
    
    printf("worked: %d\n", cont.items[0].prices[0]);*/
    
    /*struct container cont;
    struct item items[2];
    
    struct item it, it2;
    int sizes[2];
    sizes[0] = 1;
    sizes[1] = 2;
    it.sizes = sizes;
    
    int sizes2[2];
    sizes2[0] = 4;
    sizes2[1] = 5;
    it2.sizes = sizes2;
    
    items[0] = it;
    items[1] = it2;
    
    cont.items = items;
    
    printf("%d\n", cont.items[0].sizes[0]);*/
    
    /*int *ip;
	int a[10];
	a[2]=3;
	
	struct perro p;
	p.gato = a;
	int i = p.gato[2];
	printf("%d\n", i);*/
	
	/*ip = a;
	*(ip+1) = 3;
    int i = ip[1];
    printf("%d\n", i);*/
    
    /*struct perro p;
    int perro[3];
    perro[0]=0;
    perro[1]=1;
    perro[2]=2;
    p.gato = &perro;
    
    
    int gato[3];
    &gato = (p.gato);
    
    gato[2]=3;
    
    printf("%d\n", p.gato[2]);
    printf("%d\n", gato[2]);*/
    
    return 0;
}