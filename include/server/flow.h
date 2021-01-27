#if !defined(FLOW_H)
#define FLOW_H

/**
 * @brief 
 * 
 * @param r_code 
 * @param log 
 * @return int 
 */
int shutdown_server(const int r_code, const char *log);

/**
 * @brief 
 * 
 * @param port 
 * @return int 
 */
int init_connection(const char *port);

/**
 * @brief 
 * 
 * @param sig 
 */
void signal_handler(int sig);

#endif // FLOW_H
