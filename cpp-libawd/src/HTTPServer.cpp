#include "HTTPServer.h"
#include "Utils.h"
#include "utils/util.h"

#include <time.h>
namespace AWD
{

    /* -------------------------------------------------- Constants */

    /* -------------------------------------------------- Static Functions */

    /* -------------------------------------------------- HTTPServer */

    std::auto_ptr<HTTPServer> HTTPServer::m_instance;

    HTTPServer::HTTPServer() : m_context(NULL)
    {
    }

    HTTPServer::~HTTPServer()
    {
        Stop();
    }

    HTTPServer* HTTPServer::GetInstance()
    {
        if (!m_instance.get())
        {
            m_instance.reset(new HTTPServer());
        }

        return m_instance.get();
    }

    void HTTPServer::SetConfig(const ServerConfigParam& config)
    {
        m_config = config;
    }

    int HTTPServer::Start()
    {
        FCM::Result res = FCM_GENERAL_ERROR;
        std::string portStr;
        mg_callbacks callbacks;

		portStr = FILES::int_to_string(m_config.port);

        const char *options[] = {
            "document_root", m_config.root.c_str(),
            "listening_ports", portStr.c_str(),
            NULL
        };

        memset(&callbacks, 0, sizeof(mg_callbacks));
        callbacks.begin_request = BeginRequestHandler;

        // Start the server
        m_context = mg_start(&callbacks, NULL, options);

        if (m_context)
        {
            res = FCM_SUCCESS;
        }

        return res;
    }

    void HTTPServer::Stop()
    {
        if (m_context)
        {
            // Stop the server
            mg_stop(m_context);
            m_context = NULL;
        }
    }

    int HTTPServer::BeginRequestHandler(struct mg_connection *conn) 
    {
        // Inform Mongoose that it needs to handle the request
        return 0;
    }
	
    void HTTPServer::InitSockAddr(sockaddr_in* sockAddr)
    {
        ASSERT(sockAddr);

        memset(sockAddr, 0, sizeof(struct sockaddr_in));
        sockAddr->sin_family = AF_INET;
        char *ipAddressStr = (char*)"127.0.0.1";

#ifdef _MAC
        sockAddr->sin_addr.s_addr = inet_addr(ipAddressStr);
#endif
        
#ifdef _WINDOWS
        sockAddr->sin_addr.S_un.S_addr = inet_addr(ipAddressStr);
#endif

    }
	int HTTPServer::GetUnusedLocalPort()
    {
        sockaddr_in client;

        InitSockAddr(&client);

        SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
        
        // Look for a port in the private port range
        int minPortNumber = 49152;
        int maxPortNumber = 65535;
        int defaultPortNumber = 50000;
        
        int port = defaultPortNumber;
        
        int nTries = 0;
        int maxTries = 10;
        srand ((int)time(NULL));
        // Try connect
        while (nTries++ <= maxTries) 
        {
            client.sin_port = htons(port);

            int result = connect(sock, (struct sockaddr *) &client, sizeof(client));

            CLOSE_SOCKET(sock);

            // Connect unsuccessful, port is available.
            if (result != 0) 
            {
                break;  
            }

            // Retry at a random port number in the valid range
            port = minPortNumber + rand() % (maxPortNumber - minPortNumber - 1);
        }
        
        if (nTries > maxTries) 
        {
            port = -1;
        }
        
        return port;
    }

};

