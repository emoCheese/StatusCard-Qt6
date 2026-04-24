#pragma once

#include <QString>

inline QString statusCardDefaultStyleSheet()
{
    return QStringLiteral(R"(
StatusCard {
    background-color: #2D2D2D;
    border: 1px solid #444444;
    border-radius: 8px;
    padding: 12px;
}

StatusCard[level="normal"] {
    border-color: #4CAF50;
}

StatusCard[level="warning"] {
    border-color: #FFD700;
}

StatusCard[level="error"] {
    border-color: #FF4444;
}

StatusCard[level="unknown"] {
    border-color: #888888;
}

StatusCard .card-title {
    color: #FFFFFF;
    font-size: 14px;
    font-weight: bold;
    padding-bottom: 8px;
}

StatusCard .field-label {
    color: #AAAAAA;
    font-size: 12px;
}

StatusCard .status-value {
    font-size: 12px;
    font-weight: bold;
}

StatusCard[level="normal"] .status-value {
    color: #FFFFFF;
}

StatusCard[level="warning"] .status-value {
    color: #FFD700;
}

StatusCard[level="error"] .status-value {
    color: #FF4444;
}

StatusCard[level="unknown"] .status-value {
    color: #888888;
}

StatusCardGrid {
    background-color: #1E1E1E;
}
)");
}
