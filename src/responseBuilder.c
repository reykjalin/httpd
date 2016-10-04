#include "responseBuilder.h"

const int HEADER_LENGTH = 6;

void respond() {
   return;
}

void initHttpResponseStruct(struct httpResponse *response){
    response->statusLine = g_string_new("");
    response->headers = g_string_new("");
    response->msgBody = g_string_new("");
}
void freeHttpResponseStruct(struct httpResponse *response){
    g_string_free(response->statusLine, TRUE);
    g_string_free(response->headers, TRUE);
    g_string_free(response->msgBody, TRUE);
}



void createResponse(struct httpRequest *request, struct httpResponse *response, struct sockaddr_in *client) {
    if(strcmp(request->method->str, "GET") == 0) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client->sin_addr), ip, INET_ADDRSTRLEN);
        GString *ipaddr = g_string_new(ip);

        GString *port = g_string_new("");
        g_string_printf(port, "%d", client->sin_port);

        generateGetResponse(response, request, 200, ipaddr, port);

        g_string_free(ipaddr, TRUE);
        g_string_free(port, TRUE);
    }
    else if(strncmp(request->method->str, "HEAD", 4) == 0) {
        // HEAD REQUEST
        generateHeadResponse(response, 200);
    }
    else if(strncmp(request->method->str, "POST", 4) == 0) {
        // POST REQUEST
    }
    else {
        // INVALID REQUEST
    }
}

void sendResponse(struct httpResponse *response, int connectionSocket){
    GString *msg = g_string_new("");
    g_string_append(msg, response->statusLine->str);

    if(strncmp(response->msgBody->str, "", 1)){
        g_string_append(msg, response->headers->str);
    }

    if(strncmp(response->msgBody->str, "", 1)){
        g_string_append(msg, "\r\n"); // Empty line before msgbody
        g_string_append(msg, response->msgBody->str);
    }

    send(connectionSocket, msg->str, (size_t) msg->len, 0);
    g_string_free(msg, TRUE);
}


void generateHeadResponse(struct httpResponse *resp, unsigned int statusCode) {
    g_string_printf(resp->statusLine, "%d", statusCode);
    g_string_prepend(resp->statusLine, "HTTP/1.1 ");
    if(statusCode == 200){
        g_string_append(resp->statusLine, " OK\r\n");
    }
}

void generateGetResponse(struct httpResponse *resp,
                         struct httpRequest *req,
                         unsigned int statusCode,
                         GString *ip,
                         GString *port)
{
    // Status line
    g_string_printf(resp->statusLine, "%d", statusCode);
    g_string_prepend(resp->statusLine, "HTTP/1.1");
    if(statusCode == 200){
        g_string_append(resp->statusLine, "OK\r\n");
    }

    // Message body
    GString *host = g_string_new("");
    extractHostFromHeaders(req->headers, host);

    g_string_printf(resp->msgBody, "http://%s%s %s:%s",
                    host->str, req->target->str, ip->str, port->str);
    g_string_prepend(resp->msgBody, "<!DOCTYPE html>\n<body>\n<p>");
    g_string_append(resp->msgBody, "</p>\n</body>");

    // Headers
    g_string_printf(resp->headers, "%d\r\n", (int) resp->msgBody->len);
    g_string_prepend(resp->headers, "Content-Length: ");

    g_string_free(host, TRUE);
}

int readFile(struct httpRequest *req, struct httpResponse *resp)
{
    GString *fname = g_string_erase(req->target, 0, 1); // Remove first '/'
    FILE *fp/* = fopen(fname->str, "r")*/;

    int retVal = 1;
    if((fp = fopen(fname->str, "r")) == NULL) 
    {
      printf("error opening file\n");
      return 1;

    }

    /*get the length of the file */    
    fseek(fp, 0, SEEK_END); // Move fstream to end of file
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET); // Move to start of file

    char *buf;
    if((buf = malloc(len)) != NULL)
    {
        if(fread(buf, 1, len, fp))
        {
            g_string_assign(resp->msgBody, buf);
            retVal = 0; // SUCCESS
        }
        free(buf);
    }
    fclose(fp);
    g_string_free(fname, TRUE);

    return retVal;
}
