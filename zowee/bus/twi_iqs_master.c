#if 1//defined(__TWI_MASTER_CTRL__)
#include "twi_iqs_master.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "cmsis_os.h"
#include "cmsis.h"
#include "tgt_hardware.h"
#include "stdio.h"
#include "string.h"
#include "touch_iqs269a.h"

struct iqs_iic_state iqs_state;

static bool twi_iqs_sw_master_clear_bus(void);
static bool twi_iqs_sw_master_issue_startcondition(void);
static bool twi_iqs_sw_master_issue_stopcondition(void);
static bool twi_iqs_sw_master_clock_byte(uint8_t databyte);
static bool twi_iqs_sw_master_clock_byte_in(uint8_t *databyte, bool ack);
static bool twi_iqs_sw_master_wait_while_scl_low(void);


static bool twi_iqs_sw_master_clear_bus(void)
{
    bool bus_clear;

    TWI_SW_SDA_HIGH();
    TWI_SW_SCL_HIGH();
    TWI_SW_DELAYN();

    if (TWI_SW_SDA_READ() == 1 && TWI_SW_SCL_READ() == 1)
    {
        bus_clear = true;
    }
    else if (TWI_SW_SCL_READ() == 1)
    {
        bus_clear = false;

        // Clock max 18 pulses worst case scenario(9 for master to send the rest of command and 9 for slave to respond) to SCL line and wait for SDA come high
        for (uint8_t i = 18; i--;)
        {
            TWI_SW_SCL_LOW();
            TWI_SW_DELAYN();
            TWI_SW_SCL_HIGH();
            TWI_SW_DELAYN();

            if (TWI_SW_SDA_READ() == 1)
            {
                bus_clear = true;
                break;
            }
        }
    }
    else
    {
        bus_clear = false;
    }

    return bus_clear;
}

static bool twi_iqs_sw_master_issue_startcondition(void)
{
    // Make sure both SDA and SCL are high before pulling SDA low.
    TWI_SW_SCL_HIGH();
    TWI_SW_SDA_HIGH();
    TWI_SW_DELAYN();

    if (!twi_iqs_sw_master_wait_while_scl_low())
    {
        return false;
    }

    TWI_SW_SDA_LOW();
    TWI_SW_DELAYN();

    // Other module function expect SCL to be low
    TWI_SW_SCL_LOW();
    TWI_SW_DELAYN();

    return true;
}

/**
 * Issues TWI STOP condition to the bus.
 *
 * STOP condition is signaled by pulling SDA high while SCL is high. After this function SDA and SCL will be high.
 *
 * @return
 * @retval false Timeout detected
 * @retval true Clocking succeeded
 */
static bool twi_iqs_sw_master_issue_stopcondition(void)
{
    TWI_SW_SDA_LOW();
    TWI_SW_DELAYN();

    if (!twi_iqs_sw_master_wait_while_scl_low())
    {
        return false;
    }

    TWI_SW_SDA_HIGH();
    TWI_SW_DELAYN();

    return true;
}

static bool twi_iqs_sw_master_clock_byte(uint8_t databyte)
{
    bool transfer_succeeded = true;
	uint32_t time_wait = 0;

    // Make sure SDA is an output
    TWI_SW_SDA_OUTPUT();

    // MSB first
    for (uint8_t i = 0x80; i != 0; i >>= 1)
    {
        TWI_SW_SCL_LOW();
        TWI_SW_DELAYN();

        if (databyte & i)
        {
            TWI_SW_SDA_HIGH();
        }
        else
        {
            TWI_SW_SDA_LOW();
        }

        if (!twi_iqs_sw_master_wait_while_scl_low())
        {
            transfer_succeeded = false; // Timeout
            break;
        }
    }

#if 0
    TWI_IQS_MASTER_TRACE("\n\r transfer_succeeded = %d", transfer_succeeded);
#else
    TWI_SW_DELAYN();
#endif
    // Finish last data bit by pulling SCL low
    TWI_SW_SCL_LOW();
    TWI_SW_DELAYN();

    // Configure TWI_SDA pin as input for receiving the ACK bit
    TWI_SW_SDA_INPUT();

    // Give some time for the slave to load the ACK bit on the line
   ////TWI_SW_DELAYN();
	//TWI_RH_ACK_DELAYN();//wait for ack ,delay 80us
    // Pull SCL high and wait a moment for SDA line to settle
    // Make sure slave is not stretching the clock
    transfer_succeeded &= twi_iqs_sw_master_wait_while_scl_low();
#if 0
    TWI_IQS_MASTER_TRACE("\n\r transfer_succeeded = %d", transfer_succeeded);
#else
    TWI_SW_DELAYN();
#endif
    // Read ACK/NACK. NACK == 1, ACK == 0

//h
	time_wait = 5000;

	do
	{
		TWI_RH_ACK_DELAYN();
		
	}while((1 == TWI_SW_SDA_READ())&&(time_wait--));

    transfer_succeeded &= !(TWI_SW_SDA_READ());
	//transfer_succeeded &= time_wait;
	if(time_wait==0)
	{
		transfer_succeeded = false; 
	}
	

    // Finish ACK/NACK bit clock cycle and give slave a moment to release control
    // of the SDA line
    TWI_SW_SCL_LOW();
    TWI_SW_DELAYN();

    // Configure TWI_SDA pin as output as other module functions expect that
    TWI_SW_SDA_OUTPUT();

    return transfer_succeeded;
}


static bool twi_iqs_sw_master_clock_byte_in(uint8_t *databyte, bool ack)
{
    uint8_t byte_read = 0;
    bool transfer_succeeded = true;

    // Make sure SDA is an input
    TWI_SW_SDA_INPUT();

    // SCL state is guaranteed to be high here

    // MSB first
    for (uint8_t i = 0x80; i != 0; i >>= 1)
    {
        if (!twi_iqs_sw_master_wait_while_scl_low())
        {
            transfer_succeeded = false;
            break;
        }

        if (TWI_SW_SDA_READ())
        {
            byte_read |= i;
        }
        else
        {
            // No need to do anything
        }

        TWI_SW_SCL_LOW();
        TWI_SW_DELAYN();
    }

    // Make sure SDA is an output before we exit the function
    TWI_SW_SDA_OUTPUT();

    *databyte = (uint8_t)byte_read;

    // Send ACK bit

    // SDA high == NACK, SDA low == ACK
    if (ack)
    {
        TWI_SW_SDA_LOW();
    }
    else
    {
        TWI_SW_SDA_HIGH();
    }

    // Let SDA line settle for a moment
    TWI_SW_DELAYN();

    // Drive SCL high to start ACK/NACK bit transfer
    // Wait until SCL is high, or timeout occurs
    if (!twi_iqs_sw_master_wait_while_scl_low())
    {
        transfer_succeeded = false; // Timeout
    }

    // Finish ACK/NACK bit clock cycle and give slave a moment to react
    TWI_SW_SCL_LOW();
    TWI_SW_DELAYN();

    return transfer_succeeded;
}

static bool twi_iqs_sw_master_wait_while_scl_low(void)
{
    uint32_t volatile timeout_counter = 1000; //TWI_MASTER_TIMEOUT_COUNTER_LOAD_VALUE;

    // Pull SCL high just in case if something left it low
    TWI_SW_SCL_HIGH();
    TWI_SW_DELAYN_SHORT();

    while (TWI_SW_SCL_READ() == 0)
    {
        // If SCL is low, one of the slaves is busy and we must wait

        if (timeout_counter-- == 0)
        {
            // If timeout_detected, return false
            return false;
        }
    }

    return true;
}



bool twi_iqs_master_write(uint8_t address, uint8_t reg_addr, uint8_t *data_buffer, uint8_t len)
{
    uint8_t I2c_Write_Buffer[256];
    bool bret = true;

    I2c_Write_Buffer[0] = reg_addr;
    memcpy(I2c_Write_Buffer + 1, data_buffer, len);

	bret = twi_iqs_sw_master_transfer(address << 1, I2c_Write_Buffer, 1 + len, TWI_ISSUE_STOP);

    return bret;
}

bool twi_iqs_master_read(uint8_t address, uint8_t reg_addr, uint8_t *data, uint16_t rx_size)
{
    bool bret = true;
    bret = twi_iqs_sw_master_transfer(address << 1, &reg_addr, 1, TWI_ISSUE_STOP);

    if (bret)
    {
        bret = twi_iqs_sw_master_transfer((address << 1) + 1, data, rx_size, TWI_ISSUE_STOP);
    }

    return bret;
}

bool twi_iqs_sw_master_transfer(uint8_t address, uint8_t *data, uint16_t data_length, bool issue_stop_condition)
{
    bool transfer_succeeded = true;

    transfer_succeeded &= twi_iqs_sw_master_issue_startcondition();
    transfer_succeeded &= twi_iqs_sw_master_clock_byte(address);

    TWI_SW_DELAYN_SHORT();

    if (address & TWI_READ_BIT)
    {
        /* Transfer direction is from Slave to Master */
        while (data_length-- && transfer_succeeded)
        {
            // To indicate to slave that we've finished transferring last data byte
            // we need to NACK the last transfer.
            if (data_length == 0)
            {
                transfer_succeeded &= twi_iqs_sw_master_clock_byte_in(data, (bool)false);
            }
            else
            {
                transfer_succeeded &= twi_iqs_sw_master_clock_byte_in(data, (bool)true);
            }

            data++;
        }
    }
    else
    {
        /* Transfer direction is from Master to Slave */
        while (data_length-- && transfer_succeeded)
        {
            transfer_succeeded &= twi_iqs_sw_master_clock_byte(*data);
            data++;
        }
    }

    if (issue_stop_condition || !transfer_succeeded)
    {
    	if(iqs_state.i2c_stop_bit==1)
       		 transfer_succeeded &= twi_iqs_sw_master_issue_stopcondition();
    }

    return transfer_succeeded;
}


void twi_iqs_master_init(void)
{
    twi_iqs_sw_master_init();
}

bool twi_iqs_sw_master_init(void)
{
    TWI_IQS_MASTER_TRACE("!!!");

    // Configure SCL as output
    TWI_SW_SCL_OUTPUT();
    TWI_SW_SCL_HIGH();

    // Configure SDA as output
    TWI_SW_SDA_OUTPUT();
    TWI_SW_SDA_HIGH();

    return twi_iqs_sw_master_clear_bus();
}


#endif
