/* getact.c: read the subcommand. */

#include <stdio.h>


#include "getact.h"


int actind = 0;
char *actarg = NULL;



int
getact (int __argc, char *const *__argv, const struct action *__acts)
{
  static int ind = 1;
  int i = 0;
  int act_code = -1;

  actind = ind;
    
  if (ind <= __argc)
    {
      while (__acts[i].name != NULL)
	{
	  if (strcmp (__argv[ind], __acts[i].name) == 0)
	    {
	      act_code = __acts[i].val;
	      actarg = __argv[ind];

	    }
	  i++;
	}
      ind++;
    }

  return act_code;
}
