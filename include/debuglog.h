#ifndef __DEBUGLOG_H__ 
#define __DEBUGLOG_H__
#define SYS_LOG_DEBUG
#ifdef  SYS_LOG_DEBUG
#define sys_log(format,...) LOG(""__FILE__"-%s-%04d: "format"",__FUNCTION__, __LINE__, ##__VA_ARGS__); 
#endif

#ifdef  SYS_LOG_FILE
#define SYS_LOG_FILE
#endif
extern void LOG(const char* ms, ...);
#endif
