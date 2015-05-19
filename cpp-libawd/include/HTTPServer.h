
#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

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
#endif // HTTP_SERVER_H_
