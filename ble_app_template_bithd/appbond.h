#ifndef APP_BOND_H__
#define APP_BOND_H__

/** @file
 *
 * @defgroup app_bond Application bond handler
 * @{
 * @ingroup app_common
 *
 * @brief Application bond handler.
 *
 * @details The application bond handler lets the application keep track of which order the bonds were
 *          stored in. This can be used to determine which device to delete from the bond table. 
 *
 * @note    app_bond relies on the device_manager for storing the bond indexes persistently in  
 *          the Application Context of the bond table. The user must ensure that at least 4 bytes
 *          is allocated for the Application Context in device_manager_cnfg.h, and that device manger 
 *          initialization has completed before using this module. 
 *
 * @note    
 */

#include <stdint.h>
#include "device_manager.h"

#define NO_APP_CONTEXT                   0xFFFFFFFF         /**< Value to indicate no application context. This is true when there is not bond info for the given device.*/
#define INVALID_APP_CONTEXT              0                  /**< Value if app context failed to be fully updated in previous session. May occur if device received an unexpected reset during flash write*/

/**@brief app_bond_table_t index info. */
typedef struct
{
	uint8_t		    mr_index;           /**Most recent index for device  */
    uint32_t        mr_cnt_val;         /**Counter value for bond for most recent bond*/
	uint8_t         lr_index;           /**Least recent index for device */
    uint32_t        lr_cnt_val;         /**Counter value for bond for least recent bond*/
}table_index_t;

/**@brief This structure contains the application context for each device. */
typedef struct
{
    uint8_t         device_id[DEVICE_MANAGER_MAX_BONDS];	    /**device id's as stored in bond table*/		
    uint32_t        app_bond_cnt[DEVICE_MANAGER_MAX_BONDS];     /**Counter variable which is incremented for every new bond */
}app_bond_table_t; 



/**@brief Function for initializing module.
 *
 * @details Loads app context into p_bond_table.
 *
 * @note Function needs to be called after device manager has been initialized
 *
 * @param[in]     p_bond_table Pointer to RAM table.       
 */
void app_bond_init(app_bond_table_t * p_bond_table);

/**@brief Function for sorting the bond table.  
 *
 * @details Find the least recently and most recently stored bond from bond table in ram.
 *
 * @param[in]     p_bond_table Pointer to RAM table.
 * @param[in\out] p_index
 */
void app_bond_find(app_bond_table_t * p_bond_table, table_index_t * p_index);

/**@brief Function for updating the bond index in the Application context.
 *
 * @details Updates the bond index for the last stored device. 
 *
 * @note With the current implementation, only the latest bond index is being incremented. 
 *       This is to avoid having to increment the index for all devices, hence reducing flash write operations.          
 *       Using 4 bytes for the indexing ensures that it will not overflow.
 *
 * @param[in]     p_handle     The Device Manager handle that identifies the connection for which  
 *                             the context should be stored.
 * @param[in\out] p_bond_table Pointer to RAM table. 
 */
void app_bond_update_context(app_bond_table_t * p_bond_table, dm_handle_t const * p_handle);


#endif // APP_BOND_H__
