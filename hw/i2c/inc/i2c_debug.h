#ifndef __I2C_DEBUG_H__
#define __I2C_DEBUG_H__

#define i2c_parem_invalid()         printf("%s[%d] ERROR: param Invalid !\n", __func__, __LINE__)
#define i2c_pointer_invalid(ptr)    printf("%s[%d] ERROR: %s is null pointer !\n", __func__, __LINE__, #ptr)
#define i2c_function_err(func)      printf("%s[%d] ERROR: %s faild !\n", __func__, __LINE__, #func)
#define i2c_check_chanel(id)        printf("%s[%d] ERROR: channel id %d is nvalid !\n", __func__, __LINE__, (id))
#define i2c_debug_err               printf
#define i2c_debug_info              printf
#define i2c_debug_dbg               printf
#endif