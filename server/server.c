/**
 * NAME : Dao McGill
 *
 * HOMEWORK : 9
 *
 * CLASS : ICS 451
 *
 * DATE : November 18, 2023
 *
 * FILE : server.c
 *
 **/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

FILE *file;

/**
 * FUNCTION: printBoth
 * 
 * DESCRIPTION: Prints to both stdout and file.
 *
 * @param fp: file to print to
 * @param string: string to print
 * 
 * @return: void
 *
 **/
/* I took this entire function from https://stackoverflow.com/questions/6420194/how-to-print-both-to-stdout-and-file-in-c */
void printBoth(FILE *fp, char *string) 
{
    printf("%s", string);
    fprintf(fp, "%s", string);
    return;
}

/**
 * FUNCTION: createSocket
 * 
 * DESCRIPTION: Creates a socket using the given parameters.
 *
 * @param ipv4: family of ip address (AF_INET)
 * @param tcp: tcp socket type (SOCK_STREAM)
 * 
 * @return: socket or -1 if error
 *
 **/
int createSocket(int ipv4, int tcp)
{
   int sock = 0;
   sock = socket(ipv4, tcp, 0);
   if (sock < 0)
   {
      printBoth(file, "ERR: socket could not be created\n");
      sock = -1;
   }
   return sock;
}

/**
 * FUNCTION: bindSocket
 * 
 * DESCRIPTION: Binds socket to parameters.
 *
 * @param sock: socket to bind
 * @param ipv4: family of ip address (AF_INET)
 * @param port: port number
 * @param allip: all available ip (INADDR_ANY)
 * 
 * @return: 0 or -1 if error
 * 
 **/
int bindSocket(int sock, int ipv4, int port, int allip)
{  
   int rv = 0;
   struct sockaddr_in servaddr;
   servaddr.sin_family = ipv4;
   servaddr.sin_port = htons(port);
   servaddr.sin_addr.s_addr = allip;
   /* the following three lines are from: https://people.cs.rutgers.edu/~pxk/rutgers/notes/sockets/ */
   if (bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) 
   {
      printBoth(file, "ERR: socket could not be bound\n");
      rv = -1;
   }
   return rv;
}

/**
 * FUNCTION: startListen
 * 
 * DESCRIPTION: Starts listening for connections.
 * 
 * @param sock: socket to listen on
 * 
 * @return: 0 or -1 if error
 * 
**/
int startListen(int sock)
{
   int rv = 0;
   if (listen(sock, 10) < 0)
   {
      printBoth(file, "ERR: listen failed\n");
      rv = -1;
   }
   return rv;
}

/**
 * FUNCTION: acceptConnection
 * 
 * DESCRIPTION: Accepts a connection.
 * 
 * @param sock: socket to accept connection on
 * 
*/
int acceptConnection(int sock)
{
   int conn = 0;
   struct sockaddr_in cliaddr;
   /* for use with ptr to socklen_t */
   socklen_t cliaddrlen = sizeof(cliaddr);
   conn = accept(sock, (struct sockaddr*)&cliaddr, &cliaddrlen);
   if (conn < 0)
      {
         printBoth(file, "ERR: connection failed\n");
         conn = -1;
      }
   return conn;
}

/**
 * FUNCTION: main
 * 
 * DESCRIPTION: Main function of server.
 * 
 * @param argc: number of arguments
 * @param argv: arg[0]: ./server, arg[1]: port number
*/
int main(int argc, char *argv[])
{
   int rv = 0;
   //arg to int
   int port = atoi(argv[1]);
   int ipv4 = AF_INET;
   int tcp = SOCK_STREAM;
   int allip = INADDR_ANY;
   int sock = 0;

   //file to write to
   file = fopen("output_server.txt", "w");
   if (file == NULL)
   {
      printBoth(file, "ERR: could not open file\n");
      rv = -1;
   }

   sock = createSocket(ipv4, tcp);
   if (sock >= 0 && rv == 0)
   {
      printBoth(file, "socket created\n");

      if (bindSocket(sock, ipv4, port, allip) >= 0 && rv == 0)
      {
         printBoth(file, "socket bound\n");

         if (startListen(sock) >= 0 && rv == 0)
         {
               printBoth(file, "listening for connections...\n");

               int conn = acceptConnection(sock);
               if (conn >= 0 && rv == 0)
               {
                  printBoth(file, "connection established\n");
                  // char message[50] = "Hello Dao. The server is working!\n";
                  // send(conn, message, strlen(message), 0);
                  // printBoth(file, "message sent\n");
                  FILE *fp = fopen("file_server.jpeg", "rb");
                  if (fp != NULL)
                  {
                     char buff[1000];
                     int read = 0;
                     printBoth(file, "sending file...\n");
                     while ((read = fread(buff, 1, sizeof(buff), fp)) > 0)
                     {
                        send(conn, buff, read, 0);
                     }
                     printBoth(file, "file sent\n");
                     fclose(fp);
                     close(conn);
                     printBoth(file, "connection to client closed\n");
                     rv = 1;
                  }
                  else
                  {
                     printBoth(file, "ERR: could not open file\n");
                     rv = -1;
                  }
               }
               else
               {
                  printBoth(file, "ERR: connection failed\n");
                  rv = -1;
               }
         }
         else
         {
               printBoth(file, "ERR: listen failed\n");
               rv = -1;
         }
      }
      else
      {
         printBoth(file, "ERR: socket could not be bound\n");
         rv = -1;
      }
   }
   else
   {
      printBoth(file, "ERR: socket could not be created\n");
      rv = -1;
   }

   /* close file */
   fclose(file);
}
