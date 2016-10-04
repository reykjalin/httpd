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
