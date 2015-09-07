/* rand example: guess the number */
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int main ()
{
  int r, i;
  int N = 0,
      M = 10;
      //RAND_MAX = 12;
  /* initialize random seed: */
  srand (time(NULL));

  /* generate secret number between 1 and 10: */
  for(i=0; i < 1000 ; i++){ 
     r = M + rand() / (RAND_MAX / (N - M + 1) + 1);
     printf("%d, ", r);
  }


  puts ("Congratulations!");
  return 0;
}