/* Action definitions for samba utils. 
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


#ifndef _GETACT_H
# define _GETOPT_H 1



/* For communication from `getact' to the caller.
   When `getact' finds an option that takes an argument,
   the argument value is returned here. */

extern char *actarg;

/* Index in ARGV of the next element to be scanned.
   This is used for communication to and from the caller
   and for communication between successive calls to `getact'.

   On entry to `getact', zero means this is the first call; initialize.

   When `getact' returns -1, this is the index of the first of the
   non-option elements that the caller should itself scan.

   Otherwise, `actind' communicates from one call to the next
   how much of ARGV has been scanned so far.  */

extern int actind;


/* Describe the actions requested by the application. */

struct action
{
  const char *name;
  int has_arg;
  int val;
};


/* Names for the values of the `has_arg' field of `struct action'.  */
# define no_arg       0
# define req_arg      1
# define opt_arg      2


/* Get definitions and prototypes for functions to process the
   arguments in ARGV (ARGC of them, minus the program name) for
   options given in ACTS. */

extern int getact (int __argc, char *const *__argv, const struct action *__acts);





#endif /* getact.h */
