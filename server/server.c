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
#include <time.h>

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
* STRUCT: tcp_header
 *
* DESCRIPTION: Structure to represent the TCP header.
**/
struct TCP_header
{
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t data_offset;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urg_ptr;
};

/**
 * FUNCTION: setHeaderReceived
 * 
 * DESCRIPTION: sets header according to received data
 * 
 * @param headerArray: array of data to set header to
 * @param header: header to set
 * 
*/
void setHeaderReceived(const uint8_t *headerArray, struct TCP_header *header) {
    header->dest_port = ntohs(*((uint16_t *)headerArray));
    headerArray += 2;
    header->src_port = ntohs(*((uint16_t *)headerArray));
    headerArray += 2;
    header->seq_num = ntohl(*((uint32_t *)headerArray));
    headerArray += 4;
    header->ack_num = ntohl(*((uint32_t *)headerArray));
    headerArray += 4;
    header->data_offset = *headerArray >> 4;
    headerArray += 1;
    header->flags = *headerArray;
    headerArray += 1;
    header->window = ntohs(*((uint16_t *)headerArray));
    headerArray += 2;
    header->checksum = ntohs(*((uint16_t *)headerArray));
    headerArray += 2;
    header->urg_ptr = ntohs(*((uint16_t *)headerArray));
}

/**
 * FUNCTION: setHeaderSend
 * 
 * DESCRIPTION: sets header according to received data
 * 
 * @param headerArray: array of data to set header to
 * @param header: header to set
 * 
*/
void setHeaderSend(struct TCP_header *receivedHeader, struct TCP_header *sendHeader) {
    sendHeader->src_port = receivedHeader->dest_port;
    sendHeader->dest_port = receivedHeader->src_port;
    sendHeader->seq_num = htonl(((uint32_t)rand() << 16) | (uint32_t)rand());
    sendHeader->ack_num = htonl(ntohl(receivedHeader->seq_num)) + 20;
    sendHeader->data_offset = receivedHeader->data_offset;
    sendHeader->window = receivedHeader->window;
    sendHeader->checksum = receivedHeader->checksum;
    sendHeader->urg_ptr = receivedHeader->urg_ptr;
    sendHeader->flags = (receivedHeader->flags | (1 << 1) | (1 << 4));
}


/**
 * FUNCTION: printHeader
 * 
 * DESCRIPTION: Prints the human readable format of the TCP header.
 *
 * @param headerArray: Pointer to the byte array representing the TCP header.
 * @param size: The size of the TCP header array.
 * 
 * @return: void
 **/
void printHeader(uint8_t *array, size_t size) 
{
    if (size < 20) 
    {
        printf("Array size is too small to represent a TCP header.\n");
        return;
    }

    printf("  Source Port:      %u\n", ntohs(*((uint16_t *)array)));
    array += 2;
    printf("  Destination Port: %u\n", ntohs(*((uint16_t *)array)));
    array += 2;
    printf("  Sequence Number:  %u\n", ntohl(*((uint32_t *)array)));
    array += 4;
    printf("  Acknowledgment:   %u\n", ntohl(*((uint32_t *)array)));
    array += 4;
    uint8_t data_offset = *array >> 4;
    printf("  Data Offset:      %u\n", data_offset);
    
    uint16_t flagsField = ntohs(*((uint16_t *)array)) & 0x01FF; 
    printf("  Flags:\n");
    printf("    NS:  %u\n", (flagsField >> 8) & 1); 
    printf("    CWR: %u\n", (flagsField >> 7) & 1); 
    printf("    ECE: %u\n", (flagsField >> 6) & 1); 
    printf("    URG: %u\n", (flagsField >> 5) & 1); 
    printf("    ACK: %u\n", (flagsField >> 4) & 1); // ACK
    printf("    PSH: %u\n", (flagsField >> 3) & 1);
    printf("    RST: %u\n", (flagsField >> 2) & 1); 
    printf("    SYN: %u\n", (flagsField >> 1) & 1); // SYN
    printf("    FIN: %u\n", flagsField & 1);       
    array += 2;
    printf("  Window:           %u\n", ntohs(*((uint16_t *)array)));
    array += 2;
    printf("  Checksum:         %u\n", ntohs(*((uint16_t *)array)));
    array += 2;
    printf("  Urgent Pointer:   %u\n", ntohs(*((uint16_t *)array)));
}

/**
 * FUNCTION: printRawHeader
 * 
 * DESCRIPTION: Prints the raw bytes of the TCP header.
 *
 * @param headerArray: Pointer to the byte array representing the TCP header.
 * @param size: The size of the TCP header array.
 * 
 * @return: void
 **/
void printRawHeader(const uint8_t *headerArray, size_t size) {
    printf("Raw TCP Header Bytes:\n");
    for (size_t i = 0; i < size; ++i) {
        printf("%02X ", headerArray[i]);
        if ((i + 1) % 8 == 0) { 
            printf("\n");
        }
    }
    printf("\n");
}

/**
 * FUNCTION: headerArray
 *
 * DESCRIPTION: convert header struct to array of bytes
 *
 * @param header: TCP header struct instance
 * @param array: array of bytes
 *
 * @return: void
 **/
void headerArray(struct TCP_header *header, uint8_t *array)
{
    *((uint16_t *)array) = htons(header->dest_port);
    array += 2;
    *((uint16_t *)array) = htons(header->src_port);
    array += 2;
    *((uint32_t *)array) = htonl(header->seq_num);
    array += 4;
    *((uint32_t *)array) = htonl(header->ack_num);
    array += 4;
    *((uint8_t *)array) = header->data_offset;
    array += 1;
    *((uint8_t *)array) = header->flags;
    array += 1;
    *((uint16_t *)array) = htons(header->window);
    array += 2;
    *((uint16_t *)array) = htons(header->checksum);
    array += 2;
    *((uint16_t *)array) = htons(header->urg_ptr);
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
   srand(time(NULL));

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
                  uint8_t headArray[20] = {0};
                  ssize_t read_bytes = read(conn, headArray, sizeof(headArray));
                  if (read_bytes < 0) {
                     printBoth(file, "ERR: read failed\n");
                     rv = -1;
                  } else if (read_bytes == 0) {
                     printBoth(file, "Connection closed by client\n");
                  } else {
                     printBoth(file, "header received from client: \n");
                     struct TCP_header receivedHeader;
                     setHeaderReceived(headArray, &receivedHeader);
                     headerArray(&receivedHeader, headArray);
                     printBoth(file, "RECEIVED TCP HEADER\n");
                     printHeader(headArray, sizeof(headArray));
                     printRawHeader(headArray, sizeof(headArray));

                     struct TCP_header sendHeader;
                     setHeaderSend(&receivedHeader, &sendHeader); 
                     uint8_t sendHeaderArray[20];
                     headerArray(&sendHeader, sendHeaderArray);
                     printBoth(file, "SENT TCP HEADER\n");
                     printHeader(sendHeaderArray, sizeof(sendHeaderArray));
                     printRawHeader(sendHeaderArray, sizeof(sendHeaderArray));
                     if (send(conn, sendHeaderArray, sizeof(sendHeaderArray), 0) < 0) {
                        printBoth(file, "ERR: send failed\n");
                        rv = -1;
                     } else {
                        printBoth(file, "header sent to client\n");
                     }

                     read_bytes = read(conn, headArray, sizeof(headArray));
                     if (read_bytes < 0) {
                        printBoth(file, "ERR: read failed\n");
                        rv = -1;
                     } else if (read_bytes == 0) {
                        printBoth(file, "Connection closed by client\n");
                     } else {
                        printBoth(file, "header received from client: \n");
                        struct TCP_header receivedHeader;
                        setHeaderReceived(headArray, &receivedHeader);
                        headerArray(&receivedHeader, headArray);
                        printBoth(file, "RECEIVED TCP HEADER\n");
                        printHeader(headArray, sizeof(headArray));
                        printRawHeader(headArray, sizeof(headArray));
                     }




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
