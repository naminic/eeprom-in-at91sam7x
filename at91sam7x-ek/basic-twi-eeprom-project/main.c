//------------------------------------------------------------------------------
//         Headers

//WWW.NAMINIC.COM

//------------------------------------------------------------------------------

#include <board.h>
#include <pio/pio.h>
#include <dbgu/dbgu.h>
#include <twi/twi.h>
#include <utility/math.h>
#include <utility/assert.h>
#include <utility/trace.h>
#include <drivers/twi/twid.h>

#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------

/// TWI peripheral redefinition if needed
#if !defined(AT91C_BASE_TWI) && defined(AT91C_BASE_TWI0)
    #define AT91C_BASE_TWI      AT91C_BASE_TWI0
    #define AT91C_ID_TWI        AT91C_ID_TWI0
    #define PINS_TWI            PINS_TWI0
#endif

/// TWI clock frequency in Hz.
#define TWCK            400000

/// Slave address of AT24C chips.
#define AT24C_ADDRESS   0x50

/// Page size of an AT24C02 chip (in bytes)
#define PAGE_SIZE       8

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

/// Pio pins to configure.
#include <at91sam7x256.H>
static const Pin pins[] = {PINS_TWI};

/// TWI driver instance.
static Twid twid;

/// Page buffer.
static unsigned char pData[PAGE_SIZE];

//------------------------------------------------------------------------------
///        Local functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
///        Global functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Main function
//------------------------------------------------------------------------------
int main()
{
    unsigned int i;
   	//Section 1 
    PIO_Configure(pins, PIO_LISTSIZE(pins));
    TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
    printf("Basic TWI EEPROM Project\n\rNaminicGroup,MajidKhalili\n\r");
    
	//Section 2
    // Configure TWI
    // In polling mode: try to disable all IRQs if possible.
    // (in this example it does not matter, there is only the TWI IRQ active)
    AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_TWI;
    TWI_ConfigureMaster(AT91C_BASE_TWI, TWCK, BOARD_MCK);
    TWID_Initialize(&twid, AT91C_BASE_TWI);

	//Section 3
    // Erase page #0 and #1
    memset(pData, 0, PAGE_SIZE);
    printf("Filling page #0 with zeroes ...\n\r");
    TWID_Write(&twid, AT24C_ADDRESS, 0x0000, 1, pData, PAGE_SIZE, 0);

    // Wait at least 10 ms
    for (i=0; i < 1000000; i++);

    

    // Writting operation
    printf("Writting on page #0\n\r");

    //Section 4
	// Write checkerboard pattern in first page
    for (i=0; i < PAGE_SIZE; i++) 
	{
	    // Even
        if ((i & 1) == 0) 
		{
        	pData[i] = 0xAA;
        }
        // Odd
        else 
		{
			pData[i] = 0xBB;
        }
    }
    TWID_Write(&twid, AT24C_ADDRESS, 0x0000, 1, pData, PAGE_SIZE, 0);

    // Wait at least 10 ms
    for (i=0; i < 1000000; i++);

    //Section 5
	// Read back data
    memset(pData, 0, PAGE_SIZE);
    TWID_Read(&twid, AT24C_ADDRESS, 0x0000, 1, pData, PAGE_SIZE, 0);


	printf("Data Read From EEPROM : \n\r");
    for (i=0; i < PAGE_SIZE; i++) 
	{
		printf("0x%X ",pData[i]);
		if ((i+1)%8 == 0)
			printf("\n\r");
    }

    return 0;
}

