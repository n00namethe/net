#define DEST_PORT 5000
#define SOURCE_PORT 6000
#define SIZE_OF_MSG 255

#define DEBUG

#define PRINT_ERR(fmt, ...) \
        do { fprintf(stderr, "%d:%s(): " fmt, \
                     __LINE__, __func__,## __VA_ARGS__); } while (0)
        
#define PRINT_INFO(fmt, ...) \
        do { fprintf(stdout, "%d:%s(): " fmt,\
                     __LINE__, __func__,## __VA_ARGS__); } while (0)

#ifdef DEBUG
#define PRINT_DEBUG_INFO(fmt, ...) \
        do { fprintf(stdout, "%d:%s(): " fmt,\
                     __LINE__, __func__,## __VA_ARGS__); } while (0)
#else
#define PRINT_DEBUG_INFO(...)
#endif