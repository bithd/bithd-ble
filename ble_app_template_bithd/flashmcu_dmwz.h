#ifndef _flashmcu_dmwz_H_
#define _flashmcu_dmwz_H_

extern pstorage_handle_t       USEblock_handle;  
extern  pstorage_handle_t       base_handle;     
extern pstorage_module_param_t param;
extern unsigned char storage_app_point;
extern volatile unsigned char pstorage_wait_flag;    
void mcuflash_init(void);
#endif
