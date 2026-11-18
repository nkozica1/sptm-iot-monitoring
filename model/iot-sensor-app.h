#ifndef IOT_SENSOR_APP_H
#define IOT_SENSOR_APP_H

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/event-id.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"

#include <cstdint>

namespace ns3
{

class IoTSensorApp : public Application
{
public:
    static TypeId GetTypeId();

    IoTSensorApp();
    ~IoTSensorApp() override;

    void Setup(Ipv4Address serverAddress,
               uint16_t serverPort,
               Time interval,
               uint32_t packetSize,
               uint32_t sensorId);

private:
    void StartApplication() override;
    void StopApplication() override;

    void SendPacket();
    void ScheduleNextPacket();

    Ptr<Socket> m_socket;
    EventId m_sendEvent;

    Ipv4Address m_serverAddress;
    uint16_t m_serverPort;

    Time m_interval;
    uint32_t m_packetSize;
    uint32_t m_sensorId;
    uint32_t m_sequence;

    bool m_running;
};

} // namespace ns3

#endif
