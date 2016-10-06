#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H


#include "header.h"
#include "utility.h"
#include "logger.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // send function
#include <sys/socket.h> // send function
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_ntop()

GHashTable *headerDict;

void respond();
void createResponse(struct httpRequest *request, struct httpResponse *resposne,
                    struct sockaddr_in *client, gboolean closeConnection);
void sendResponse(struct httpResponse *response, int connectionSocket);

// Init and free functions
void initHttpResponseStruct(struct httpResponse *response);
void freeHttpResponseStruct(struct httpResponse *response);

/**
 * generateHeadResponse - Generates a response to a HEAD request
 */
void generateStatusLine(struct httpResponse *resp, unsigned int statusCode);
void generateConnectionHeader(GString *headers, gboolean closeConnection);
/**
 * generateGetResponse - Generate a response to a GET request on the form
 * <!DOCTYPE html>
 * <body>
 * <p>[REQUEST_URL] [CLIENT_IP:PORT]
 * </body>
 *
 * As is stated in project description
 */
void generateGetResponse(struct httpResponse *resp, struct httpRequest *req, struct sockaddr_in *client);
void generateGetMsgBody(struct httpResponse *resp, GString *colour,
                        struct httpRequest *req, struct sockaddr_in *client);
void generatePostResponse(struct httpResponse *resp, struct httpRequest *req);


#endif
