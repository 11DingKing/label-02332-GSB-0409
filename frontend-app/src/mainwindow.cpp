// -*- coding: utf-8 -*-
#include "mainwindow.h"
#include "gameselectwidget.h"
#include "difficultyselectwidget.h"
#include "historywidget.h"
#include "historymanager.h"
#include "minesweepergame.h"
#include "snakegame.h"
#include "gomokugame.h"
#include "reversigame.h"
#include "tetrisgame.h"
#include <QMessageBox>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    HistoryManager::instance().initDatabase();
    setupUi();
}

void MainWindow::setupUi()
{
    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_gameSelect = new GameSelectWidget(this);
    m_difficultySelect = new DifficultySelectWidget(this);
    m_historyWidget = new HistoryWidget(this);

    m_minesweeper = new MinesweeperGame(this);
    m_snake       = new SnakeGame(this);
    m_gomoku      = new GomokuGame(this);
    m_reversi     = new ReversiGame(this);
    m_tetris      = new TetrisGame(this);

    m_stack->addWidget(m_gameSelect);        // 0
    m_stack->addWidget(m_difficultySelect);  // 1
    m_stack->addWidget(m_historyWidget);     // 2
    m_stack->addWidget(m_minesweeper);       // 3
    m_stack->addWidget(m_snake);             // 4
    m_stack->addWidget(m_gomoku);            // 5
    m_stack->addWidget(m_reversi);           // 6
    m_stack->addWidget(m_tetris);            // 7

    connect(m_gameSelect, &GameSelectWidget::gameSelected,
            this, &MainWindow::showDifficultySelect);
    connect(m_gameSelect, &GameSelectWidget::historyRequested,
            this, [this]() { showHistory(); });

    connect(m_difficultySelect, &DifficultySelectWidget::difficultyChosen,
            this, &MainWindow::startGame);
    connect(m_difficultySelect, &DifficultySelectWidget::backRequested,
            this, &MainWindow::showGameSelect);
    connect(m_difficultySelect, &DifficultySelectWidget::historyRequested,
            this, [this](const QString &game) { showHistory(game); });

    connect(m_historyWidget, &HistoryWidget::backRequested,
            this, &MainWindow::showGameSelect);

    auto connectGame = [this](GameBase *game, int stackIdx) {
        connect(game, &GameBase::gameFinished, this, &MainWindow::onGameFinished);
        connect(game, &GameBase::backRequested, this, &MainWindow::showGameSelect);
    };
    connectGame(m_minesweeper, 3);
    connectGame(m_snake, 4);
    connectGame(m_gomoku, 5);
    connectGame(m_reversi, 6);
    connectGame(m_tetris, 7);

    m_stack->setCurrentIndex(0);
}

void MainWindow::showGameSelect()
{
    if (auto *game = dynamic_cast<GameBase*>(m_stack->currentWidget())) {
        game->resetGame();
    }
    m_stack->setCurrentIndex(0);
}

void MainWindow::showDifficultySelect(const QString &gameName)
{
    m_currentGame = gameName;
    m_difficultySelect->setGame(gameName);
    m_stack->setCurrentIndex(1);
}

void MainWindow::showHistory(const QString &gameName)
{
    m_historyWidget->loadRecords(gameName);
    m_stack->setCurrentIndex(2);
}

void MainWindow::startGame(const QString &gameName, GameBase::Difficulty diff,
                            const QVariantMap &customParams)
{
    m_currentGame = gameName;
    GameBase *game = gameWidget(gameName);
    if (!game) return;

    m_stack->setCurrentWidget(game);
    game->startGame(diff, customParams);
    game->setFocus();
}

void MainWindow::onGameFinished(int score, const QString &result, const QString &extraInfo)
{
    auto *game = qobject_cast<GameBase*>(sender());
    if (!game) return;

    HistoryManager::instance().addRecord(
        game->gameName(),
        GameBase::difficultyToString(game->difficulty()),
        score,
        result,
        extraInfo
    );

    QString gameName = game->gameName();
    QJsonObject extra = QJsonDocument::fromJson(extraInfo.toUtf8()).object();

    QString msg;
    if (gameName == "gomoku") {
        QString winner = extra["winner"].toString();
        if (winner == "black")       msg = "游戏结束\n\n🏆  黑方获胜";
        else if (winner == "white")  msg = "游戏结束\n\n🏆  白方获胜";
        else                         msg = "游戏结束\n\n平局";
    } else if (gameName == "reversi") {
        int black = extra["black"].toInt(score);
        int white = extra["white"].toInt();
        if (result == "win")        msg = QString("游戏结束\n\n🏆  黑方获胜\n黑 %1  :  白 %2").arg(black).arg(white);
        else if (result == "lose")  msg = QString("游戏结束\n\n🏆  白方获胜\n黑 %1  :  白 %2").arg(black).arg(white);
        else                        msg = QString("游戏结束\n\n平局\n黑 %1  :  白 %2").arg(black).arg(white);
    } else {
        QString displayResult;
        if (result == "win")        displayResult = "🏆  胜利";
        else if (result == "lose")  displayResult = "失败";
        else if (result == "draw")  displayResult = "平局";
        else                        displayResult = "完成";
        msg = QString("游戏结束\n\n%1\n得分: %2").arg(displayResult).arg(score);
    }

    QMessageBox box(this);
    box.setWindowTitle(HistoryManager::gameDisplayName(gameName));
    box.setText(msg);
    box.setStyleSheet(
        "QMessageBox { background-color: #1E293B; }"
        "QMessageBox QLabel { color: #F1F5F9; font-size: 16px; }"
        "QPushButton { background-color: #6366F1; color: white; border: none; "
        "border-radius: 6px; padding: 8px 24px; font-size: 14px; min-width: 80px; }"
        "QPushButton:hover { background-color: #818CF8; }"
    );
    QAbstractButton *returnBtn = box.addButton("返回菜单", QMessageBox::AcceptRole);
    box.addButton("再来一局", QMessageBox::RejectRole);
    box.exec();

    if (box.clickedButton() == returnBtn) {
        showGameSelect();
    } else {
        game->startGame(game->difficulty(), game->customParams());
        game->setFocus();
    }
}

GameBase *MainWindow::gameWidget(const QString &gameName) const
{
    if (gameName == "minesweeper") return m_minesweeper;
    if (gameName == "snake")       return m_snake;
    if (gameName == "gomoku")      return m_gomoku;
    if (gameName == "reversi")     return m_reversi;
    if (gameName == "tetris")      return m_tetris;
    return nullptr;
}
