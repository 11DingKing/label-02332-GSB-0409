// -*- coding: utf-8 -*-
#ifndef REVERSIGAME_H
#define REVERSIGAME_H

#include "gamebase.h"
#include <QVector>
#include <QLabel>
#include <QPoint>
#include <QList>

/**
 * ReversiGame — 黑白棋 / 翻转棋（双人对战）
 * 8×8 棋盘，通过夹击翻转对方棋子
 */
class ReversiGame : public GameBase
{
    Q_OBJECT

public:
    explicit ReversiGame(QWidget *parent = nullptr);

protected:
    void initGame() override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum Disc { Empty, BlackDisc, WhiteDisc };

    bool isValidMove(int row, int col, Disc disc) const;
    QList<QPoint> getFlippable(int row, int col, Disc disc) const;
    QList<QPoint> getFlippableInDir(int row, int col, int dr, int dc, Disc disc) const;
    bool hasValidMoves(Disc disc) const;
    void countDiscs(int &black, int &white) const;
    QPoint cellFromPos(const QPoint &pos) const;
    QRect cellRect(int row, int col) const;
    void updateStatus();

    static constexpr int BOARD_SIZE = 8;
    QVector<QVector<Disc>> m_board;
    Disc m_currentPlayer = BlackDisc;
    bool m_gameOver = false;
    int m_blackCount = 2;
    int m_whiteCount = 2;

    QLabel *m_statusLabel = nullptr;
    QLabel *m_countLabel = nullptr;

    int m_cellSize = 50;
    int m_boardOffsetX = 0;
    int m_boardOffsetY = 0;
};

#endif // REVERSIGAME_H
