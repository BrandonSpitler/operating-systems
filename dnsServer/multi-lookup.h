#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H


#define MAXFILES 10
#define MAXRESOLVERS 10
#define MINRESOLVERS 2
#define MAXNAMELENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN
#define USAGE "<inputFilePath> <outputFilePath>"
#define MAX_RESOLVER_THREADS 10
#define BUFFERSIZE 1025
#define INPUTFS "%1024s"

void *requester(void* inputFile);

void *resolver();

#endif

