#include "appbond.h"
#include "app_error.h"


void app_bond_init(app_bond_table_t * p_bond_table)
{
    uint32_t                        err_code;  
    dm_application_context_t        app_context;
    dm_handle_t                     dm_handle = {.appl_id = 0}; 
    static uint32_t          		context_data;
    
    app_context.len    =  sizeof(context_data);
    app_context.p_data = (uint8_t *)&context_data;
               
    for(uint8_t device_id = 0; device_id < DEVICE_MANAGER_MAX_BONDS; device_id++)  
    {
        dm_handle.device_id = device_id;
        
        err_code = dm_application_context_get(&dm_handle,&app_context);
        if(err_code == NRF_SUCCESS)
        {
           
           p_bond_table->app_bond_cnt[device_id] =*((uint32_t *)app_context.p_data);
           p_bond_table->device_id[device_id] = device_id; 
                
        }
        else if(err_code == (NRF_ERROR_INVALID_ADDR | DEVICE_MANAGER_ERR_BASE))
        {
            //No context available. 
            p_bond_table->app_bond_cnt[device_id] = NO_APP_CONTEXT;
            p_bond_table->device_id[device_id] = device_id; 
        }
        else if(err_code == DM_NO_APP_CONTEXT)
        {   
            //set context as invalid
            p_bond_table->app_bond_cnt[device_id] = INVALID_APP_CONTEXT;
            p_bond_table->device_id[device_id] = device_id; 
        }
        else
        {
            APP_ERROR_CHECK(err_code);
        }
    }     
}

void app_bond_find(app_bond_table_t * p_bond_table,table_index_t * p_index)
{
	p_index->mr_cnt_val = 0;
	p_index->lr_cnt_val = NO_APP_CONTEXT;
	
	for(uint8_t i = 0; i < DEVICE_MANAGER_MAX_BONDS; i++)
    {
        //Find most recently stored bond 
        if((p_index->mr_cnt_val < p_bond_table->app_bond_cnt[i]) &&
           (p_bond_table->app_bond_cnt[i] != NO_APP_CONTEXT) &&
           (p_bond_table->app_bond_cnt[i] != INVALID_APP_CONTEXT))
            {
               p_index->mr_cnt_val = p_bond_table->app_bond_cnt[i];
               p_index->mr_index = i;
            }
                
        // Find least recently stored bond 
        if((p_index->lr_cnt_val > p_bond_table->app_bond_cnt[i]) &&
           (p_bond_table->app_bond_cnt[i] != NO_APP_CONTEXT) &&
           (p_bond_table->app_bond_cnt[i] != INVALID_APP_CONTEXT))
            {
                p_index->lr_cnt_val = p_bond_table->app_bond_cnt[i];
                p_index->lr_index = i;
            }
        
        // Update bond context in case it is not valid for a particular device
        if( p_bond_table->app_bond_cnt[i] == INVALID_APP_CONTEXT)
        {
            uint32_t                               err_code;
	        dm_handle_t                            dm_handle;
            dm_application_context_t               app_context;
            
            p_bond_table->app_bond_cnt[i] = p_bond_table->app_bond_cnt[p_index->mr_index] + 1;
            p_index->mr_cnt_val = p_bond_table->app_bond_cnt[i];
            p_index->mr_index = i;
            
            dm_handle.device_id     = i;
            dm_handle.appl_id       = 0;
            dm_handle.connection_id = 0;
            
            app_context.p_data = (uint8_t *)&p_bond_table->app_bond_cnt[i];
            app_context.len    = sizeof(p_bond_table->app_bond_cnt[0]);
                          
            err_code = dm_application_context_set(&dm_handle,&app_context);  
            APP_ERROR_CHECK(err_code); 
        }
                 
    }
    
}

void app_bond_update_context(app_bond_table_t * p_bond_table, dm_handle_t const * p_handle)
{   
	uint32_t                        err_code;
	dm_application_context_t        app_context;

    app_context.p_data = (uint8_t *)&p_bond_table->app_bond_cnt[p_handle->device_id];
    app_context.len    = sizeof(p_bond_table->app_bond_cnt[0]);
                          
    err_code = dm_application_context_set(p_handle,&app_context);  
    APP_ERROR_CHECK(err_code); 
    
}

