#include "logger.h"

void logEntry(struct sockaddr_in *client, struct httpRequest *req){
    GString *entry = g_string_new("");
    createLogEntry(entry, req, client, 200);

    FILE *fp = fopen("log.txt", "a");

    if(fp){
        fwrite(entry->str, 1, entry->len, fp);
    }
    else {
        printf("error opening log file\n");
    }
}


void createLogEntry(GString *entry, struct httpRequest *req, struct sockaddr_in *client, int responseCode){
    GString *ip = g_string_new(""),
        *url = g_string_new("");

    getIpAndPort(ip, client);
    getURL(req, url);

    GTimeVal time;
    g_get_current_time(&time);
    char *time_str = g_time_val_to_iso8601(&time);

    g_string_printf(entry, "%d\n", responseCode);
    g_string_prepend(entry, " : ");
    g_string_prepend(entry, url->str);
    g_string_prepend(entry, "\n");
    g_string_prepend(entry, req->method->str);
    g_string_prepend(entry, " ");
    g_string_prepend(entry, ip->str);
    g_string_prepend(entry, " : ");
    g_string_prepend(entry, time_str);

    g_free(time_str);
    g_string_free(ip, TRUE);
    g_string_free(url, TRUE);
}
