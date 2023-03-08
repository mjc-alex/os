#include <stdio.h>
#include <dirent.h>
#include <getopt.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

struct node
{
	char name[50];
	pid_t pid;
	pid_t ppid;
}T[1000];
int num = 0;

void get_nodes(void){
	DIR *folder;
	struct dirent *entry;
	struct stat filestat;
	folder = opendir("/proc");
	if (folder == NULL)
	{
		perror("Unable to read directory");
		assert(0);
	}

	/* Read directory entries */
	while ((entry = readdir(folder)))
	{

		stat(entry->d_name, &filestat);
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		/* skip self and parent */
		int pid_int = atoi(entry->d_name);
		if (pid_int)
		{
			T[num].pid = pid_int;
			// open subdir named by number
			DIR *subfolder;
			struct dirent *subentry;
			struct stat subfilestat;
			char proc[20] = "/proc/";
			char subname[20];
			strcpy(subname, entry->d_name);
			strcat(proc, subname);
			subfolder = opendir(proc);
			if (subfolder == NULL)
			{
				perror("Unable to read subdirectory");
				assert(0);
			}

			/* Read directory entries */
			while ((subentry = readdir(subfolder)))
			{
				stat(subentry->d_name, &subfilestat);
				if (strcmp(subentry->d_name, ".") == 0 || strcmp(subentry->d_name, "..") == 0)
					continue;
				/* skip self and parent */

				if (strcmp(subentry->d_name, "stat") == 0)
				{
					strcat(proc, "/stat");
					//printf("%s\n", proc);
					FILE *fp = fopen(proc, "r");
					if (fp)
					{
						char first[50], third[50], ppid_str[50];
						fscanf(fp, "%s %s %s %s", first, T[num].name, third, ppid_str);
						T[num].ppid = atoi(ppid_str);
						fclose(fp);
					}
					else
					{
						// 错误处理
						perror("Unable to read file");
					}
				}
			}
			closedir(subfolder);
			++num;
		}
	}
	// for (pid_t i = 0; i < num; ++i)
	// {
	// 	printf("%d\t%s\t%d\n", T[i].pid, T[i].name, T[i].ppid);
	// }
	closedir(folder);
	// return (0);
}


int main(int argc, char *argv[]) {
	get_nodes();
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
	
	DIR *proc;

	int choice;
	while (1)
	{
		static struct option long_options[] =
		{
			/* Use flags like so:
			{"verbose",	no_argument,	&verbose_flag, 'V'}*/
			/* Argument styles: no_argument, required_argument, optional_argument */
			{"version", no_argument,	0,	'V'},
			{"show-pids",	no_argument,	0,	'p'},
			{"numeric-sort", no_argument, 0, 'n'},
			
			{0,0,0,0}
		};
	
		int option_index = 0;
	
		/* Argument parameters:
			no_argument: " "
			required_argument: ":"
			optional_argument: "::" */
	
		choice = getopt_long( argc, argv, "Vpn",
					long_options, &option_index);
	
		if (choice == -1)
			break;
	
		switch( choice )
		{
			case 'V':
			  printf("pstree (PSmisc) 23.4\n\
Copyright (C) 1993-2020 Werner Almesberger and Craig Small\n\n\
PSmisc comes with ABSOLUTELY NO WARRANTY.\n\
This is free software, and you are welcome to redistribute it under\n\
the terms of the GNU General Public License.\n\
For more information about these matters, see the files named COPYING.\n");	
				break;
	
			case 'n':
			  printf("Output direct children of a process sort by pid!\n");	
				break;
	
			case 'p':
				printf("Output every process's pid\n");
				break;
			case '?':
				/* getopt_long will have already printed an error */
				break;
	
			default:
				/* Not sure how to get here... */
				printf("default\n");
		}
	}
	
	if ( optind == 1){
		for (pid_t i = 0; i < num; ++i)
		{
			printf("%d\t%s\t%d\n", T[i].pid, T[i].name, T[i].ppid);
		}
	}	
	/* Deal with non-option arguments here */
	if ( optind < argc )
	{
		printf("No argument(s)!\n");		
	}
		
  assert(!argv[argc]);
  return 0;
}
