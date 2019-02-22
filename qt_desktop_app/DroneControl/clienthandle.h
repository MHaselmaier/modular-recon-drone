#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QByteArray>

#include <string>

class ClientHandle: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray cameraData MEMBER m_data NOTIFY onCameraDataChanged)
    Q_PROPERTY(int cameraImageWidth READ imageWidth)
    Q_PROPERTY(int cameraImageHeight READ imageHeight)

public:
    explicit ClientHandle(QObject *parent = nullptr);
    Q_INVOKABLE void sendData(char motorADir, char motorASpeed, char motorBDir, char motorBSpeed);

    int imageWidth() const;
    int imageHeight() const;

signals:
    void onCameraDataChanged(const QByteArray& camera_data);

public slots:
    void readyReadCamera();

private:
    QUdpSocket m_socket;
    QTcpSocket m_tcp_socket;
    QByteArray m_data;
    const int m_image_width = 160;
    const int m_image_height = 120;
    const QString m_drone_ip{"192.168.4.1"};
    const quint16 m_drone_tcp_port = 1234;
    const quint16 m_drone_udp_port = 4242;
};
