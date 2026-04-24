# StatusCardKit

基于 **Qt6 QWidget + QSS** 的状态卡片监控组件库，专为远程驾驶舱等秒级刷新监控场景设计。

---

## 目录

- [功能特性](#功能特性)
- [快速开始](#快速开始)
- [API 说明](#api-说明)
  - [StatusCard（状态卡片）](#statuscard状态卡片)
  - [StatusCardGrid（网格容器）](#statuscardgrid网格容器)
  - [StatusMapper（状态映射器）](#statusmapper状态映射器)
- [使用案例](#使用案例)
- [QSS 样式定制](#qss-样式定制)
- [集成到其他项目](#集成到其他项目)
  - [方式一：add_subdirectory](#方式一add_subdirectory)
  - [方式二：find_package](#方式二find_package)
- [线程安全](#线程安全)
- [设计说明](#设计说明)

---

## 功能特性

- **声明式 Builder API**：链式配置卡片标题、字段、状态映射规则
- **QSS 动态属性驱动**：状态颜色完全由 QSS 控制，新增级别无需修改 C++ 绘制逻辑
- **线程安全更新**：`updateField` / `updateFields` 支持从工作线程直接调用，内部自动排队到 UI 线程
- **独立映射逻辑**：`StatusMapper` 与 UI 解耦，便于单元测试
- **CMake 现代集成**：支持 `add_subdirectory` 与 `find_package(StatusCardKit)` 两种接入方式

---

## 快速开始

```cpp
#include "StatusCard/statuscardgrid.h"
#include "StatusCard/statuscardqss.h"

// 1. 应用默认 QSS（或自定义）
qApp->setStyleSheet(statusCardDefaultStyleSheet());

// 2. 创建网格容器
auto* grid = new StatusCardGrid(this);
grid->setGridSize(2, 3);   // 2 行 3 列

// 3. 声明式添加卡片
grid->addCard(0, 0, StatusCard::create()
    .title(QStringLiteral("主控状态"))
    .index(1)
    .addField(QStringLiteral("驾舱CAN"), QStringLiteral("初始化中..."))
    .addField(QStringLiteral("网络传输"), QStringLiteral("初始化中..."))
    .mapRange(QStringLiteral("驾舱CAN"), 0, 50, StatusCard::Normal)
    .mapRange(QStringLiteral("驾舱CAN"), 51, 100, StatusCard::Warning)
    .mapValue(QStringLiteral("网络传输"), QStringLiteral("OK"), StatusCard::Normal)
    .mapValue(QStringLiteral("网络传输"), QStringLiteral("ERR"), StatusCard::Error)
);

// 4. 运行时刷新数据（支持从任意线程调用）
auto* card = grid->cardAt(0, 0);
card->updateField(QStringLiteral("驾舱CAN"), 42);
card->updateField(QStringLiteral("网络传输"), QStringLiteral("OK"));
```

---

## API 说明

### StatusCard（状态卡片）

单张状态卡片，顶部显示 `序号. 标题`，下方垂直排列多组 `标签: 状态值`。

#### 嵌套 Builder（声明式配置）

所有配置必须在卡片 `show()` 前通过 Builder 完成，运行时仅允许更新数据。

| 方法 | 说明 |
|------|------|
| `static Builder create()` | 创建 Builder 实例 |
| `Builder& title(const QString&)` | 设置卡片标题 |
| `Builder& index(int)` | 设置序号 |
| `Builder& addField(const QString& label, const QString& defaultValue)` | 添加监控字段 |
| `Builder& mapRange(const QString& field, int min, int max, StatusLevel)` | 数值范围映射 |
| `Builder& mapValue(const QString& field, const QVariant& value, StatusLevel)` | 离散值映射 |
| `StatusCard* build(QWidget* parent = nullptr) &&` | 构建卡片对象 |

#### 运行时接口

| 方法 | 说明 |
|------|------|
| `void updateField(const QString& fieldName, const QVariant& value)` | 更新单个字段，**线程安全** |
| `void updateFields(const QMap<QString, QVariant>& values)` | 批量更新，**线程安全** |
| `bool validateConfig() const` | 检查字段映射规则是否冲突 |
| `QString title() const` / `int index() const` | 获取标题与序号 |

#### 状态级别

```cpp
StatusCard::Normal    // 正常
StatusCard::Warning   // 警告
StatusCard::Error     // 错误
StatusCard::Unknown   // 未知（默认值）
```

---

### StatusCardGrid（网格容器）

卡片网格布局容器，内部使用 `QGridLayout`，支持窗口拉伸时等比例缩放。

| 方法 | 说明 |
|------|------|
| `void setGridSize(int rows, int cols)` | 设置行列数并均分拉伸 |
| `StatusCard* addCard(int row, int col, StatusCard* card)` | 添加已构建的卡片 |
| `StatusCard* addCard(int row, int col, StatusCard::Builder builder)` | **直接传入 Builder**，内部自动构建 |
| `StatusCard* cardAt(int row, int col) const` | 通过坐标获取卡片 |
| `void setCardClass(const QString& className)` | 设置卡片统一样式类名，便于 QSS 批量控制 |

---

### StatusMapper（状态映射器）

纯数据驱动的映射逻辑，**不依赖 QWidget**，可独立单元测试。

| 方法 | 说明 |
|------|------|
| `void addRangeRule(const QString& field, int min, int max, StatusLevel level)` | 添加数值范围规则 |
| `void addValueRule(const QString& field, const QVariant& value, StatusLevel level)` | 添加离散值规则 |
| `StatusLevel evaluate(const QString& field, const QVariant& value) const` | 评估字段值对应的状态级别 |
| `bool validateConfig() const` | 校验同一字段是否同时存在 range 与 value 映射（冲突时输出 `qWarning`） |

---

## 使用案例

完整示例见 [mainwindow.cpp](mainwindow.cpp)。以下为典型场景提炼：

### 场景 1：数值范围监控（电池电压）

```cpp
grid->addCard(0, 0, StatusCard::create()
    .title(QStringLiteral("动力系统"))
    .index(1)
    .addField(QStringLiteral("电池电压"), QStringLiteral("0V"))
    .mapRange(QStringLiteral("电池电压"), 48, 60, StatusCard::Normal)
    .mapRange(QStringLiteral("电池电压"), 40, 47, StatusCard::Warning)
    .mapRange(QStringLiteral("电池电压"), 0, 39, StatusCard::Error)
);

// 运行时从传感器线程刷新
card->updateField(QStringLiteral("电池电压"), QStringLiteral("52V"));
```

### 场景 2：离散状态监控（网络连接）

```cpp
grid->addCard(0, 1, StatusCard::create()
    .title(QStringLiteral("通信链路"))
    .index(2)
    .addField(QStringLiteral("网络传输"), QStringLiteral("初始化中..."))
    .mapValue(QStringLiteral("网络传输"), QStringLiteral("OK"), StatusCard::Normal)
    .mapValue(QStringLiteral("网络传输"), QStringLiteral("ERR"), StatusCard::Error)
);
```

### 场景 3：批量更新

```cpp
QMap<QString, QVariant> batch;
batch.insert(QStringLiteral("驾舱CAN"), 78);
batch.insert(QStringLiteral("网络传输"), QStringLiteral("OK"));
card->updateFields(batch);
```

---

## QSS 样式定制

状态颜色**不硬编码**，完全由 QSS 动态属性驱动。组件库提供默认样式（见 [statuscardqss.h](StatusCard/statuscardqss.h)），可通过以下方式覆盖：

```cpp
// 全局样式
qApp->setStyleSheet(statusCardDefaultStyleSheet());

// 或针对单个网格
grid->setStyleSheet(yourCustomQss);
```

### 核心选择器

```css
/* 卡片整体 */
StatusCard {
    background-color: #2D2D2D;
    border: 1px solid #444444;
    border-radius: 8px;
}

/* 根据卡片级别改变边框颜色 */
StatusCard[level="normal"]    { border-color: #4CAF50; }
StatusCard[level="warning"]   { border-color: #FFD700; }
StatusCard[level="error"]     { border-color: #FF4444; }
StatusCard[level="unknown"]   { border-color: #888888; }

/* 标题 */
StatusCard .card-title {
    color: #FFFFFF;
    font-size: 14px;
    font-weight: bold;
}

/* 字段标签 */
StatusCard .field-label {
    color: #AAAAAA;
    font-size: 12px;
}

/* 状态值颜色 */
StatusCard[level="normal"]  .status-value { color: #FFFFFF; }
StatusCard[level="warning"] .status-value { color: #FFD700; }
StatusCard[level="error"]   .status-value { color: #FF4444; }
StatusCard[level="unknown"] .status-value { color: #888888; }
```

> **提示**：新增 `StatusLevel` 枚举值后，只需在 QSS 中补充对应 `[level="xxx"]` 选择器即可生效，**无需修改 C++ 代码**。

---

## 集成到其他项目

### 方式一：add_subdirectory

将本仓库作为子模块或拷贝到项目目录下：

```cmake
# 在你的 CMakeLists.txt 中
add_subdirectory(StatusCard)          # 路径指向 StatusCard/ 目录

target_link_libraries(YourTarget
    PRIVATE
        StatusCardKit
)
```

头文件引用：

```cpp
#include "StatusCard/statuscardgrid.h"
#include "StatusCard/statuscardqss.h"
```

> 由于 `target_include_directories` 已配置为 `${CMAKE_CURRENT_SOURCE_DIR}`，父项目可直接通过相对路径包含头文件。

---

### 方式二：find_package

先在本项目构建目录执行安装：

```bash
cd build
sudo cmake --install . --prefix /usr/local
```

安装后会生成：

- 库文件：`/usr/local/lib/libStatusCardKit.so`
- 头文件：`/usr/local/include/StatusCardKit/`
- CMake 配置：`/usr/local/lib/cmake/StatusCardKit/`

然后在你的项目中：

```cmake
find_package(StatusCardKit REQUIRED)

target_link_libraries(YourTarget
    PRIVATE
        StatusCardKit::StatusCardKit
)
```

头文件引用：

```cpp
#include <StatusCardKit/statuscardgrid.h>
#include <StatusCardKit/statuscardqss.h>
```

---

## 线程安全

- `updateField` / `updateFields` 支持从**任意线程**调用。
- 内部自动检测当前线程：若非 UI 线程，则通过 `QMetaObject::invokeMethod(Qt::QueuedConnection)` 排队到主线程执行刷新。
- 调用方无需关心自身所处线程，直接更新即可。

```cpp
// 安全：从 QThread 工作线程调用
card->updateField(QStringLiteral("电机温度"), 85);
```

---

## 设计说明

1. **QSS 动态属性 vs 硬编码调色板**  
   状态变更时仅调用 `setProperty("level", ...)`，由 Qt 样式系统内建重绘机制处理。新增状态级别只需扩展枚举和 QSS 选择器，无需修改卡片绘制代码，符合开闭原则。

2. **Builder 模式优势**  
   卡片配置参数多（标题、序号、字段、映射规则），Builder 将构造与表示分离，避免构造函数参数爆炸；Fluent Interface 使配置代码接近声明式，可读性高；且可在 `build()` 时统一执行 `validateConfig`。

3. **StatusMapper 独立**  
   映射逻辑纯数据驱动，无 QWidget 依赖，可独立单元测试；UI 层仅消费其返回的 `StatusLevel`。

4. **线程安全设计**  
   `updateField` 自动检测线程并排队，调用方无需关心自己运行在哪个线程，降低使用门槛与出错概率。
