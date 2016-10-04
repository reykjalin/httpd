#ifndef REQUEST_PARSER_H
#define REQUEST_PARSER_H

#include "header.h"

/**
 * parseRequest - Parses request message 'req' received from client
 * and stores information in httpRequest struct 'parsed'
 */
void parseRequest(char *req, struct httpRequest *parsed);

/**
 * initRequestStruct - Initializes the httpRequest struct 'req'
 * with empty strings, using the glib string library
 */
void initRequestStruct(struct httpRequest *req);

/**
 * freeRequestStruct - Free memory allocated by GStrings in httpRequest
 * struct 'req'
 */
void freeRequestStruct(struct httpRequest *req);

#endif
