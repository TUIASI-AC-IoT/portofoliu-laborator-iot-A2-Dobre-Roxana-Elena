#ifndef _HTTP_S_H_
#define _HTTP_S_H_

#define MAX_APs 10

static wifi_ap_record_t ap_info[MAX_APs];
static uint16_t ap_count = 0;

httpd_handle_t start_webserver(void);
bool save_credentials(const char* ssid, const char* pass);

#endif