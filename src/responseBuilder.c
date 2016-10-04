#include "responseBuilder.h"

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
    int responseCode = 200;

    if( !strncmp(request->method->str, "GET", 3) ) {
        // GET REQUEST
        generateGetResponse(response, request, responseCode, client);
    }
    else if( !strncmp(request->method->str, "HEAD", 4) ) {
        // HEAD REQUEST
        generateHeadResponse(response, responseCode);
    }
    else if( !strncmp(request->method->str, "POST", 4) ) {
        // POST REQUEST
        generatePostResponse(response, request, responseCode);
    }
    else {
        // INVALID REQUEST
    }

    logEntry(client, request, responseCode);
}

void sendResponse(struct httpResponse *response, int connectionSocket){
    GString *msg = g_string_new("");
    g_string_append(msg, response->statusLine->str);

    // ADD headers
    if(strncmp(response->headers->str, "", 1)){
        g_string_append(msg, response->headers->str);
    }

    // ADD message body
    if(strncmp(response->msgBody->str, "", 1)){
        g_string_append(msg, "\r\n"); // Empty line before msgbody
        g_string_append(msg, response->msgBody->str);
    }

    // SEND response
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

void generateGetResponse(struct httpResponse *resp, struct httpRequest *req,
                         int statusCode, struct sockaddr_in *client)
{
    // Status line
    g_string_printf(resp->statusLine, "%d", statusCode);
    g_string_prepend(resp->statusLine, "HTTP/1.1 ");
    if(statusCode == 200)
        g_string_append(resp->statusLine, " OK\r\n");

    // Message body
    GString *ip = g_string_new("");
    GString *url = g_string_new("");

    // Get url and ip:port
    getIpAndPort(ip, client);
    getURL(req, url);

    // Add requested data
    g_string_append(resp->msgBody, url->str);
    g_string_append(resp->msgBody, " ");
    g_string_append(resp->msgBody, ip->str);

    // Add html data
    addHtmlToMsgBody(resp->msgBody);

    // Headers
    g_string_printf(resp->headers, "%d\r\n", (int) resp->msgBody->len);
    g_string_prepend(resp->headers, "Content-Length: ");

    g_string_free(ip, TRUE);
    g_string_free(url, TRUE);
}


void generatePostResponse(struct httpResponse *resp, struct httpRequest *req, int statusCode)
{
    // Status line
    g_string_printf(resp->statusLine, "%d", statusCode);
    g_string_prepend(resp->statusLine, "HTTP/1.1 ");
    if(statusCode == 200)
        g_string_append(resp->statusLine, " OK\r\n");

    // Message body
    if(strncmp(req->message->str, "", 1))
        g_string_assign(resp->msgBody, req->message->str);

    // Add html data
    addHtmlToMsgBody(resp->msgBody);

    // Headers
    g_string_printf(resp->headers, "%d\r\n", (int) resp->msgBody->len);
    g_string_prepend(resp->headers, "Content-Length: ");
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
