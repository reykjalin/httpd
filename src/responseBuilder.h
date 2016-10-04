#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H


#include "header.h"
#include "utility.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // send function
#include <sys/socket.h> // send function
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_ntop()

void respond();
void createResponse(struct httpRequest *request, struct httpResponse *resposne, struct sockaddr_in *client);
void sendResponse(struct httpResponse *response, int connectionSocket);

// Init and free functions
void initHttpResponseStruct(struct httpResponse *response);
void freeHttpResponseStruct(struct httpResponse *response);

/**
 * generateHeadResponse - Generates a response to a HEAD request
 */
void generateHeadResponse(struct httpResponse *resp, unsigned int statusCode);
/**
 * generateGetResponse - Generate a response to a GET request on the form
 * <!DOCTYPE html>
 * <body>
 * <p>[REQUEST_URL] [CLIENT_IP:PORT]
 * </body>
 *
 * As is stated in project description
 */
void generateGetResponse(struct httpResponse *resp,
                         struct httpRequest *req,
                         unsigned int statusCode,
                         GString *ip,
                         GString *port);


void sendFile(char *file, int sockId);

/**
 * readFile - reads the file requested by the client and stores
 * the contents of the file in the httpResponse struct 'resp'.
 * Returns 0 on success, returns 1 otherwise.
 */
int readFile(struct httpRequest *req, struct httpResponse *resp);

#endif
