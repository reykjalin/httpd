/* A TCP echo server.
 *
 * Receive a message on port 32000, turn it into upper case and return
 * it to the sender.
 *
 * Copyright (c) 2016, Marcel Kyas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Reykjavik University nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MARCEL
 * KYAS NOR REYKJAVIK UNIVERSITY BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h> // Get rid of printf warning

#include <sys/stat.h> // open?
#include<fcntl.h>
#include<stdlib.h>
#include <sys/time.h>
#include <arpa/inet.h> // For inet_ functions

#include "header.h"
#include "requestParser.h"
#include "responseBuilder.h"
#include "utility.h"
#include "logger.h"



void logFile(struct sockaddr_in* client, struct httpRequest* req);
void createLogString(char* message, char *time, char* ip, char* port,char* method,char* reqURL, char *code);

int main(int arg, char** argv)
{
    if(arg != 2){
        printf("%d\n", arg);
        printf("need one argument");
        exit(1);
    }

    char *temp;
    int port = strtol(argv[1], &temp, 10);
    printf("%d\n",port);

    int welcomeSocket;
    struct sockaddr_in server, client;
    char message[512];

    /* set of sockets*/
    fd_set sockets;

    /* for connection timeout*/
    /* struct timeval timeout; */

    /* Create and bind a TCP socket */
    welcomeSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* Network functions need arguments in network byte order instead of
       host byte order. The macros htonl, htons convert the values. */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    bind(welcomeSocket, (struct sockaddr *) &server, (socklen_t) sizeof(server));

    /* Before the server can accept messages, it has to listen to the
       welcome port. A backlog of one connection is allowed. */
    listen(welcomeSocket, 1);

    /*initialize the socket set */
    FD_ZERO(&sockets);

    /*add the welcome socket to the socket set */
    FD_SET(welcomeSocket, &sockets);

    for (;;) {
        /* We first have to accept a TCP connection, connfd is a fresh
           handle dedicated to this connection. */
        socklen_t len = (socklen_t) sizeof(client);
        int connectionSocket = accept(welcomeSocket, (struct sockaddr *) &client, &len);

        gboolean closeConnection = FALSE;

        while(!closeConnection){
            /* Receive from connfd, not sockfd. */
            ssize_t n;
            gint64 time = g_get_monotonic_time();
            while( (n = recv(connectionSocket, message, sizeof(message) - 1, 0)) == 0){
                if(g_get_monotonic_time() - time > 30000000){
                    printf("timeout\n");
                    break;
                }
            }
            if(n == 0)
                break;

            message[n] = '\0';

            struct httpRequest req;
            initRequestStruct(&req);
            parseRequest(message, &req);

            if(g_strstr_len(req.headers->str, req.headers->len, "Connection: ") &&
               ( g_strstr_len(req.headers->str, req.headers->len, "Connection: keep-alive") ||
                 g_strstr_len(req.headers->str, req.headers->len, "Connection: Keep-Alive"))
               ){
                closeConnection = FALSE;
            }
            else{
                closeConnection = TRUE;
            }

            struct httpResponse response;
            initHttpResponseStruct(&response);
            createResponse(&req, &response, &client, closeConnection);
            sendResponse(&response, connectionSocket);

            freeRequestStruct(&req);
            freeHttpResponseStruct(&response);
        }

        shutdown(connectionSocket, SHUT_RDWR);
        close(connectionSocket);
    }
}
