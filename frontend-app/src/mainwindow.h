// -*- coding: utf-8 -*-
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "gamebase.h"

class GameSelectWidget;
class DifficultySelectWidget;
class HistoryWidget;
class MinesweeperGame;
class SnakeGame;
class GomokuGame;
class ReversiGame;
class TetrisGame;

/**
 * MainWindow — 主窗口
 * 通过 QStackedWidget 管理所有页面切换
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void showGameSelect();
    void showDifficultySelect(const QString &gameName);
    void showHistory(const QString &gameName = "");
    void startGame(const QString &gameName, GameBase::Difficulty diff,
                   const QVariantMap &customParams);
    void onGameFinished(int score, const QString &result, const QString &extraInfo);

private:
    void setupUi();
    GameBase *gameWidget(const QString &gameName) const;

    QStackedWidget *m_stack = nullptr;

    GameSelectWidget      *m_gameSelect = nullptr;
    DifficultySelectWidget *m_difficultySelect = nullptr;
    HistoryWidget         *m_historyWidget = nullptr;

    MinesweeperGame *m_minesweeper = nullptr;
    SnakeGame       *m_snake = nullptr;
    GomokuGame      *m_gomoku = nullptr;
    ReversiGame     *m_reversi = nullptr;
    TetrisGame      *m_tetris = nullptr;

    QString m_currentGame;
};

#endif // MAINWINDOW_H
