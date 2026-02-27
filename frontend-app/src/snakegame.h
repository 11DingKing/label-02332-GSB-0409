// -*- coding: utf-8 -*-
#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include "gamebase.h"
#include <QTimer>
#include <QLabel>
#include <QPoint>
#include <QList>

/**
 * SnakeGame — 贪吃蛇
 * 方向键控制蛇移动，吃食物增长得分
 */
class SnakeGame : public GameBase
{
    Q_OBJECT

public:
    explicit SnakeGame(QWidget *parent = nullptr);

    void pauseGame() override;
    void resumeGame() override;

protected:
    void initGame() override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum Direction { Up, Down, Left, Right };

    void tick();
    void spawnFood();
    QRect cellRect(int row, int col) const;

    static constexpr int GRID_ROWS = 20;
    static constexpr int GRID_COLS = 20;

    QList<QPoint> m_snake;
    QPoint m_food;
    Direction m_direction = Right;
    Direction m_nextDirection = Right;
    int m_score = 0;
    bool m_gameOver = false;

    QTimer *m_moveTimer = nullptr;
    int m_speed = 150;

    QLabel *m_scoreLabel = nullptr;

    int m_cellSize = 24;
    int m_boardOffsetX = 0;
    int m_boardOffsetY = 0;
};

#endif // SNAKEGAME_H
