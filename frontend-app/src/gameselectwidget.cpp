// -*- coding: utf-8 -*-
#include "gameselectwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>

GameSelectWidget::GameSelectWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

bool GameSelectWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *frame = qobject_cast<QFrame*>(obj);
        if (frame && frame->objectName() == "cardFrame") {
            QString gameName = frame->property("gameName").toString();
            if (!gameName.isEmpty()) {
                emit gameSelected(gameName);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void GameSelectWidget::setupUi()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(createSidePanel());
    mainLayout->addWidget(createGameGrid(), 1);
}

QWidget *GameSelectWidget::createSidePanel()
{
    auto *panel = new QFrame(this);
    panel->setObjectName("sidePanel");
    panel->setFixedWidth(250);

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(20, 32, 20, 32);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel("游戏掌机", panel);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    auto *subtitleLabel = new QLabel("Game Console v1.0", panel);
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitleLabel);

    layout->addSpacing(24);

    auto *sep = new QFrame(panel);
    sep->setObjectName("separator");
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    layout->addSpacing(16);

    struct GameInfo { QString name; QString display; };
    const QList<GameInfo> games = {
        {"minesweeper", "扫  雷"},
        {"snake",       "贪吃蛇"},
        {"gomoku",      "五子棋"},
        {"reversi",     "黑白棋"},
        {"tetris",      "俄罗斯方块"},
    };

    for (const auto &g : games) {
        auto *btn = new QPushButton(g.display, panel);
        btn->setFixedHeight(44);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, name = g.name]() {
            emit gameSelected(name);
        });
        layout->addWidget(btn);
    }

    layout->addSpacing(16);

    auto *sep2 = new QFrame(panel);
    sep2->setObjectName("separator");
    sep2->setFrameShape(QFrame::HLine);
    layout->addWidget(sep2);

    layout->addSpacing(8);

    auto *historyBtn = new QPushButton("历史记录", panel);
    historyBtn->setObjectName("accentBtn");
    historyBtn->setFixedHeight(44);
    historyBtn->setCursor(Qt::PointingHandCursor);
    connect(historyBtn, &QPushButton::clicked, this, &GameSelectWidget::historyRequested);
    layout->addWidget(historyBtn);

    layout->addStretch();

    auto *creditLabel = new QLabel("Powered by Qt 6", panel);
    creditLabel->setObjectName("infoLabel");
    creditLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(creditLabel);

    return panel;
}

QWidget *GameSelectWidget::createGameGrid()
{
    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(32, 32, 32, 32);

    auto *headerLabel = new QLabel("选择游戏", container);
    headerLabel->setStyleSheet("font-size: 22px; font-weight: 700; color: #F1F5F9; margin-bottom: 8px;");
    layout->addWidget(headerLabel);

    auto *descLabel = new QLabel("点击游戏封面开始游戏", container);
    descLabel->setObjectName("infoLabel");
    layout->addWidget(descLabel);

    layout->addSpacing(16);

    auto *grid = new QGridLayout();
    grid->setSpacing(16);

    struct GameInfo { QString name; QString display; QString icon; };
    const QList<GameInfo> games = {
        {"minesweeper", "扫  雷",     ":/icons/minesweeper.svg"},
        {"snake",       "贪吃蛇",     ":/icons/snake.svg"},
        {"gomoku",      "五子棋",     ":/icons/gomoku.svg"},
        {"reversi",     "黑白棋",     ":/icons/reversi.svg"},
        {"tetris",      "俄罗斯方块", ":/icons/tetris.svg"},
    };

    for (int i = 0; i < games.size(); ++i) {
        auto *card = createGameCard(games[i].name, games[i].display, games[i].icon);
        grid->addWidget(card, i / 3, i % 3);
    }

    layout->addLayout(grid, 1);
    layout->addStretch();

    return container;
}

QWidget *GameSelectWidget::createGameCard(const QString &gameName,
                                          const QString &displayName,
                                          const QString &iconPath)
{
    auto *card = new QFrame(this);
    card->setObjectName("cardFrame");
    card->setCursor(Qt::PointingHandCursor);
    card->setMinimumSize(200, 180);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 20, 16, 16);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(12);

    auto *iconLabel = new QLabel(card);
    QPixmap pix(80, 80);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QSvgRenderer renderer(iconPath);
    renderer.render(&painter, QRectF(0, 0, 80, 80));
    painter.end();
    iconLabel->setPixmap(pix);
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    auto *nameLabel = new QLabel(displayName, card);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-size: 16px; font-weight: 600; color: #F1F5F9;");
    layout->addWidget(nameLabel);

    card->setProperty("gameName", gameName);
    card->installEventFilter(this);

    return card;
}
