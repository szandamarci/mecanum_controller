#include "Scanner.h"
#include <iostream>

using namespace std;

Scanner::Scanner() {
    pConnection = nullptr;
    s_pBinaryProtocol = nullptr;
    measurementEnabled = false;
}

Scanner::~Scanner() {
    if (pConnection != nullptr)
        delete pConnection;

    if (s_pBinaryProtocol != nullptr)
        delete s_pBinaryProtocol;
}

/**
 * connect function
 * Connect to the laser scanner
 * @return true, if connection was successful
 */
bool Scanner::connect(void)
{
    if (pConnection == nullptr)
    {
        pConnection = new TcpSocket();
        pConnection->setHostIp((const char*)SCANNER_IP);
        pConnection->setHostPort(SCANNER_PORT);
        pConnection->connect();

        if (!pConnection->isConnected())
        {
            // Connection failed
            delete pConnection;
            pConnection = nullptr;
            return false;
        }
        else
        {
            s_pBinaryProtocol = new ROD4BinaryProtocol();
            s_pBinaryProtocol->setConnection(pConnection);

            // Using std::cout instead of ros::cout
            cout << "Connection was successful!" << endl;
        }
    }

    return true;
}

/**
 * disconnect function
 * Disconnect from the laser scanner
 */
void Scanner::disconnect(void)
{
    if (pConnection != nullptr)
    {
        pConnection->close();
        delete pConnection;
        pConnection = nullptr;
    }
}

/**
 * enableMeasurement function
 * Enable or disable measurement in the laser scanner
 * @param enable
 * @return true if enable/disable was successful
 */
bool Scanner::enableMeasurement(bool enable)
{
    if (enable == true)
    {
        if (!s_pBinaryProtocol->enableMeasurement(true))
        {
            cout << "Measurement couldn't be started!" << endl;
            measurementEnabled = false;
        }
        else
        {
            cout << "Measurement enabled!" << endl;
            measurementEnabled = true;
        }
    }
    else
    {
        if (!s_pBinaryProtocol->enableMeasurement(false))
        {
            cout << "Measurement couldn't be stopped!" << endl;
        }
        else
        {
            cout << "Measurement disabled!" << endl;
            measurementEnabled = false;
        }
    }

    return (measurementEnabled == enable);
}

/**
 * receiveDatagram function
 *
 */
bool Scanner::receiveDatagram(Datagram* pDatagram)
{
    // Check connection
    if (pConnection == nullptr)
    {
        cout << "pConnection is NULL!" << endl;
        return false;
    }
    // Enable measurement
    if (measurementEnabled == false)
    {
        cout << "Measurement is disabled!" << endl;
        return false;
    }

    bool received = s_pBinaryProtocol->receive(pDatagram);

    if(received)
    {
        switch(pDatagram->frame.msg_identifier)
        {
            case Leuze::ROD4::VALID_ALIVE_FRAME:
            {
                // ignore - just an alive message
                // Alive messages are sent occasionally during measure mode and in error/configuration mode
                // and eventually on startup.
                cout << "VALID_ALIVE_FRAME" << endl;
                received = false;
                break;
            }
            case Leuze::ROD4::VALID_DATA_FRAME:
            {
                // Frame is valid
                break;
            }
            case Leuze::ROD4::VALID_ERROR_FRAME:
            {
                // Prints the error message sent by ROD before leaving measure mode and
                // switching to error/configuration state.
                cout << "VALID_ERROR_FRAME" << endl;
                cout << " Error number: " << pDatagram->distances[0] << endl;
                cout << " Error location: " << pDatagram->distances[1] << endl;
                received = false;
                break;
            }
            case Leuze::ROD4::VALID_WARNING_FRAME:
            {
                // Prints a warning message sent by ROD. At the time this is written (Firmware Version 8.3)
                // the warning message is sent only to signal a dirty scanner window.
                cout << "VALID_WARNING_FRAME" << endl;
                cout << " Warning number: " << pDatagram->distances[0] << endl;
                cout << " Warning location: " << pDatagram->distances[1] << endl;
                received = false;
                break;
            }
        }
    }

    return received;
}
