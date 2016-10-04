#ifndef HEADER_H_
#define HEADER_H_

#define STATUSSIZE 3
#define VERSIONSIZE 8
#define PHRASESIZE 128

#define HEADERNAMESIZE 128
#define HEADERVALUESIZE 16

#include <glib.h>
#include <string.h>


// Struct Object That Represents The Response Object
struct httpResponse {
    GString *statusLine;
    GString *headers;
    GString *msgBody;
};

// Request Parameters
struct httpRequest {
    GString *method;
    GString *target;
    GString *protocol;
    GString *headers;
    GString *message;
};

#endif
