#ifndef UTILITY_H
#define UTILITY_H

#include <glib.h>
#include <netinet/in.h> // struct sockaddr_include
#include <arpa/inet.h>  // intet_ntop()

#include "header.h"

void getIpAndPort(GString *ip, struct sockaddr_in *client);
void getURL(struct httpRequest *req, GString *url);
void extractHostFromHeaders(GString *headers, GString *host);

void addHtmlToMsgBody(GString *msgBody);

#endif
