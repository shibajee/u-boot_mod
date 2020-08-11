#ifndef _NET_HTTPD_H__
#define _NET_HTTPD_H__

void HttpdStart(void);
void HttpdHandler(void);

/* board specific implementation */
extern int do_http_upgrade(const ulong size, const int upgrade_type);
extern int do_http_progress(const int state);
extern void LED_ALERT_ON(void);
extern void LED_ALERT_OFF(void);

#endif
