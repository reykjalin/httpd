#include "utility.h"

void getIpAndPort(GString *ip, struct sockaddr_in *client){
    char ipaddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client->sin_addr), ipaddr, INET_ADDRSTRLEN);

    g_string_printf(ip, "%d", client->sin_port);
    g_string_prepend(ip, ":");
    g_string_prepend(ip, ipaddr);
}

void getURL(struct httpRequest *req, GString *url){
    extractHostFromHeaders(req->headers, url);
    g_string_append(url, req->target->str);
}

void extractHostFromHeaders(GString *headers, GString *host){
    char *h = g_strstr_len(headers->str, -1, "Host: ");

    if(h){
        char **hostArray = g_strsplit(h, ": ", 2);
        char **hostLine = g_strsplit(hostArray[1], "\r\n", 2);

        g_string_assign(host, hostLine[0]);
        g_string_prepend(host, "http://");

        g_strfreev(hostArray);
        g_strfreev(hostLine);
    }
}

void addHtmlToMsgBody(GString *msgBody, GString *colour){
    char **msgArray = g_strsplit(msgBody->str, "\r\n", -1);
    int counter = 0;
    g_string_assign(msgBody, "");
    while(msgArray[counter]){
        g_string_append(msgBody, "<p>");
        g_string_append(msgBody, msgArray[counter]);
        g_string_append(msgBody, "</p>\n");

        counter += 1;
    }
    if(colour->len > 0){
        g_string_prepend(msgBody, ";\">\n");
        g_string_prepend(msgBody, colour->str);
        g_string_prepend(msgBody, "<body style=\"background-color:");
    }
    else
        g_string_prepend(msgBody, "<body>\n");

    g_string_prepend(msgBody, "<!DOCTYPE html>\n<html>\n");
    g_string_append(msgBody, "</body>\n</html>\r\n");

    g_strfreev(msgArray);
}

void parseQuery(GString *target, GString *query){
    char *queryStart = g_strstr_len(target->str, target->len, "?");
    if(queryStart){
        char **queryArray = g_strsplit(queryStart, "#", 2);
        g_string_assign(query, &queryArray[0][1]);
        g_strfreev(queryArray);
    }
}

void parsePage(GString *target, GString *page){
    if(target->len > 1){
        char *pageStart = &target->str[1];
        char **pageArray = g_strsplit(pageStart, "?", 2);
        g_string_assign(page, pageArray[0]);
        g_strfreev(pageArray);
    }
}

void getHeaderData(struct httpRequest *req, GString *header){
    char **headerArray = g_strsplit(req->headers->str, "\r\n", -1);

    gboolean changed = FALSE;
    int counter = 0;
    while(headerArray[counter]){
        char **headerLine = g_strsplit(headerArray[counter], ": ", 2);

        if( !(strncmp(headerLine[0], header->str, header->len)) ){
            g_string_assign(header, headerLine[1]);
            g_strfreev(headerLine);
            changed = TRUE;
            break;
        }

        g_strfreev(headerLine);
        counter += 1;
    }

    if(!changed)
        g_string_assign(header, "");

    g_strfreev(headerArray);
}
