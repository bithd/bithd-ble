#include "include_all.h"

pstorage_handle_t       USEblock_handle;  
pstorage_handle_t       base_handle;     
pstorage_module_param_t param;

volatile unsigned char pstorage_wait_flag=0; 

unsigned char storage_app_point=0;

static void example_cb_handler(pstorage_handle_t  * handle,
															 uint8_t              op_code,
                               uint32_t             result,
                               uint8_t            * p_data,
                               uint32_t             data_len)
{

	switch(op_code)
	{
		case PSTORAGE_LOAD_OP_CODE:
				if (result == NRF_SUCCESS)
				{
				pstorage_wait_flag=0;
				}
				else
				{
				}
		break;
		case PSTORAGE_STORE_OP_CODE:
				if (result == NRF_SUCCESS)
				{
				pstorage_wait_flag=0;
				}
				else
				{
				}
		break;				 
		case PSTORAGE_UPDATE_OP_CODE:
				if (result == NRF_SUCCESS)
				{

				}
				else
				{

				}
		break;
		case PSTORAGE_CLEAR_OP_CODE:
				if (result == NRF_SUCCESS)
				{
				pstorage_wait_flag=0;
				}
				else
				{

				}
		break;
					
	}			
}

void mcuflash_init(void)
{
    param.block_size  = 16;
	param.block_count = 66+34;
	param.cb          = example_cb_handler;
	pstorage_register(&param, &base_handle);
	if(pstorage_flag==0)
	{
		pstorage_wait_flag=1;
		pstorage_clear(&base_handle, 66 * 16);
		while(pstorage_wait_flag==1);
	}
	
}



















