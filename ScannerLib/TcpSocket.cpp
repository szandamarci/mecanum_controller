#include "TcpSocket.h"


using namespace Leuze;


//-------------------------------------------------------------------
bool TcpSocket::s_bWsaInitialized = false;
//-------------------------------------------------------------------
int TcpSocket::s_nInstanceCount = 0;
//-------------------------------------------------------------------
TcpSocket::TcpSocket(void)
{
    ++s_nInstanceCount;
    m_socket = INVALID_SOCKET;
    m_sockAddrHost.sin_family = AF_INET;

#ifdef WIN32
    //initialize ethernet (necessary only first time)
    if(!s_bWsaInitialized)
    {
        //we want version 2.2
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        int iErr = WSAStartup(wVersionRequested, &wsaData);
        if(iErr)
        {
            MessageBox(NULL, "Could not find usable WinSock DLL", "TcpSocket", MB_OK);
        }
        else
        {
            //is version 2.2 supported?
            if( (2 != LOBYTE(wsaData.wVersion)) ||
                    (2 != HIBYTE(wsaData.wVersion)) )
            {
                MessageBox(NULL, "WinSock DLL does not support Version 2.2", "TcpSocket", MB_OK);
                WSACleanup( );
            }
            else
            {
                s_bWsaInitialized = true;
            }
        }
    }
#endif
}
//-------------------------------------------------------------------
TcpSocket::~TcpSocket(void)
{
    --s_nInstanceCount;

#ifdef WIN32
    //if last instance clean up ethernet
    if( (0 == s_nInstanceCount) &&
            (s_bWsaInitialized) )
    {
        WSACleanup();
        s_bWsaInitialized = false;
    }
#endif
}
//-------------------------------------------------------------------
void TcpSocket::setHostIp(const char* zIp)
{
    m_sockAddrHost.sin_addr.s_addr = inet_addr(zIp);
}
//-------------------------------------------------------------------
void TcpSocket::setHostPort(int iPort)
{
    m_sockAddrHost.sin_port = htons(iPort);
}
//-------------------------------------------------------------------
void TcpSocket::connect(void)
{
    int iErr = 0;

    if(INVALID_SOCKET != m_socket)
    {
        close();
    }

    //stream oriented socket with TCP as protocol
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == m_socket)
    {
        perror("Could not create socket!");
#ifdef WIN32
        MessageBox(NULL, "Could not create socket!", "TcpSocket", MB_OK);
#endif
        m_socket = INVALID_SOCKET;
        close();
        return;
    }

    //do not linger on close
    linger Linger;
    Linger.l_onoff = 0;
    Linger.l_linger = 0;
    iErr = setsockopt(m_socket, SOL_SOCKET, SO_LINGER, &Linger, sizeof(Linger));
    if(iErr)
    {
        perror("Could not set socket option 'do not linger on close'!");
#ifdef WIN32
        MessageBox(NULL, "Could not set socket option 'do not linger on close'!", "TcpSocket", MB_OK);
#endif
    }

    //make socket nonblocking
#ifdef WIN32
    u_long uArg = 1;
    iErr = ioctlsocket(m_socket, FIONBIO, &uArg);
    if(iErr)
    {
        perror("Could not set socket to nonblocking mode!");
        MessageBox(NULL, "Could not set socket to nonblocking mode!", "TcpSocket", MB_OK);
        close();
        return;
    }
#endif

#ifdef unix
    int flags = fcntl(m_socket, F_GETFL, 0);              // Get socket flags
    iErr = fcntl(m_socket, F_SETFL, flags | O_NONBLOCK);   // Add non-blocking flag
    if(iErr)
    {
        perror("Could not set socket to nonblocking mode!");
        close();
        return;
    }
#endif

    //use any local addres and any port number
    sockaddr_in sockAddrLocal;
    sockAddrLocal.sin_family = AF_INET;
    sockAddrLocal.sin_addr.s_addr = INADDR_ANY;
    sockAddrLocal.sin_port = 0;

    iErr = bind(m_socket, (sockaddr *) &sockAddrLocal, sizeof(sockAddrLocal));
    if(iErr)
    {
        perror("Could not bind socket to a local address!");
#ifdef WIN32
        MessageBox(NULL, "Could not bind socket to a local address!", "TcpSocket", MB_OK);
#endif
        close();
        return;
    }

    //connect to host
    iErr = ::connect(m_socket, (sockaddr *) &m_sockAddrHost, sizeof(m_sockAddrHost));
    if (iErr)
    {
#ifdef WIN32
        if(WSAEWOULDBLOCK != WSAGetLastError())
        {
            MessageBox(NULL, "Could not connect to host", "TcpSocket", MB_OK);
            close();
            return;
        }
#endif
#ifdef unix
        if(errno != EINPROGRESS)
        {
            perror("Could not connect to host");
            close();
            return;
        }
#endif

        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(m_socket, &writeSet);

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 50000;
        if(1 != select(m_socket + 1, NULL, &writeSet, NULL, &timeout))
        {
            perror("Connect to host failed");
#ifdef WIN32
            MessageBox(NULL, "Connect to host failed", "TcpSocket", MB_OK);
#endif
            close();
            return;
        }

        if(!FD_ISSET(m_socket, &writeSet))
        {
            perror("Connect to host failed");
#ifdef WIN32
            MessageBox(NULL, "Connect to host failed", "TcpSocket", MB_OK);
#endif
            close();
            return;
        }
    }
}
//-------------------------------------------------------------------
bool TcpSocket::isConnected(void)
{
    return (INVALID_SOCKET != m_socket) ? true : false;
}
//-------------------------------------------------------------------
void TcpSocket::close(void)
{
    if(INVALID_SOCKET != m_socket)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}
//-------------------------------------------------------------------
int TcpSocket::send(const char *pcBuffer, int iLen)
{
    if(INVALID_SOCKET == m_socket)
    {
        return 0;
    }

    int iRet = ::send(m_socket, pcBuffer, iLen, 0);

    if(iRet < 1)
    {
#ifdef WIN32
        if(WSAEWOULDBLOCK == WSAGetLastError())
        {
            //currently we cannot send
            return 0;
        }
#endif
#ifdef unix
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            //currently we cannot send
            return 0;
        }
#endif
        //a 'real' error
        close();
        return -1;
    }

    return iRet;
}
//-------------------------------------------------------------------
int TcpSocket::recv(char *pcBuffer, int iLen)
{
    if(INVALID_SOCKET == m_socket)
    {
        return 0;
    }

    int iRet = ::recv(m_socket, pcBuffer, iLen, 0);
    if(0 == iRet)
    {
        close();
        return 0;
    }
    else if(iRet < 0)
    {
#ifdef WIN32
        if(WSAEWOULDBLOCK == WSAGetLastError())
        {
            //currently no data available
            return 0;
        }
#endif
#ifdef unix
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            //currently no data available
            return 0;
        }
#endif
        //a 'real' error
        close();
        return -1;
    }

    return iRet;
}
//-------------------------------------------------------------------
