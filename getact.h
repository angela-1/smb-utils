/* Declarations for getact. */


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
