#include "statuscard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QThread>
#include <QMetaObject>

StatusCard::StatusCard(QWidget* parent)
    : QWidget(parent)
    , m_mapper(std::make_unique<StatusMapper>())
{
    setObjectName(QStringLiteral("statusCard"));
    setProperty("class", QStringLiteral("status-card"));
    setProperty("level", QStringLiteral("unknown"));

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(6);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName(QStringLiteral("cardTitle"));
    m_titleLabel->setProperty("class", QStringLiteral("card-title"));
    m_mainLayout->addWidget(m_titleLabel);

    m_mainLayout->addStretch();
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
    fw.value->setProperty("level", levelToString(level));

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

    QLabel* valueWidget = new QLabel(defaultValue, this);
    valueWidget->setObjectName(QStringLiteral("fieldValue_%1").arg(label));
    valueWidget->setProperty("class", QStringLiteral("status-value"));
    valueWidget->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

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
