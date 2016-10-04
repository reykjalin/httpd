#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <glib.h>
#include <netinet/in.h> // struct sockaddr_include
#include <arpa/inet.h>  // intet_ntop()

#include "header.h"
#include "utility.h"

void logEntry(struct sockaddr_in *client, struct httpRequest *req);

void createLogEntry(GString *entry, struct httpRequest *req, struct sockaddr_in *client, int responseCode);

#endif
