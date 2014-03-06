/* Main control flow of samba utils. 
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
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "getact.h"


/* Define progname and version number. */
#define PROGRAM_NAME "smbact"
#define VERSION "0.2"


/* Set the max line length. */
#define MAX_LEN 80


/* Group/user name should short than G_NAME_LEN length. */
#define G_NAME_LEN 10

/* Define auto generated random password length. */
#define PASSWD_LEN 5



/* Flag to determine file input. */
int file_flag = 0;
int group_flag = 0;


/* Store the file name if gives '-f' option. */
char src_file[MAX_LEN] = "";

/* Store the group name. */
char group_name[G_NAME_LEN] = "";




/* Declaration */
static void print_help (void);
static void print_version (void);


static int stdin_assemble (int _argc, char *const *_argv);
static int file_assemble (char *src_file);
static char *gen_passwd (char *passwd, int seed);

static int stdin_delete(int _argc, char *const *_argv);
static int sys_adduser (char *user_name, char *group, char *passwd);
static int sys_deluser (char *user_name);


/* Define subcommand enum. '-1' for errors. */
enum
  {
    action_create = 1,
    action_read, 
    action_update, 
    action_delete
  };


  
/* Redefine uppercase for actions. */
#define ACTION_CREATE    (int) (action_create)
#define ACTION_READ      (int) (action_read)
#define ACTION_UPDATE    (int) (action_update)
#define ACTION_DELETE    (int) (action_delete)




/* Define actions. */
static const struct action acts[] =
{
  { "create", opt_arg, ACTION_CREATE },
  { "read", req_arg, ACTION_READ },
  { "update", req_arg, ACTION_UPDATE },
  { "delete", req_arg, ACTION_DELETE },
  { NULL, 0 }
};



/* Define all options. */
static const struct option longopts[] =
{
  { "file", required_argument, NULL, 'f' },
  { "group", required_argument, NULL, 'g' },
  { "add", no_argument, NULL, 'a' },
  { "del", no_argument, NULL, 'd' },
  { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'v' },
  { NULL, 0, NULL, 0 }
};




/* Read user names from source file,
   assemble the user name and password, write to output file. 

   Source file format:
     admin:admin
     user:admin
     ...

   Admin is a group itself, and user should join his admin's group. */

static int
file_assemble (char *src_file)
{
  FILE *fp = NULL;

  /* Check if file exists. */
  if((fp = fopen(src_file, "r")) == NULL)
    {
      printf("Open file error! Check your source file.\n");
      return (EXIT_FAILURE);
    }
  
  /* Assemble output file name. */
  char output_file[MAX_LEN];
  sprintf (output_file, "%s%s", src_file, ".db");


  /* Open output file for write. */
  FILE *fop = NULL;
  fop = fopen (output_file, "w");

  /* Set a int to give the random seed, it will increase 
     when scanf a new user. */
  int i = 0;
  

  char user_name[G_NAME_LEN];
  char passwd[PASSWD_LEN + 1];

  /* Use `fscanf' to read user names. 
     `fgets" return a '\n', drop it. */
  while ((fscanf (fp, "%s:%s", user_name, group_name)) != EOF)
    {
      //printf("Read a new user: %s", USERNAME);

      /* Generate a password with a seed, store it in 'PASSWD[]'. */
      gen_passwd(passwd, i++);

      //printf("Password: %s\n\n", PASSWD);
      sys_adduser (user_name, group_name, passwd);

      /* Output result to file. */
      fprintf(fop, "%s:%s:%s\n",user_name, group_name, passwd);
      
    }

  /* All done, close file. */
  fclose (fp);
  fclose (fop);

}

/* A function generate 5 bit random password. */
static char *
gen_passwd (char *passwd, int seed)
{
  /* A random pool, string with '\0' ends. */
  char pool[] = 
    {
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
    };

  /* Set a seed for random number. */
  srand (time (NULL) + seed);

  int i;
  for (i = 0; i < PASSWD_LEN; i++)
    /* The sizeof (pool) decrease 1, pool[] is a string with '\0' ends. */
    passwd[i] = pool[rand () % (sizeof (pool) - 1)];
  
  /* Treate as string. */
  passwd[PASSWD_LEN] = '\0';
  return passwd;
}


/* Deal with user input account. */
static int
stdin_assemble (int _argc, char *const *_argv)
{
  char user_name[G_NAME_LEN];
  char passwd[PASSWD_LEN + 1];
  

  FILE *fp = NULL;
  fp = fopen ("smb_act", "a+");


  //  puts ("user input\n");
  int i = 2;
  printf ("add these users:\n");
  while (i <= _argc)
    {
      //printf ("arg %d is %s\n", i++, _argv[i]);
      strcpy (user_name, _argv[i]);
      gen_passwd(passwd, i++);
      //printf ("%s:%s\n", user_name, passwd);

      group_flag ?
	      fprintf (fp, "%s:%s:%s\n", user_name, group_name, passwd) :
	      fprintf (fp, "%s:%s:%s\n", user_name, user_name, passwd); 
      
      sys_adduser (user_name, group_name, passwd);

      
    }
  fclose (fp);
  return 0;
}



/* System call to add one user. */
static int
sys_adduser (char *user_name, char *group_name, char *passwd)
{
  char cmd[MAX_LEN];

  /* Situations for add a user:
     - file input, detect if it is a admin or user.
     - user input, with '-g' option or not. */
  (group_flag || (!strcmp (user_name, group_name))) ?
    sprintf (cmd, "useradd -M -g %s -s /sbin/nologin %s", 
	     group_name, user_name) :
    sprintf (cmd, "useradd -M -s /sbin/nologin %s", user_name);

  printf ("Now run: %s\n", cmd);
  system (cmd);

  sprintf (cmd, "echo -e \"%s\n%s\n\" | pdbedit -a -u %s -t", 
	   passwd, passwd, user_name);
  printf ("Now run: %s\n", cmd);
  system (cmd);


  return 0;
}




/* Delete a user. From both system and samba db. */
static int
stdin_delete(int _argc, char *const *_argv)
{

  char user_name[G_NAME_LEN];
  puts ("delete users from input\n");
  int i = 2;
  printf ("argc is: %d i is: %d, value: %s\n", _argc, i, _argv[i]);
  printf ("argc is: %d \n", _argc);
  printf ("del these users:\n");
  while (i <= _argc)
    {
      printf ("argc is: %d i is: %d, value: %s\n", _argc, i, _argv[i]);
      strcpy (user_name, _argv[i++]);
      
      sys_deluser (user_name);
      
    }
    return 0; 

}


/* System calls to delete a user. */
static int
sys_deluser (char *user_name)
{

  char cmd[MAX_LEN];
  sprintf (cmd, "userdel %s", user_name);
  system (cmd);


  sprintf (cmd, "pdbedit -x -u %s", user_name);
  system (cmd);

  return 0;



}


















/* Main control. */
int
main (int argc, char *argv[])
{
  int optc = 0;
  int lose = 0;

  
  while ((optc = getopt_long (argc, argv, "f:g:adhv", longopts, NULL)) != -1)
    switch (optc)
      {
      case 'v':
        print_version ();
        exit (EXIT_SUCCESS);
        break;
      case 'h':
        print_help ();
        exit (EXIT_SUCCESS);
        break;
      case 'f':
	file_flag = 1;

	strcpy (src_file, optarg);
	printf ("read a input file: %s\n", src_file);
	break;
      case 'g':
	group_flag = 1;
	strcpy (group_name, optarg);
        break;
      case 'a':
	puts ("-a");
        break;
      case 'd':
	puts ("-d");
	break;

      default:
        lose = 1;
        break;	
      }

    /* ////////////////// print argv[] for test */
    /* int i = 0; */
    /* printf ("1111111111111111111\nargc is %d\n", argc); */
    /* while (i < argc) */
    /*   printf ("arg %d is %s\n", i++, argv[i]); */

    /* printf ("optind is %d\n", optind); */
    /* ///////////////// */



    if (lose || optind == argc)
      {
	fprintf (stderr, "Try `%s --help' for more information.\n",
		 PROGRAM_NAME);
	exit (EXIT_FAILURE);
      }

    /* Shift arguments. */
    argc -= optind;
    argv += optind - 1;



    /* ////////////////// print argv[] for test */
    /* i = 0; */
    /* printf ("2222222222222222\nargc is %d\n", argc); */
    /* while (i <= argc) */
    /*   printf ("arg %d is %s\n", i++, argv[i]); */

    /* printf ("optind is %d\n", optind); */
    /* ///////////////// */



    int act = 0;
    int lost = 0;
    while ((act = getact (argc, argv, acts)) != -1)
      switch (act)
	{
	case ACTION_CREATE:

	  file_flag ? file_assemble (src_file) : stdin_assemble (argc, argv);
	  puts ("start create.");


    /* ////////////////// print argv[] for test */
    /* i = 2; */
    /* printf ("add these users:\n"); */
    /* while (i <= argc) */
    /*   printf ("arg %d is %s\n", i++, argv[i]); */


    /* ///////////////// */

	  break;
	case ACTION_READ:
	  puts ("read user.");
	  printf ("--->>>%s\n", actarg);
	  printf ("act ----> %d\n", act);
	  break;
	case ACTION_UPDATE:
	  puts ("update user.");
	  printf ("group: %s\n", group_name);
	  printf ("--->>>%s\n", actarg);
	  printf ("act ----> %d\n", act);
	  break;
	case ACTION_DELETE:


	  puts ("delete from db.");
	  printf ("user_name--->>>%s\n", actarg);
	  stdin_delete (argc, argv);
	  break;
	default:
	  lost = 1;
	  break;



	}

    /* printf ("3333333333\nact is %d\n", act); */
    /* ////////////////// print argv[] for test */
    /* i = 0; */
    /* printf ("argc is %d\n", argc); */
    /* while (i <= argc) */
    /*   printf ("arg %d is %s\n", i++, argv[i]); */

    /* ///////////////// */


    if (lost || actind < argc)
      {
	if (actind < argc)
	  fprintf (stderr, "%s: >extra operand: %s\n",
		   PROGRAM_NAME, argv[actind]);
	fprintf (stderr, "Try `%s --help' for more information.\n",
		 PROGRAM_NAME);
	exit (EXIT_FAILURE);
      }



}




/* Print help info.  This long message is split into
   several pieces to help translators be able to align different
   blocks and identify the various pieces.  */

static void
print_help (void)
{

  /* TRANSLATORS: --help output 1 (synopsis)
     no-wrap */
  printf ("\
Usage: %s ACTION [OPTION]...\n", PROGRAM_NAME);

  puts ("");
  /* TRANSLATORS: --help output 3: options 1/2
     no-wrap */
  fputs ("\
  -h, --help          display this help and exit\n\
  -v, --version       display version information and exit\n", stdout);

  puts ("");


  fputs ("\
Create a new user:\n\
  - from file\n\
    create -f file\n\
    Will generate a file to store user account.\n", stdout);

  puts ("");

  fputs ("\
  - from stdin\n\
    create [-g group] user\n\
    Will generate a \'smb_act\' file to store user account.\n", stdout);

  puts ("");


}




/* Print version and copyright information.  */

static void
print_version (void)
{
  printf ("%s %s\n", PROGRAM_NAME, VERSION);
  /* xgettext: no-wrap */
  puts ("");

  /* It is important to separate the year from the rest of the message,
     as done here, to avoid having to retranslate the message when a new
     year comes around.  */
  printf ("\
Copyright (C) %s ZHAO Peng <shui.yunduo@gmail.com>.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n",
              "2014");
}
