/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 8999 $
 * $Date: 2010-04-12 15:42:36 +0800 (?��?一, 12 ?��? 2010) $
 *
 * Purpose : register service APIs in the SDK.
 *
 * Feature : register service APIs
 *
 */

#ifndef __HAL_MAC_REG_H__
#define __HAL_MAC_REG_H__

/*
 * Include Files
 */
#include <hal/chipdef/allreg.h>


/*
 * Symbol Definition
 */
#define REG_ARRAY_INDEX_NONE    (-1)


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

typedef struct rtk_reg_info_s
{
    uint32 offset;          /* offset address */
    uint32 lport;           /* port start index */
    uint32 hport;           /* port end index */
    uint32 larray;          /* array start index */
    uint32 harray;          /* array end index */
    uint32 bit_offset;      /* array offset */
    uint32 is_PpBlock;      /* per-port MAC block or not? */
} rtk_reg_info_t;



/* Function Name:
 *      reg_read
 * Description:
 *      Read one specified register from the chip or lower layer I/O.
 * Input:
 *      reg    - register index
 * Output:
 *      pValue - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
extern int32
reg_read(uint32 reg, uint32 *pValue);

/* Function Name:
 *      reg_write
 * Description:
 *      Write one specified register to the chip or lower layer I/O.
 * Input:
 *      reg   - register index
 *      pValue - pointer buffer of register data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      Use semaphore for prevent race condition with reg_field_write
 */
extern int32
reg_write(uint32 reg, uint32 *pValue);

/* Function Name:
 *      reg_field_get
 * Description:
 *      Get the value from one specified field of register in buffer.
 * Input:
 *      reg    - register index
 *      field  - field index of the register
 *      pData   - pointer buffer of register data
 * Output:
 *      pValue - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
extern int32
reg_field_get(
    uint32  reg,
    uint32  field,
    uint32  *pValue,
    uint32  *pData);

/* Function Name:
 *      reg_field_set
 * Description:
 *      Set the value to one specified field of register in buffer.
 * Input:
 *      reg   - register index
 *      field - field index of the register
 *      pValue - pointer buffer of value from the specified field of register
 * Output:
 *      pData - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
extern int32
reg_field_set(
    uint32  reg,
    uint32  field,
    uint32  *pValue,
    uint32  *pData);

/* Function Name:
 *      reg_field_read
 * Description:
 *      Read the value from one specified field of register in chip.
 * Input:
 *      reg    - register index
 *      field  - field index of the register
 * Output:
 *      pValue - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
extern int32
reg_field_read(
    uint32  reg,
    uint32  field,
    uint32  *pValue);

/* Function Name:
 *      reg_field_write
 * Description:
 *      Write the value to one specified field of register in chip.
 * Input:
 *      unit  - unit id
 *      reg   - register index
 *      field - field index of the register
 *      pValue - pointer buffer of value from the specified field of register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      Use semaphore for prevent race condition with reg_write
 */
extern int32
reg_field_write(
    uint32  reg,
    uint32  field,
    uint32  *pValue);

/* Function Name:
 *      reg_idx2Addr_get
 * Description:
 *      Register index to physical address transfer in chip.
 * Input:
 *      unit  - unit id
 *      reg   - register index
 * Output:
 *      pAddr - pointer buffer of register address
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern int32 
reg_idx2Addr_get(uint32 reg, uint32 *pAddr);


/* Function Name:
 *      reg_idxMax_get
 * Description:
 *      Get the maximum value of register index in chip.
 * Input:
 *      reg  - register index
 * Output:
 *      pMax - pointer buffer of maximum value of register index
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      None
 */
extern int32 
reg_idxMax_get(uint32 *pMax);


/*#################################################*/
/* Following APIS are for register array mechanism */
/*#################################################*/
/* Function Name:
 *      reg_array_read
 * Description:
 *      Read one specified register or register array from the chip or lower layer I/O.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 * Output:
 *      pValue  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_array_read(uint32 reg, int32 index_1, int32 index_2, uint32 *pValue);

/* Function Name:
 *      reg_array_write
 * Description:
 *      Write one specified register or register array from the chip or lower layer I/O.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 * Output:
 *      pValue  - pointer buffer of register data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_array_write(uint32 reg, int32 index_1, int32 index_2, uint32 *pValue);

/* Function Name:
 *      reg_array_field_read
 * Description:
 *      Read the value from one specified field of register or register array in chip.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 *      field   - field index of the register
 * Output:
 *      pValue  - pointer buffer of value from the specified field of register
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      The function is supported 32-bit register access.
 */
int32
reg_array_field_read(
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue);

/* Function Name:
 *      reg_array_field_write
 * Description:
 *      Write the value to one specified field of register or register array in chip.
 * Input:
 *      reg     - register index
 *      index_1 - dimension 1 index
 *      index_2 - dimension 2 index
 *      field   - field index of the register
 *      pValue  - pointer buffer value from the specified field of register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. Use semaphore for prevent race condition with reg_write
 *      2. The function is supported 32-bit register access.
 */
int32
reg_array_field_write(
    uint32  reg,
    int32   index_1,
    int32   index_2,
    uint32  field,
    uint32  *pValue);

/* Function Name:
 *      reg_info_get
 * Description:
 *      Get register information of the specified register index.
 * Input:
 *      reg   - register index
 * Output:
 *      pData - pointer buffer of register information
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32 
reg_info_get(uint32 reg, rtk_reg_info_t *pData);

#endif  /* __HAL_MAC_REG_H__ */
