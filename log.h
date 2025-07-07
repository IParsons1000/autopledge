/*
 *
 * (c)2025 Ira Parsons
 * syslog.h - syslog interface (specification)
 *
 */

#ifndef _AUTOPLEDGE_LOG_H
#define _AUTOPLEDGE_LOG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef USE_SYSLOG

# include <syslog.h>

# define log_init() openlog("libautopledge.so", LOG_CONS | LOG_PERROR | LOG_PID, LOG_AUTH)
# define log_note(...) syslog(LOG_INFO, __VA_ARGS__)
# define log_error(...) syslog(LOG_ERR, __VA_ARGS__)
# define log_debug(...) syslog(LOG_DEBUG, __VA_ARGS__)
# define log_close() closelog()

#else /* !USE_SYSLOG */

# define log_init()
# define log_note(...)
# define log_error(...)
# define log_debug(...)
# define log_close()

#endif /* USE_SYSLOG */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_LOG_H */
