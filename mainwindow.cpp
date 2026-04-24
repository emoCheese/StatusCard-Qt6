#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "StatusCard/statuscardgrid.h"
#include "StatusCard/statuscardqss.h"
#include <QTimer>
#include <QRandomGenerator>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qApp->setStyleSheet(statusCardDefaultStyleSheet());

    m_grid = new StatusCardGrid(this);
    m_grid->setGridSize(2, 3);
    setCentralWidget(m_grid);

    // Card 1: 主控状态
    m_grid->addCard(0, 0, StatusCard::create()
        .title(QStringLiteral("主控状态"))
        .index(1)
        .addField(QStringLiteral("驾舱CAN"), QStringLiteral("初始化中..."))
        .addField(QStringLiteral("网络传输"), QStringLiteral("初始化中..."))
        .mapRange(QStringLiteral("驾舱CAN"), 0, 50, StatusCard::Normal)
        .mapRange(QStringLiteral("驾舱CAN"), 51, 100, StatusCard::Warning)
        .mapValue(QStringLiteral("网络传输"), QStringLiteral("OK"), StatusCard::Normal)
        .mapValue(QStringLiteral("网络传输"), QStringLiteral("ERR"), StatusCard::Error)
    );

    // Card 2: 动力系统
    m_grid->addCard(0, 1, StatusCard::create()
        .title(QStringLiteral("动力系统"))
        .index(2)
        .addField(QStringLiteral("电池电压"), QStringLiteral("0V"))
        .addField(QStringLiteral("电机温度"), QStringLiteral("0°C"))
        .mapRange(QStringLiteral("电池电压"), 48, 60, StatusCard::Normal)
        .mapRange(QStringLiteral("电池电压"), 40, 47, StatusCard::Warning)
        .mapRange(QStringLiteral("电池电压"), 0, 39, StatusCard::Error)
        .mapRange(QStringLiteral("电机温度"), 0, 80, StatusCard::Normal)
        .mapRange(QStringLiteral("电机温度"), 81, 100, StatusCard::Warning)
        .mapRange(QStringLiteral("电机温度"), 101, 150, StatusCard::Error)
    );

    // Card 3: 环境感知
    m_grid->addCard(0, 2, StatusCard::create()
        .title(QStringLiteral("环境感知"))
        .index(3)
        .addField(QStringLiteral("前向雷达"), QStringLiteral("待机"))
        .addField(QStringLiteral("摄像头"), QStringLiteral("待机"))
        .mapValue(QStringLiteral("前向雷达"), QStringLiteral("正常"), StatusCard::Normal)
        .mapValue(QStringLiteral("前向雷达"), QStringLiteral("遮挡"), StatusCard::Warning)
        .mapValue(QStringLiteral("前向雷达"), QStringLiteral("故障"), StatusCard::Error)
        .mapValue(QStringLiteral("摄像头"), QStringLiteral("正常"), StatusCard::Normal)
        .mapValue(QStringLiteral("摄像头"), QStringLiteral("模糊"), StatusCard::Warning)
        .mapValue(QStringLiteral("摄像头"), QStringLiteral("离线"), StatusCard::Error)
    );

    // Card 4: 通信链路
    m_grid->addCard(1, 0, StatusCard::create()
        .title(QStringLiteral("通信链路"))
        .index(4)
        .addField(QStringLiteral("4G信号"), QStringLiteral("0%"))
        .addField(QStringLiteral("WiFi"), QStringLiteral("断开"))
        .mapRange(QStringLiteral("4G信号"), 80, 100, StatusCard::Normal)
        .mapRange(QStringLiteral("4G信号"), 50, 79, StatusCard::Warning)
        .mapRange(QStringLiteral("4G信号"), 0, 49, StatusCard::Error)
        .mapValue(QStringLiteral("WiFi"), QStringLiteral("连接"), StatusCard::Normal)
        .mapValue(QStringLiteral("WiFi"), QStringLiteral("断开"), StatusCard::Error)
    );

    // Card 5: 底盘控制
    m_grid->addCard(1, 1, StatusCard::create()
        .title(QStringLiteral("底盘控制"))
        .index(5)
        .addField(QStringLiteral("转向角"), QStringLiteral("0°"))
        .addField(QStringLiteral("制动压力"), QStringLiteral("0 bar"))
        .mapRange(QStringLiteral("转向角"), -30, 30, StatusCard::Normal)
        .mapRange(QStringLiteral("转向角"), -45, -31, StatusCard::Warning)
        .mapRange(QStringLiteral("转向角"), 31, 45, StatusCard::Warning)
        .mapRange(QStringLiteral("制动压力"), 5, 15, StatusCard::Normal)
        .mapRange(QStringLiteral("制动压力"), 0, 4, StatusCard::Warning)
        .mapRange(QStringLiteral("制动压力"), 16, 20, StatusCard::Warning)
    );

    // Card 6: 安全系统
    m_grid->addCard(1, 2, StatusCard::create()
        .title(QStringLiteral("安全系统"))
        .index(6)
        .addField(QStringLiteral("急停按钮"), QStringLiteral("释放"))
        .addField(QStringLiteral("安全带"), QStringLiteral("未系"))
        .mapValue(QStringLiteral("急停按钮"), QStringLiteral("释放"), StatusCard::Normal)
        .mapValue(QStringLiteral("急停按钮"), QStringLiteral("按下"), StatusCard::Error)
        .mapValue(QStringLiteral("安全带"), QStringLiteral("已系"), StatusCard::Normal)
        .mapValue(QStringLiteral("安全带"), QStringLiteral("未系"), StatusCard::Warning)
    );

    // 模拟工作线程秒级刷新数据
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (auto* card1 = m_grid->cardAt(0, 0)) {
            int canVal = QRandomGenerator::global()->bounded(101);
            card1->updateField(QStringLiteral("驾舱CAN"), canVal);
            QString net = (QRandomGenerator::global()->bounded(2) == 0)
                              ? QStringLiteral("OK")
                              : QStringLiteral("ERR");
            card1->updateField(QStringLiteral("网络传输"), net);
        }

        if (auto* card2 = m_grid->cardAt(0, 1)) {
            int voltage = QRandomGenerator::global()->bounded(30, 65);
            card2->updateField(QStringLiteral("电池电压"), QStringLiteral("%1V").arg(voltage));
            int temp = QRandomGenerator::global()->bounded(150);
            card2->updateField(QStringLiteral("电机温度"), QStringLiteral("%1°C").arg(temp));
        }

        if (auto* card3 = m_grid->cardAt(0, 2)) {
            static const QStringList radarStates = {
                QStringLiteral("正常"), QStringLiteral("遮挡"), QStringLiteral("故障")
            };
            static const QStringList camStates = {
                QStringLiteral("正常"), QStringLiteral("模糊"), QStringLiteral("离线")
            };
            card3->updateField(QStringLiteral("前向雷达"),
                               radarStates[QRandomGenerator::global()->bounded(radarStates.size())]);
            card3->updateField(QStringLiteral("摄像头"),
                               camStates[QRandomGenerator::global()->bounded(camStates.size())]);
        }

        if (auto* card4 = m_grid->cardAt(1, 0)) {
            int signal = QRandomGenerator::global()->bounded(101);
            card4->updateField(QStringLiteral("4G信号"), QStringLiteral("%1%").arg(signal));
            QString wifi = (QRandomGenerator::global()->bounded(2) == 0)
                               ? QStringLiteral("连接")
                               : QStringLiteral("断开");
            card4->updateField(QStringLiteral("WiFi"), wifi);
        }

        if (auto* card5 = m_grid->cardAt(1, 1)) {
            int angle = QRandomGenerator::global()->bounded(-45, 46);
            card5->updateField(QStringLiteral("转向角"), QStringLiteral("%1°").arg(angle));
            int pressure = QRandomGenerator::global()->bounded(0, 21);
            card5->updateField(QStringLiteral("制动压力"), QStringLiteral("%1 bar").arg(pressure));
        }

        if (auto* card6 = m_grid->cardAt(1, 2)) {
            QString eStop = (QRandomGenerator::global()->bounded(10) == 0)
                                ? QStringLiteral("按下")
                                : QStringLiteral("释放");
            card6->updateField(QStringLiteral("急停按钮"), eStop);
            QString belt = (QRandomGenerator::global()->bounded(3) == 0)
                               ? QStringLiteral("未系")
                               : QStringLiteral("已系");
            card6->updateField(QStringLiteral("安全带"), belt);
        }
    });
    timer->start(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}
