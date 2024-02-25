#pragma once
/***********************************************************************************/
// Continuous communciation between client and server threads
// Proxy patterned client & Singleton patterned server
// Mutex & Semaphore
// TLS as a seperated Session and Presentation Layers
/***********************************************************************************/

/***********************************************************************************/
/******************************** INCLUDES *****************************************/
/***********************************************************************************/

/***********************************************************************************/
/******************************** DEFINITIONS **************************************/
/***********************************************************************************/
/* Request messages - Message size (bytes) */
#define RQ_MESSAGESIZE_MAX			(unsigned int)500
/* Request messages - Server side info */
#define RQ_SRV_MEMORY_TYPE			(char*)"MemType"
#define RQ_SRV_MEMORY_SIZE			(char*)"MemSize"
/* Request messages - Client side info */
#define RQ_CLT_STATUS_CHK			(char*)"StatInfo"

/* Response messages */
#define RP_SRV_MEMORY_TYPE_RAM		(char*)"RAM"
#define RP_SRV_MEMORY_TYPE_FLSH		(char*)"FLASH"

/***********************************************************************************/
/************************ FUNCTION IMPLEMENTATIONS *********************************/
/***********************************************************************************/
