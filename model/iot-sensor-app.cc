
#include "iot-sensor-app.h"

#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/inet-socket-address.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("IoTSensorApp");
NS_OBJECT_ENSURE_REGISTERED(IoTSensorApp);

// Registracija nase aplikacije u ns-3
TypeId
IoTSensorApp::GetTypeId()
{
    static TypeId tid = TypeId("ns3::IoTSensorApp")
        .SetParent<Application>()
        .SetGroupName("Applications")
        .AddConstructor<IoTSensorApp>();

    return tid;
}

// Konstruktor
IoTSensorApp::IoTSensorApp()
    : m_socket(nullptr),
      m_serverPort(0),
      m_interval(Seconds(1.0)),
      m_packetSize(64),
      m_sensorId(0),
      m_sequence(0),
      m_running(false)
{
}

IoTSensorApp::~IoTSensorApp()
{
}

// Podesavanje parametara senzora
void
IoTSensorApp::Setup(Ipv4Address serverAddress,
                    uint16_t serverPort,
                    Time interval,
                    uint32_t packetSize,
                    uint32_t sensorId)
{
    m_serverAddress = serverAddress;
    m_serverPort = serverPort;
    m_interval = interval;
    m_packetSize = packetSize;
    m_sensorId = sensorId;
}

// Pokretanje aplikacije
void
IoTSensorApp::StartApplication()
{
    m_running = true;
    m_sequence = 0;

    if (!m_socket)
    {
        m_socket = Socket::CreateSocket(
            GetNode(),
            UdpSocketFactory::GetTypeId()
        );

        if (m_socket->Bind() == -1)
        {
            NS_FATAL_ERROR("IoT sensor: UDP Bind failed");
        }

        if (m_socket->Connect(
                InetSocketAddress(m_serverAddress,
                                  m_serverPort)) == -1)
        {
            NS_FATAL_ERROR("IoT sensor: UDP Connect failed");
        }
    }

    // Zakazujemo slanje prvog paketa
    m_sendEvent = Simulator::ScheduleNow(
        &IoTSensorApp::SendPacket, this
    );
}

// Zaustavljanje aplikacije
void
IoTSensorApp::StopApplication()
{
    m_running = false;

    if (m_sendEvent.IsPending())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
        m_socket = nullptr;
    }
}

// Slanje jednog UDP paketa
void
IoTSensorApp::SendPacket()
{
    if (!m_running || !m_socket)
    {
        return;
    }

    Ptr<Packet> packet = Create<Packet>(m_packetSize);

    int sent = m_socket->Send(packet);

    if (sent >= 0)
    {
        NS_LOG_INFO(
            "Time: " << Simulator::Now().GetSeconds()
            << " s | Sensor: " << m_sensorId
            << " | Sequence: " << m_sequence
            << " | Sent: " << sent << " bytes"
        );
    }
    else
    {
        NS_LOG_WARN(
            "Sensor " << m_sensorId
            << " failed to send packet "
            << m_sequence
        );
    }

    m_sequence++;

    ScheduleNextPacket();
}

// Zakazivanje sljedeceg paketa
void
IoTSensorApp::ScheduleNextPacket()
{
    if (m_running && m_interval.IsStrictlyPositive())
    {
        m_sendEvent = Simulator::Schedule(
            m_interval,
            &IoTSensorApp::SendPacket,
            this
        );
    }
}

} // namespace ns3
