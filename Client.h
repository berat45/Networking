/***********************************************************************************/
/******************************** INCLUDES *****************************************/
/***********************************************************************************/
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "Win32Cons_ClientServer.h"

// Need to link with Ws2_32.lib, Mswsock.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

/***********************************************************************************/
/******************************** DEFINITIONS **************************************/
/***********************************************************************************/
/* IP Address of the server */
#define CLIENT_MANAGER_SERVER_ADDR		(char*)("127.0.0.1")
/* Port number of the server side */
#define CLIENT_MANAGER_SERVER_PORT		(unsigned short)55559

/***********************************************************************************/
/********************************* VARIABLES ***************************************/
/***********************************************************************************/
static SOCKET clientSocketFd;
static BOOL clientManagerInit = FALSE;

/***********************************************************************************/
/******************************** CLASSES ******************************************/
/***********************************************************************************/
class Client
{
private:

	typedef enum
	{
		CLIENT_ERROR = 0,
		CLIENT_WARNING,
		CLIENT_OK
	}CLIENT_MANAGER_RESULT;

	static Client* ClientManager;

	/* Input:  clientSocket pointer from requester
	   Return: The result of the process. In case the socket handler is not a null entity, return ERROR. Otherwise, OK.
	   Info:   This function must be called internally to create a socket handler for the first time.
	*/
	CLIENT_MANAGER_RESULT ClientManagerCreate(SOCKET* clientSocket)
	{
		if (INVALID_SOCKET == *clientSocket)
		{
			return CLIENT_ERROR;
		}

		if (INVALID_SOCKET == (*clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
		{
			printf("Client socket creation failed!\n");
			return CLIENT_ERROR;
		}

		printf("Client socket has been created successfully!\n");
		return CLIENT_OK;
	}

	/* Input:  Connect client to the server
	   Return: CLIENT_ERROR in case connection fails, CLIENT_OK otherwise.
	*/
	CLIENT_MANAGER_RESULT ClientManagerConnect(SOCKET* clientSocket)
	{
		sockaddr_in connectionParams;
		connectionParams.sin_family = AF_INET;
		connectionParams.sin_addr.s_addr = inet_addr(CLIENT_MANAGER_SERVER_ADDR);
		connectionParams.sin_port = htons(CLIENT_MANAGER_SERVER_PORT);

		/* Connect client socket */
		if (SOCKET_ERROR == connect(*clientSocket, (SOCKADDR*)(&connectionParams), sizeof(connectionParams)))
		{
			printf("Client socket connection failed!\n");
			return CLIENT_ERROR;
		}

		printf("Client successfully connected to the server!\n");
		return CLIENT_OK;
	}

	
	CLIENT_MANAGER_RESULT ClientManagerSetSocket()
	{
		BOOL bOptVal = TRUE;
		struct timeval RxTimeout;
		RxTimeout.tv_sec = 60;

		/* Create client socket */
		/* todo: error check can be implemented */
		ClientManagerCreate(&clientSocketFd);

		/* Connect to the server */
		/* todo: error check can be implemented */
		ClientManagerConnect(&clientSocketFd);

		/* Set Send() and Recv() internal buffer sizes and rx timeout */
		if (SOCKET_ERROR == setsockopt(clientSocketFd, SOL_SOCKET, SO_SNDBUF, (char *)&bOptVal, sizeof(BOOL)))
		{
			printf("Send buffer size set failed!\n");
		}
		if (SOCKET_ERROR == setsockopt(clientSocketFd, SOL_SOCKET, SO_RCVBUF, (char *)&bOptVal, sizeof(BOOL)))
		{
			printf("Receive buffer size set failed!\n");
		}
		if (SOCKET_ERROR == setsockopt(clientSocketFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&RxTimeout, sizeof(BOOL)))
		{
			printf("Rx timeout duration set failed!\n");
		}

		return CLIENT_OK;
	}

	CLIENT_MANAGER_RESULT ClientManagerCommunicate()
	{
		static char localTxBuffer[RQ_MESSAGESIZE_MAX];
		static char localRxBuffer[RQ_MESSAGESIZE_MAX];

		/* Pre-set request message */
		memcpy(&localTxBuffer, RQ_SRV_MEMORY_TYPE, strlen(RQ_SRV_MEMORY_TYPE));

		/* Request memory type from the server device with the pre-defined request messages */
		if (SOCKET_ERROR == send(clientSocketFd, localTxBuffer, sizeof(localTxBuffer), 0))
		{
			printf("Request send failed!\n");
			return CLIENT_ERROR;
		}

		/* Awaiting server response for the memory type */
		if (SOCKET_ERROR == recv(clientSocketFd, localRxBuffer, sizeof(localRxBuffer), 0))
		{
			printf("Requet get response failed!\n");
			return CLIENT_ERROR;
		}

		/* Client got the message, process it */
		if (0 == memcmp(localRxBuffer, RP_SRV_MEMORY_TYPE_RAM, strlen(RP_SRV_MEMORY_TYPE_RAM)))
		{
			printf("Memory type known: %s\n", RP_SRV_MEMORY_TYPE_RAM);
			return CLIENT_OK;
		}
		else if (0 == memcmp(localRxBuffer, RP_SRV_MEMORY_TYPE_FLSH, strlen(RP_SRV_MEMORY_TYPE_FLSH)))
		{
			printf("Memory type known: %s\n", RP_SRV_MEMORY_TYPE_FLSH);
			return CLIENT_OK;
		}
		else
		{
			printf("Memory type unknown: %s\n", localRxBuffer);
			return CLIENT_ERROR;
		}
	}

public:
	Client()
	{
		printf("Activating the client manager!\n");
	}
	~Client()
	{
		printf("Deactivating the client manager!\n");
	}

	static Client* ClientManagerInit()
	{
		if (ClientManager == NULL)
		{
			ClientManager = new Client();
		}

		return ClientManager;
	}
	Client(const Client& obj) = delete;

	/* Initialize the client manager in the first call and communicate with the server */
	void ClientManagerRun(void)
	{
		if (FALSE == clientManagerInit)
		{
			/* todo: error check can be implemented */
			ClientManagerSetSocket();
			clientManagerInit = TRUE;
		}

		ClientManagerCommunicate();
	}

};

// Null initialization of manager object
Client* Client::ClientManager = NULL;