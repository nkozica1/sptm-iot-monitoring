
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/iot-sensor-app.h"

#include <iostream>

using namespace ns3;

int main(int argc, char* argv[])
{
    // Kreiranje senzora i servera
    NodeContainer nodes;
    nodes.Create(2);

    // Point-to-Point veza
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices = p2p.Install(nodes);

    // TCP/IP stek
    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // UDP server na portu 9000
    uint16_t port = 9000;

    PacketSinkHelper sinkHelper(
        "ns3::UdpSocketFactory",
        InetSocketAddress(Ipv4Address::GetAny(), port)
    );

    ApplicationContainer sinkApps = sinkHelper.Install(nodes.Get(1));
    sinkApps.Start(Seconds(0.5));
    sinkApps.Stop(Seconds(6.0));

    // Nasa vlastita IoT senzorska aplikacija
    Ptr<IoTSensorApp> sensor = CreateObject<IoTSensorApp>();

    sensor->Setup(
        interfaces.GetAddress(1),
        port,
        Seconds(1.0),
        64,
        1
    );

    nodes.Get(0)->AddApplication(sensor);

    sensor->SetStartTime(Seconds(1.0));
    sensor->SetStopTime(Seconds(5.5));

    // Pokretanje simulacije
    Simulator::Stop(Seconds(6.0));
    Simulator::Run();

    Ptr<PacketSink> sink =
        DynamicCast<PacketSink>(sinkApps.Get(0));

    std::cout << "Primljeno bajtova: "
              << sink->GetTotalRx()
              << std::endl;

    std::cout << "Ocekivano: 320 bajtova (5 x 64)"
              << std::endl;

    Simulator::Destroy();

    return 0;
}
