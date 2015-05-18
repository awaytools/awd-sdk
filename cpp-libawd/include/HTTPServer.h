/*************************************************************************
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright [2015] Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of Adobe Systems Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to Adobe Systems Incorporated and its
* suppliers and are protected by all applicable intellectual 
* property laws, including trade secret and copyright laws.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
**************************************************************************/

/**
 * @file  HTTPServer.h
 *
 * @brief This file contains the implementation for a HTTP server using Mongoose.
 */

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#ifndef USE_HTTP_SERVER
#define USE_HTTP_SERVER
#endif
#ifdef USE_HTTP_SERVER

    #ifdef _WINDOWS
        #define CLOSE_SOCKET(sock) closesocket(sock)
    #else
        #ifdef _MAC
            #define CLOSE_SOCKET(sock) close(sock)
        #endif
    #endif

#ifdef _WINDOWS
    #include <WinSock.h>
    #include "Windows.h"
    #include "ShellApi.h"
#endif

#ifdef __APPLE__
    #include "CoreFoundation/CoreFoundation.h"
    #include <dlfcn.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <copyfile.h>
#endif
#include <string>
#ifdef _WINDOWS
#include <xmemory>
#endif
#include "mongoose.h"

/* -------------------------------------------------- Forward Decl */


/* -------------------------------------------------- Enums */


/* -------------------------------------------------- Macros / Constants */


/* -------------------------------------------------- Structs / Unions */

struct ServerConfigParam
{
    std::string root;
    int port;
};

/* -------------------------------------------------- Class Decl */

#define MAX_RETRY_ATTEMPT               10
namespace AWD
{
    class HTTPServer
    {
    public:
        static HTTPServer* GetInstance();
        virtual ~HTTPServer();

        int Start();
        void Stop();

        void SetConfig(const ServerConfigParam& config);
		int GetUnusedLocalPort();

    private:
        HTTPServer();

        static int BeginRequestHandler(struct mg_connection *conn);
		void InitSockAddr(sockaddr_in* sockAddr);
    private:
        static std::auto_ptr<HTTPServer> m_instance;
        ServerConfigParam m_config;
        struct mg_context *m_context;
    };

}

#endif // USE_HTTP_SERVER

#endif // HTTP_SERVER_H_
