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



void createResponse(struct httpRequest *request, struct httpResponse *response,
                    struct sockaddr_in *client, gboolean closeConnection)
{
    int responseCode = 200;

    if( !g_strstr_len(request->headers->str, request->headers->len, "Host: ") ){
        // INVALID HEADERS
        responseCode = 400;
    }

    generateStatusLine(response, responseCode);
    generateConnectionHeader(response->headers, closeConnection);

    if(responseCode != 200)
        return;

    if( !strncmp(request->method->str, "GET", 3) ) {
        // GET REQUEST
        generateGetResponse(response, request, client);
    }
    else if( !strncmp(request->method->str, "POST", 4) ) {
        // POST REQUEST
        generatePostResponse(response, request);
    }
    else if( strncmp(request->method->str, "HEAD", 4) ) {
        // INVALID REQUEST
        // EVERYTHING BUT GET, POST AND HEAD REQUESTS
    }

    logEntry(client, request, responseCode);
}

void sendResponse(struct httpResponse *response, int connectionSocket){
    GString *msg = g_string_new("");
    g_string_append(msg, response->statusLine->str);

    // ADD headers
    if(response->headers->len > 0){
        g_string_append(msg, response->headers->str);
    }

    // ADD message body
    if(response->msgBody->len > 0){
        g_string_append(msg, "\r\n"); // Empty line before msgbody
        g_string_append(msg, response->msgBody->str);
    }

    // SEND response
    send(connectionSocket, msg->str, (size_t) msg->len, 0);
    g_string_free(msg, TRUE);
}


void generateStatusLine(struct httpResponse *resp, unsigned int statusCode) {
    g_string_printf(resp->statusLine, "%d", statusCode);
    g_string_prepend(resp->statusLine, "HTTP/1.1 ");
    if(statusCode == 200)
        g_string_append(resp->statusLine, " OK\r\n");
    else if(statusCode == 400)
        g_string_append(resp->statusLine, " Bad Request\r\n");
}

void generateConnectionHeader(GString *headers, gboolean closeConnection){
    if(closeConnection)
        g_string_assign(headers, "Connection: close\r\n");
    else
        g_string_assign(headers, "Connection: keep-alive\r\n");
}

void generateGetResponse(struct httpResponse *resp, struct httpRequest *req, struct sockaddr_in *client)
{
    // Generate message body
    GString *colour = g_string_new("");
    generateGetMsgBody(resp, colour, req, client);

    if(colour->len <= 0){
        GString *header = g_string_new("Connection");
        getHeaderData(header);
        if(header->len > 0){
            g_string_append(resp->msgBody, "\r\n");
            g_string_append(resp->msgBody, req->headers->str);
        }

        g_string_assign(header, "Host");
        getHeaderData(header);

        if(header->len > 0){
            g_string_append(resp->msgBody, "\r\n");
            g_string_append(resp->msgBody, req->headers->str);
        }
        g_string_append(resp->msgBody, "\r\n");
        g_string_free(header, TRUE);
    }

    // Add html data
    addHtmlToMsgBody(resp->msgBody, colour);

    // Headers
    GString *contentLength = g_string_new("");
    g_string_printf(contentLength, "Content-Length: %d\r\n", (int) resp->msgBody->len);
    g_string_prepend(resp->headers, contentLength->str);

    g_string_free(contentLength, TRUE);
    g_string_free(colour, TRUE);
}

void generateGetMsgBody(struct httpResponse *resp, GString *colour,
                        struct httpRequest *req, struct sockaddr_in *client){
     // Message body
    GString *ip = g_string_new("");
    GString *url = g_string_new("");
    GString *page = g_string_new("");
    GString *query = g_string_new("");

    parsePage(req->target, page);
    parseQuery(req->target, query);

    if(strncmp(page->str, "colour", 6)){
        // Get url and ip:port
        getIpAndPort(ip, client);
        getURL(req, url);

        // Add requested data
        g_string_append(resp->msgBody, url->str);
        g_string_append(resp->msgBody, " ");
        g_string_append(resp->msgBody, ip->str);

        if( !strncmp(page->str, "test", 4) ){
            // test page requested
            if(query->len > 0){
                g_string_append(resp->msgBody, "\r\n");
                g_string_append(resp->msgBody, query->str);
            }

        }
    }
    else{
        // colour page requested
        char *cookie;
        if(query->len > 3 && g_str_has_prefix(query->str, "bg=")){
            g_string_assign(colour, &query->str[3]);
            g_string_append(resp->headers, "Set-Cookie: ");
            g_string_append(resp->headers, query->str);
            g_string_append(resp->headers, "\r\n");
        }
        else if( (cookie = g_strstr_len(req->headers->str, req->headers->len, "Cookie: ")) ){
            char **cookieArray = g_strsplit(cookie, ": ", 2);
            if(g_str_has_prefix(cookieArray[1], "bg=")){
                g_string_assign(colour, &cookieArray[1][3]);
                g_string_append(resp->headers, "Set-Cookie: ");
                g_string_append(resp->headers, cookieArray[1]);
                g_string_append(resp->headers, "\r\n");
            }
        }
        // color query not found
    }

    g_string_free(ip, TRUE);
    g_string_free(url, TRUE);
    g_string_free(page, TRUE);
    g_string_free(query, TRUE);
}


void generatePostResponse(struct httpResponse *resp, struct httpRequest *req)
{
    // Message body
    if(req->message->len > 0){
        g_string_assign(resp->msgBody, req->message->str);
    }
    g_string_append(resp->msgBody, "\n");
    g_string_append(resp->msgBody, req->headers->str);

    GString *colour = g_string_new("");
    addHtmlToMsgBody(resp->msgBody, colour);

    // Headers
    GString *contentLength = g_string_new("");
    g_string_printf(contentLength, "Content-Length: %d\r\n", (int) resp->msgBody->len);
    g_string_prepend(resp->headers, contentLength->str);

    g_string_free(colour, TRUE);
}
