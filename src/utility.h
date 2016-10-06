#ifndef UTILITY_H
#define UTILITY_H

#include <glib.h>
#include <netinet/in.h> // struct sockaddr_include
#include <arpa/inet.h>  // intet_ntop()
#include <stdio.h>

#include "header.h"

void getIpAndPort(GString *ip, struct sockaddr_in *client);
void getURL(struct httpRequest *req, GString *url);
void extractHostFromHeaders(GString *headers, GString *host);

void addHtmlToMsgBody(GString *msgBody, GString *colour);
void parseQuery(GString *target, GString *query);
void parsePage(GString *target, GString *page);

void parseHeaders(struct httpRequest *req, GString *headers);
void getHeaderData(struct httpRequest *req, GString *header);

#endif
