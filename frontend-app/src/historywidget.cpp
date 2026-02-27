// -*- coding: utf-8 -*-
#include "historywidget.h"
#include "historymanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>

static QString formatExtraInfo(const QString &gameName, const QString &extraJson)
{
    QJsonObject obj = QJsonDocument::fromJson(extraJson.toUtf8()).object();
    if (obj.isEmpty()) return "";

    if (gameName == "minesweeper") {
        int rows  = obj["rows"].toInt();
        int cols  = obj["cols"].toInt();
        int mines = obj["mines"].toInt();
        int time  = obj["time"].toInt();
        return QString("%1×%2 棋盘  %3颗地雷  用时%4秒").arg(rows).arg(cols).arg(mines).arg(time);
    }
    if (gameName == "snake") {
        int speed  = obj["speed"].toInt();
        int length = obj["length"].toInt();
        return QString("速度%1ms  最终蛇长%2格").arg(speed).arg(length);
    }
    if (gameName == "gomoku") {
        int size   = obj["board_size"].toInt();
        QString w  = obj["winner"].toString();
        QString wt = (w == "black") ? "黑方" : (w == "white") ? "白方" : "无人";
        return QString("%1×%1 棋盘  %2获胜").arg(size).arg(wt);
    }
    if (gameName == "reversi") {
        int black = obj["black"].toInt();
        int white = obj["white"].toInt();
        return QString("黑方%1子  白方%2子").arg(black).arg(white);
    }
    if (gameName == "tetris") {
        int level = obj["level"].toInt();
        int lines = obj["lines"].toInt();
        return QString("最终等级%1  消行%2").arg(level).arg(lines);
    }
    return extraJson;
}

HistoryWidget::HistoryWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void HistoryWidget::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(32, 24, 32, 24);

    auto *backBtn = new QPushButton("← 返回", this);
    backBtn->setFixedWidth(100);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        if (m_innerStack->currentIndex() == 1) {
            m_innerStack->setCurrentIndex(0);
        } else {
            emit backRequested();
        }
    });

    auto *topBar = new QHBoxLayout();
    topBar->addWidget(backBtn);
    topBar->addStretch();
    mainLayout->addLayout(topBar);

    mainLayout->addSpacing(16);

    m_innerStack = new QStackedWidget(this);
    m_innerStack->addWidget(createGameListPage());
    m_innerStack->addWidget(createRecordTablePage());
    mainLayout->addWidget(m_innerStack);
}

QWidget *HistoryWidget::createGameListPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(16);

    auto *title = new QLabel("历史记录", page);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto *desc = new QLabel("选择游戏查看历史记录", page);
    desc->setObjectName("subtitleLabel");
    desc->setAlignment(Qt::AlignCenter);
    layout->addWidget(desc);

    layout->addSpacing(24);

    struct GameInfo { QString name; QString display; QString color; };
    const QList<GameInfo> games = {
        {"minesweeper", "扫  雷",     "#EF4444"},
        {"snake",       "贪吃蛇",     "#10B981"},
        {"gomoku",      "五子棋",     "#F59E0B"},
        {"reversi",     "黑白棋",     "#8B5CF6"},
        {"tetris",      "俄罗斯方块", "#22D3EE"},
    };

    for (const auto &g : games) {
        auto *btn = new QPushButton(g.display, page);
        btn->setFixedSize(280, 48);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: white; border: none; "
                    "border-radius: 10px; font-size: 16px; font-weight: 600; }"
                    "QPushButton:hover { background-color: %1; opacity: 0.8; }")
                .arg(g.color)
        );
        connect(btn, &QPushButton::clicked, this, [this, name = g.name]() {
            showGameRecords(name);
        });
        layout->addWidget(btn, 0, Qt::AlignCenter);
    }

    layout->addStretch();
    return page;
}

QWidget *HistoryWidget::createRecordTablePage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);

    m_recordTitle = new QLabel("", page);
    m_recordTitle->setStyleSheet("font-size: 20px; font-weight: 700; color: #22D3EE;");
    layout->addWidget(m_recordTitle);

    layout->addSpacing(12);

    m_table = new QTableWidget(page);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"日期", "难度", "得分", "结果", "附加信息"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    layout->addWidget(m_table);

    return page;
}

void HistoryWidget::loadRecords(const QString &gameName)
{
    if (gameName.isEmpty()) {
        m_innerStack->setCurrentIndex(0);
    } else {
        showGameRecords(gameName);
    }
}

void HistoryWidget::showGameRecords(const QString &gameName)
{
    m_currentGame = gameName;
    m_recordTitle->setText(HistoryManager::gameDisplayName(gameName) + " — 历史记录");
    populateTable(gameName);
    m_innerStack->setCurrentIndex(1);
}

void HistoryWidget::populateTable(const QString &gameName)
{
    auto records = HistoryManager::instance().getRecords(gameName);
    m_table->setRowCount(records.size());

    bool isBoard = (gameName == "gomoku" || gameName == "reversi");

    for (int i = 0; i < records.size(); ++i) {
        auto row = records[i].toMap();

        // 日期
        QString playedAt = row["played_at"].toString();
        playedAt.replace("T", " ");
        auto *dateItem = new QTableWidgetItem(playedAt);
        dateItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 0, dateItem);

        // 难度
        QString diffDisplay;
        if (isBoard) {
            diffDisplay = "标准";
        } else {
            QString diff = row["difficulty"].toString();
            if (diff == "easy")        diffDisplay = "简单";
            else if (diff == "medium") diffDisplay = "中级";
            else if (diff == "hard")   diffDisplay = "高级";
            else                       diffDisplay = "自定义";
        }
        auto *diffItem = new QTableWidgetItem(diffDisplay);
        diffItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 1, diffItem);

        // 得分（五子棋不显示）
        QString scoreText = isBoard && gameName == "gomoku"
                            ? "—"
                            : QString::number(row["score"].toInt());
        auto *scoreItem = new QTableWidgetItem(scoreText);
        scoreItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(i, 2, scoreItem);

        // 结果
        QString result = row["result"].toString();
        QString resultDisplay;
        if (result == "win")        resultDisplay = "胜利";
        else if (result == "lose")  resultDisplay = "失败";
        else if (result == "draw")  resultDisplay = "平局";
        else                        resultDisplay = "完成";
        auto *resultItem = new QTableWidgetItem(resultDisplay);
        resultItem->setTextAlignment(Qt::AlignCenter);
        if (result == "win")
            resultItem->setForeground(QColor("#10B981"));
        else if (result == "lose")
            resultItem->setForeground(QColor("#EF4444"));
        m_table->setItem(i, 3, resultItem);

        // 附加信息（人类可读）
        QString readable = formatExtraInfo(gameName, row["extra_info"].toString());
        auto *extraItem = new QTableWidgetItem(readable);
        m_table->setItem(i, 4, extraItem);
    }
}
