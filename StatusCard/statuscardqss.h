#pragma once

#include <QString>

inline QString statusCardDefaultStyleSheet()
{
    return QStringLiteral(R"(
/* ========== 暗色主题 ========== */

/* 网格容器背景 */
StatusCardGrid {
    background-color: #1E1E1E;
}

/* 所有 QLabel 背景透明，避免与父窗口背景不一致 */
StatusCard QLabel {
    background-color: transparent;
    border: none;
}

/* 标题 */
StatusCard .card-title {
    color: #E0E0E0;
    padding-bottom: 6px;
}

/* 字段标签 */
StatusCard .field-label {
    color: #9E9E9E;
}

/* 状态值由 QPalette 控制颜色，此处仅保留占位 */
StatusCard .status-value {
}
)");
}
