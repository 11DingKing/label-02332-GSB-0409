// -*- coding: utf-8 -*-
#ifndef GAMEBASE_H
#define GAMEBASE_H

#include <QWidget>
#include <QString>
#include <QTimer>

/**
 * GameBase — 所有游戏的抽象基类
 * 管理游戏通用生命周期：启动、暂停、恢复、重置
 * 子类实现具体游戏逻辑和绘制
 */
class GameBase : public QWidget
{
    Q_OBJECT

public:
    enum Difficulty { Easy, Medium, Hard, Custom };
    Q_ENUM(Difficulty)

    explicit GameBase(const QString &gameName, QWidget *parent = nullptr);
    virtual ~GameBase() = default;

    QString gameName() const { return m_gameName; }
    Difficulty difficulty() const { return m_difficulty; }
    bool isRunning() const { return m_running; }

    virtual void startGame(Difficulty diff, const QVariantMap &customParams = {});
    virtual void pauseGame();
    virtual void resumeGame();
    virtual void resetGame();

    static QString difficultyToString(Difficulty d);
    static QString difficultyToDisplayString(Difficulty d);
    QVariantMap customParams() const { return m_customParams; }

signals:
    void gameFinished(int score, const QString &result, const QString &extraInfo);
    void backRequested();

protected:
    virtual void initGame() = 0;
    void finishGame(int score, const QString &result, const QString &extraInfo = "{}");

    QString m_gameName;
    Difficulty m_difficulty = Easy;
    QVariantMap m_customParams;
    bool m_running = false;
    bool m_paused = false;
};

#endif // GAMEBASE_H
