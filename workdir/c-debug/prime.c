/* prime-number finding program
Originally by Norman Matloff, UC Davis
http://wwwcsif.cs.ucdavis.edu/~davis/40/gdb_Tutorial.htm
Modified by Mark Ardis, RIT, 11/1/2006

Will report a list of all primes which are less than
or equal to the user-supplied upper bound.
WARNING: There are bugs in this program! */

#include <stdio.h>
#include <stdbool.h>

bool Prime[1500];  /* Prime[i] will be 1 if i is prime, 0 otherwise */
int UpperBound; /* check all numbers up through this one for primeness */

void CheckPrime(int K, bool Prime[]) {
  int J;

  /* the plan:  see if J divides K, for all values J which are
  themselves prime (no need to try J if it is nonprime), and
  less than or equal to sqrt(K) (if K has a divisor larger
  than this square root, it must also have a smaller one,
  so no need to check for larger ones) */
 
  J = 2;
  while (J * J <= K) {
    if (Prime[J]){
      if (K % J == 0)  {
        Prime[K] = false;
        return;
      } /* if (K % J == 0) */
    } /* if (Prime[J] == 1) */
    J++;
  } /* while (1) */

  /* if we get here, then there were no divisors of K, so it is prime */
  Prime[K] = true;

}  /* CheckPrime() */

int main() {
  int i;

  printf("Enter upper bound:\n");
  scanf("%d", &UpperBound);
  Prime[1] = true;
  Prime[2] = true;
  for (i = 3; i <= UpperBound; i += 2) {
    CheckPrime(i, Prime);
    if (Prime[i]) {
      printf("%d is a prime\n", i);
    } /* if (Prime[i]) */
  } /* for (i = 3; i <= UpperBound; i += 2) */
  return 0;
} /* main() */
