#if !defined(FLOW_H)
#define FLOW_H

int shutdown_server(const int r_code, const char *log);
int init_connection(const char *port);
void signal_handler(int sig);
#endif // FLOW_H
