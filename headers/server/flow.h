#if !defined(FLOW_H)
#define FLOW_H

int shutdown_server(int r_code);
int init_connection(const char *port);
void signal_handler(int sig);
#endif // FLOW_H
