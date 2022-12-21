#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>



int8_t write_buf[1024];
int8_t read_buf[1024];

int main()
{
	FILE *fd;
	char option;
	printf("\nWelcome to the application of chr_drv");

	fd =fopen("/dev/My_Device", "w+");
	if(fd == NULL)
	{
		printf("\nUnable to open device file");
		return -1;
	}
	while(1) {
		printf("\n****please enter options***");
		printf("\n\r1)write");
		printf("\n\r2)read");
		printf("\n\r3)exit");
		scanf("%c", &option);
		printf("\nyour option: %c", option);
		switch(option)
		{
			case '1':
				printf("\nEnter the string to write");
				scanf("%s", write_buf);
				printf("\nData written: %s", write_buf);
				fwrite(write_buf, strlen(write_buf)+1, 1, fd);
				printf("Done");
				break;
			case '2':
				printf("\nReading..");
				fread(read_buf, 1024,1, fd);
				printf("Done");
				printf("Data is %s", read_buf);
				break;
			case '3':
				printf("\nClosing file");
				fclose(fd);
				exit(1);
				break;
			deafult:
				printf("\nInvalid option!");
				break;

		}
	}
	fclose(fd);
	return 0;
}

				
