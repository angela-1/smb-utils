#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* To deal with arguments. */
#include <getopt.h>

#include "getact.h"


/* Progname. */
#define PROGRAM_NAME "smbact"
#define VERSION "0.1"


/* The max line length. */
#define LINE_MAX 64
#define ORNAME   10


/* The password length. */
#define PASSWD_LEN 5











/* Flag to determine file input. */
int file_flag = 0;
int group_flag = 0;


/* Store the file name if gives '-f' option. */
char src_file[LINE_MAX];

/* Store the group name if not gives '-f' option. 
 * the name length is less the ORNAME. */

char group_name[ORNAME];




/* Declaration */
static void print_help (void);
static void print_version (void);


static int stdin_assemble (int _argc, char *const *_argv);
static int file_assemble (char *src_file);
static char *gen_passwd (char *passwd, int seed);

static int stdin_delete(int _argc, char *const *_argv);
static int sys_adduser (char *user_name, char *group, char *passwd);
static int sys_deluser (char *user_name);


enum
  {
    action_create = 1,
    action_read, 
    action_update, 
    action_delete
  };


  

#define ACTION_CREATE    (int) (action_create)
#define ACTION_READ      (int) (action_read)
#define ACTION_UPDATE    (int) (action_update)
#define ACTION_DELETE    (int) (action_delete)





static const struct action acts[] =
{
  { "create", opt_arg, ACTION_CREATE },
  { "read", req_arg, ACTION_READ },
  { "update", req_arg, ACTION_UPDATE },
  { "delete", req_arg, ACTION_DELETE },
  { NULL, 0 }
};



/* For getopt. */
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
   assemble the user name and password, write to output file. */
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
  
  /* Check if '-o' option exists.
     if exists, use the user specified file name.
     else use the source file name with suffix '.db'.*/
  //  const char *output_file;
  //  output_file = OUTPUT_FLAG ? DST_FILE_NAME : strcat (SRC_FILE_NAME, ".db");

  char output_file[LINE_MAX];
  sprintf (output_file, "%s%s", src_file, ".db");


  /* Open output file for write. */
  FILE *fop = NULL;
  fop = fopen (output_file, "w");

  /* Set a int to give the random seed. */
  int i = 0;
  

  char user_name[ORNAME];
  char passwd[PASSWD_LEN + 1];
  /* Use `fscanf' to read user names. 
     `fges" return a '\n', drop it. */
  while ((fscanf (fp, "%s", user_name)) != EOF)
    {
      //printf("Read a new user: %s", USERNAME);
      /* Generate a password with a seed, store it in 'PASSWD[]'. */
      gen_passwd(passwd, i++);
      //printf("Password: %s\n\n", PASSWD);
      sys_adduser (user_name, group_name, passwd);
      fprintf(fop, "%s:%s\n",user_name , passwd);
      
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



static int
stdin_assemble (int _argc, char *const *_argv)
{
  char user_name[ORNAME];
  char passwd[PASSWD_LEN + 1];
  

  FILE *fp = NULL;
  fp = fopen ("smb_act", "a+");


  puts ("user input\n");
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




static int
sys_adduser (char *user_name, char *group_name, char *passwd)
{
  char cmd[LINE_MAX];


  group_flag ?
    sprintf (cmd, "useradd -M -g %s -s /sbin/nologin %s", group_name, user_name) :
    sprintf (cmd, "useradd -M -s /sbin/nologin %s", user_name);

  printf ("Now run: %s\n", cmd);
  system (cmd);

  sprintf (cmd, "echo -e \"%s\n%s\n\" | pdbedit -a -u %s -t", passwd, passwd, user_name);
  printf ("Now run: %s\n", cmd);
  system (cmd);


  return 0;
}




/* Delete a user. From both system and samba db. */
static int
stdin_delete(int _argc, char *const *_argv)
{

  char user_name[ORNAME];
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


static int
sys_deluser (char *user_name)
{

  char cmd[LINE_MAX];
  sprintf (cmd, "userdel %s", user_name);
  system (cmd);


  sprintf (cmd, "pdbedit -x -u %s", user_name);
  system (cmd);

  return 0;



}



















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
Usage: %s [OPTION]...\n", PROGRAM_NAME);

  /* TRANSLATORS: --help output 2 (brief description)
     no-wrap */
  fputs ("\
Read a user name list, generate a user database with format:\n\
  user1:password1\n\
  user2:password2\n\
  ...\n", stdout);

  puts ("");
  /* TRANSLATORS: --help output 3: options 1/2
     no-wrap */
  fputs ("\
  -h, --help          display this help and exit\n\
  -v, --version       display version information and exit\n", stdout);

  puts ("");
  /* TRANSLATORS: --help output 4: options 2/2
     no-wrap */
  fputs ("\
  -i, --input       must, the source file name contains user list\n\
  -o, --output      specify the output file name\n", stdout);


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
Copyright (C) %s Free Software Foundation, Inc.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n",
              "2014");
}
