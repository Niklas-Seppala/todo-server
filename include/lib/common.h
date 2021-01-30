#if !defined COMMON_H
#define COMMON_H

#define ERROR -1
#define SUCCESS 1
#define FATAL 1

#define sfree(ptr) safe_free((void **)&ptr)

/**
 * @brief Prevents dangling pointers
 *        and double free.
 * 
 * @param ptr pointer to be freed
 */
void safe_free(void **ptr);

#endif
