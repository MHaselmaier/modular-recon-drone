#include "clienthandle.h"

ClientHandle::ClientHandle(QObject *parent)
    :
      QObject(parent),
      m_socket(this),
      m_tcp_socket(this)
{
    connect(&m_tcp_socket, SIGNAL(readyRead()), this, SLOT(readyReadCamera()));
    m_tcp_socket.connectToHost(m_drone_ip, m_drone_tcp_port);
}

void ClientHandle::sendData(char motorADir, char motorASpeed, char motorBDir, char motorBSpeed)
{
    qDebug() << "Sending motor data MotorA[speed:" << motorASpeed << ",direction:" << motorADir;
    qDebug() << "Sending motor data MotorB[speed:" << motorBSpeed << ",direction:" << motorBDir;
    char data[2];
    data[0] = static_cast<char>((motorADir << 7) | motorASpeed);
    data[1] = static_cast<char>((motorBDir << 7) | motorBSpeed);
    m_socket.writeDatagram(data, 2, QHostAddress::Broadcast, m_drone_udp_port);
}

void ClientHandle::readyReadCamera(){
    int dataSize = m_image_width * m_image_height;
    if(m_tcp_socket.bytesAvailable() < dataSize)
        return;

    m_data = m_tcp_socket.read(dataSize);
    qDebug() << "Received camera data: " << m_data.length();
    emit onCameraDataChanged(m_data);
}

int ClientHandle::imageWidth() const{
    return m_image_width;
}

int ClientHandle::imageHeight() const{
    return m_image_height;
}
