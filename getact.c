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
	      actarg = __argv[ind + 1];

	    }
	  i++;
	}
      ind++;
    }

  return act_code;
}
