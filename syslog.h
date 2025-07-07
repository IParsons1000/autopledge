/*
 *
 * (c)2025 Ira Parsons
 * syslog.h - syslog interface (specification)
 *
 */

#ifndef _AUTOPLEDGE_SYSLOG_H
#define _AUTOPLEDGE_SYSLOG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <syslog.h>

#define log_init() openlog("libautopledge.so", LOG_CONS | LOG_PERROR | LOG_PID, LOG_AUTH)
#define log_note(...) syslog(LOG_INFO, __VA_ARGS__)
#define log_error(...) syslog(LOG_ERR, __VA_ARGS__)
#define log_close() closelog()

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_SYSLOG_H */
