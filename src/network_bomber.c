#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "network_manager.h"

#define TAILLE_MESSAGE 255
#define PORT           6666
#define IP             "10.6.7."

int*       g_sockets;
pthread_t* g_threads;

void(*g_pointer_handler)(void* p_ip);

int g_lock = 0;

int g_port = 0;

int g_trace = 0;

int g_port_min = 0;
int g_port_max = 100000;

int g_sleep    = 5;

int g_count_ip = 1;

char g_ip     [15];
char g_ip_min [15];
char g_ip_max [15];

char g_type   [15];

void* g_f;

void handler_udp(void* p_ip)
{
  int d_socket;

  //int a = *((int *) p_ip);
  int a = 12;
  struct sockaddr_in sockaddr_in_client;

  char buffer[TAILLE_MESSAGE];

  d_socket = socket(AF_INET, SOCK_DGRAM , 0);

  if(d_socket == -1)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  char buff_int[2];

  sprintf(buff_int, "%d", a);

  while(1)
  {
    sleep(g_sleep);

    for(int j = g_port_min; j <= g_port_max; j++)
    {
      if(g_trace == 1)
        fprintf(stdout, "%s : %d\n", g_ip, j);

      sockaddr_in_client.sin_addr.s_addr = inet_addr(g_ip);
      sockaddr_in_client.sin_family      = AF_INET;
      sockaddr_in_client.sin_port        = htons(j);

      if(d_socket == -1)
      {
        //perror("socket ");
        //exit(EXIT_FAILURE);
      }

      strcpy(buffer, "\033cYolo.\n");

      sendto(d_socket, buffer, strlen(buffer)
      , 0,
      (struct sockaddr*) &sockaddr_in_client, sizeof sockaddr_in_client);
    }
  }
}

void handler_tcp(void* ip)
{
  int d_socket;

  //int a = *((int *) p_ip);
  int a = 12;
  struct sockaddr_in sockaddr_in_client;

  char buffer[TAILLE_MESSAGE];

  d_socket = socket(AF_INET, SOCK_STREAM , 0);

  if(d_socket == -1)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  char buff_int[2];

  sprintf(buff_int, "%d", a);

  while(1)
  {
      sleep(g_sleep);

      for(int j = g_port_min; j <= g_port_max; j++)
      {
        sockaddr_in_client.sin_addr.s_addr = inet_addr(g_ip);
        sockaddr_in_client.sin_family      = AF_INET;
        sockaddr_in_client.sin_port        = htons(j);

        if(g_trace == 1)
          fprintf(stdout, "%s : %d\n", g_ip, j);

        if(connect(d_socket , (struct sockaddr*) &sockaddr_in_client , sizeof sockaddr_in_client) == 0)
        {
          if(d_socket == -1)
          {
            //perror("socket ");
            //exit(EXIT_FAILURE);
          }

          strcpy(buffer, "\033cYolo.\n");

          sendto(d_socket, buffer, strlen(buffer)
          , 0,
          (struct sockaddr*) &sockaddr_in_client, sizeof sockaddr_in_client);
        }
        else
          puts("Fail");
      }
    }

}

void foo()
{
  g_threads = malloc(30 * sizeof(pthread_t));

  if(g_count_ip == 1)
    g_pointer_handler((void*) 1);
  else
    for(int index = 0; index < g_count_ip; index++)
    {
      int* i = malloc(1 * sizeof(int));
      pthread_create(&g_threads[index], NULL , handler_udp, (void*) i);

      pthread_join(g_threads[index], NULL);
    }
}

void init(int argc, char** argv)
{
  int c;
  int digit_optind = 0;
  int aopt = 0, bopt = 0;
  char *copt = 0, *dopt = 0;

  struct option long_options[] =
  {
    {"trace", 0, 0, 0},
    {"ports", 0, 0, 0},
    {"range", 0, 0, 0},
    {"help", 0, 0, 0},
    {"lock", 0, 0, 0},
    {"port_min", 1, 0, 0},
    {"port_max", 1, 0, 0},
    {NULL, 0, NULL, 0}
  };

  int option_index = 0;
  char* buffer;

  while ((c = getopt_long(argc, argv, "t:i:s:",
  long_options, &option_index)) != -1)
  {
    int this_option_optind = optind ? optind : 1;
    switch(c)
    {
      case 0:
        if(strcmp("trace", long_options[option_index].name) == 0)
          g_trace = 1;
        else if(strcmp("port_min", long_options[option_index].name) == 0)
          g_port_min = atoi(optarg);
        else if(strcmp("port_max", long_options[option_index].name) == 0)
          g_port_max = atoi(optarg);
        else if(strcmp("help", long_options[option_index].name) == 0)
          help();
        else if(strcmp("lock", long_options[option_index].name) == 0)
          g_lock = 1;
        else
        {

        }
      break;

      case 'i':
        strcpy(g_ip, optarg);
        g_count_ip = 1;
      break;

      case 's':
        g_sleep = atoi(optarg);
      break;

      case 't':
        strcpy(g_type, optarg);
        if(strcmp(g_type, "udp") == 0)
          g_pointer_handler = handler_udp;
        else
          g_pointer_handler = handler_tcp;
      break;
    }
  }
}

void help()
{

}

int main(int argc , char** argv)
{
  init(argc, argv);
  foo();
}
