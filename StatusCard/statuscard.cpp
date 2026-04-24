#include "statuscard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QThread>
#include <QMetaObject>
#include <QPalette>
#include <QFont>

StatusCard::StatusCard(QWidget* parent)
    : QWidget(parent)
    , m_mapper(std::make_unique<StatusMapper>())
{
    setObjectName(QStringLiteral("statusCard"));
    setProperty("class", QStringLiteral("status-card"));
    setProperty("level", QStringLiteral("unknown"));
    setAttribute(Qt::WA_StyledBackground, true);

    // 默认暗色背景
    setBackgroundColor(QColor(QStringLiteral("#252526")));

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(6);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName(QStringLiteral("cardTitle"));
    m_titleLabel->setProperty("class", QStringLiteral("card-title"));
    m_titleLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_mainLayout->addWidget(m_titleLabel);

    m_mainLayout->addStretch();

    // 初始化默认字体
    m_titleFont.setPointSize(14);
    m_titleFont.setBold(true);

    m_labelFont.setPointSize(12);

    m_valueFont.setPointSize(12);
    m_valueFont.setBold(true);

    applyTitleFont();
}

StatusCard::~StatusCard() = default;

void StatusCard::updateField(const QString& fieldName, const QVariant& value)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, [this, fieldName, value]() {
            this->updateFieldInternal(fieldName, value);
        }, Qt::QueuedConnection);
        return;
    }
    updateFieldInternal(fieldName, value);
}

void StatusCard::updateFields(const QMap<QString, QVariant>& values)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, [this, values]() {
            for (auto it = values.begin(); it != values.end(); ++it) {
                this->updateFieldInternal(it.key(), it.value());
            }
        }, Qt::QueuedConnection);
        return;
    }
    for (auto it = values.begin(); it != values.end(); ++it) {
        updateFieldInternal(it.key(), it.value());
    }
}

bool StatusCard::validateConfig() const
{
    return m_mapper->validateConfig();
}

void StatusCard::updateFieldInternal(const QString& fieldName, const QVariant& value)
{
    auto it = m_fields.find(fieldName);
    if (it == m_fields.end()) {
        qWarning() << QStringLiteral("StatusCard: field '%1' does not exist.").arg(fieldName);
        return;
    }

    FieldWidget& fw = it.value();
    fw.value->setText(value.toString());

    StatusLevel level = m_mapper->evaluate(fieldName, value);
    fw.currentLevel = level;

    QPalette pal = fw.value->palette();
    pal.setColor(QPalette::WindowText, levelToColor(level));
    fw.value->setPalette(pal);

    updateCardLevel();
}

void StatusCard::updateCardLevel()
{
    StatusLevel worst = StatusLevel::Normal;
    for (const auto& fw : std::as_const(m_fields)) {
        if (static_cast<int>(fw.currentLevel) > static_cast<int>(worst)) {
            worst = fw.currentLevel;
        }
    }
    QString levelStr = levelToString(worst);
    if (property("level").toString() != levelStr) {
        setProperty("level", levelStr);
    }
}

void StatusCard::updateTitle()
{
    if (m_titleLabel) {
        m_titleLabel->setText(QStringLiteral("%1. %2").arg(m_index).arg(m_title));
    }
}

void StatusCard::addFieldWidget(const QString& label, const QString& defaultValue)
{
    QHBoxLayout* row = new QHBoxLayout();
    row->setSpacing(8);

    QLabel* labelWidget = new QLabel(label + QStringLiteral(":"), this);
    labelWidget->setObjectName(QStringLiteral("fieldLabel_%1").arg(label));
    labelWidget->setProperty("class", QStringLiteral("field-label"));
    labelWidget->setFont(m_labelFont);
    labelWidget->setAttribute(Qt::WA_TranslucentBackground);

    QLabel* valueWidget = new QLabel(defaultValue, this);
    valueWidget->setObjectName(QStringLiteral("fieldValue_%1").arg(label));
    valueWidget->setProperty("class", QStringLiteral("status-value"));
    valueWidget->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    valueWidget->setFont(m_valueFont);
    valueWidget->setAttribute(Qt::WA_TranslucentBackground);

    QPalette pal = valueWidget->palette();
    pal.setColor(QPalette::WindowText, levelToColor(StatusLevel::Unknown));
    valueWidget->setPalette(pal);

    row->addWidget(labelWidget);
    row->addStretch();
    row->addWidget(valueWidget);

    m_mainLayout->insertLayout(m_mainLayout->count() - 1, row);

    FieldWidget fw;
    fw.label = labelWidget;
    fw.value = valueWidget;
    fw.currentLevel = StatusLevel::Unknown;
    m_fields.insert(label, fw);
}

void StatusCard::setTitleFont(const QFont& font)
{
    m_titleFont = font;
    applyTitleFont();
}

void StatusCard::setTitlePointSize(int pointSize)
{
    m_titleFont.setPointSize(pointSize);
    applyTitleFont();
}

void StatusCard::setLabelFont(const QFont& font)
{
    m_labelFont = font;
    applyLabelFont();
}

void StatusCard::setLabelPointSize(int pointSize)
{
    m_labelFont.setPointSize(pointSize);
    applyLabelFont();
}

void StatusCard::setValueFont(const QFont& font)
{
    m_valueFont = font;
    applyValueFont();
}

void StatusCard::setValuePointSize(int pointSize)
{
    m_valueFont.setPointSize(pointSize);
    applyValueFont();
}

void StatusCard::setBackgroundColor(const QColor& color)
{
    m_backgroundColor = color;
    QPalette pal = palette();
    pal.setColor(QPalette::Window, color);
    setPalette(pal);
    setAutoFillBackground(true);
}

void StatusCard::applyTitleFont()
{
    if (m_titleLabel)
        m_titleLabel->setFont(m_titleFont);
}

void StatusCard::applyLabelFont()
{
    for (const auto& fw : std::as_const(m_fields)) {
        if (fw.label)
            fw.label->setFont(m_labelFont);
    }
}

void StatusCard::applyValueFont()
{
    for (const auto& fw : std::as_const(m_fields)) {
        if (fw.value)
            fw.value->setFont(m_valueFont);
    }
}

QColor StatusCard::levelToColor(StatusLevel level)
{
    switch (level) {
    case StatusLevel::Normal:
        return QColor(QStringLiteral("#FFFFFF"));
    case StatusLevel::Warning:
        return QColor(QStringLiteral("#FFD700"));
    case StatusLevel::Error:
        return QColor(QStringLiteral("#FF4444"));
    default:
        return QColor(QStringLiteral("#888888"));
    }
}

QString StatusCard::levelToString(StatusLevel level)
{
    switch (level) {
    case StatusLevel::Normal:
        return QStringLiteral("normal");
    case StatusLevel::Warning:
        return QStringLiteral("warning");
    case StatusLevel::Error:
        return QStringLiteral("error");
    default:
        return QStringLiteral("unknown");
    }
}
