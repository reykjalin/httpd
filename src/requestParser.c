#include "requestParser.h"

void parseRequest(char *req, struct httpRequest *parsed){
    char **reqLines = g_strsplit(req, "\r\n", 2);
    char **reqHeader = g_strsplit(reqLines[0], " ", -1);
    char **reqHeadersAndMsg = g_strsplit(reqLines[1], "\r\n\r\n", 2);

    g_string_assign(parsed->method, reqHeader[0]);
    g_string_assign(parsed->target, reqHeader[1]);
    g_string_assign(parsed->protocol, reqHeader[2]);
    g_string_assign(parsed->headers, reqHeadersAndMsg[0]);
    g_string_assign(parsed->message, reqHeadersAndMsg[1]);

    g_strfreev(reqLines);
    g_strfreev(reqHeader);
    g_strfreev(reqHeadersAndMsg);
}

void initRequestStruct(struct httpRequest *req){
    req->method = g_string_new("");
    req->target = g_string_new("");
    req->protocol = g_string_new("");
    req->headers = g_string_new("");
    req->message = g_string_new("");
}

void freeRequestStruct(struct httpRequest *req){
    g_string_free(req->method, TRUE);
    g_string_free(req->target, TRUE);
    g_string_free(req->protocol, TRUE);
    g_string_free(req->headers, TRUE);
    g_string_free(req->message, TRUE);
}
