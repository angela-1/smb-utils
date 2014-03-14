/* Parse the subcommand. 
   Copyright (C) 2014 ZHAO Peng <shui.yunduo@gmail.com>
   
   This file is part of Samba utils.

   Samba utils is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Samba utils is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>

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
  
  printf ("see ind: %d, __argc: %d\n", ind, __argc);


  if (ind > __argc)
    return act_code;


  while (__acts[i].name != NULL)
    {
      if (strcmp (__argv[ind], __acts[i].name) == 0)
	{
	  act_code = __acts[i].val;

	  switch (__acts[i].has_arg)
	    {
	    case no_arg:
	      break;
	    case req_arg:
	      if (ind < __argc)
		{
		  actarg = __argv[ind + 1];
		  ind += 1;
		}
	      else
		printf ("Require a arg\n");
	      break;

	    case opt_arg:
	      printf ("need a optional arg\n");
	      if (ind < __argc)
		{
		  actarg = __argv[ind + 1];
		  ind += 1;
		  printf ("find opt arg: %s.\n", actarg);
		}
	      break;
	    default:
	      break;
	    }

	}
      i++;
    }
  ind++;


  return act_code;
}
