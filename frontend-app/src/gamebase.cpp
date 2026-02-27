// -*- coding: utf-8 -*-
#include "gamebase.h"

GameBase::GameBase(const QString &gameName, QWidget *parent)
    : QWidget(parent), m_gameName(gameName)
{
    setFocusPolicy(Qt::StrongFocus);
}

void GameBase::startGame(Difficulty diff, const QVariantMap &customParams)
{
    m_difficulty = diff;
    m_customParams = customParams;
    m_running = true;
    m_paused = false;
    initGame();
}

void GameBase::pauseGame()
{
    if (m_running && !m_paused) {
        m_paused = true;
    }
}

void GameBase::resumeGame()
{
    if (m_running && m_paused) {
        m_paused = false;
    }
}

void GameBase::resetGame()
{
    m_running = false;
    m_paused = false;
}

void GameBase::finishGame(int score, const QString &result, const QString &extraInfo)
{
    m_running = false;
    m_paused = false;
    emit gameFinished(score, result, extraInfo);
}

QString GameBase::difficultyToString(Difficulty d)
{
    switch (d) {
    case Easy:   return "easy";
    case Medium: return "medium";
    case Hard:   return "hard";
    case Custom: return "custom";
    }
    return "easy";
}

QString GameBase::difficultyToDisplayString(Difficulty d)
{
    switch (d) {
    case Easy:   return "简单";
    case Medium: return "中级";
    case Hard:   return "高级";
    case Custom: return "自定义";
    }
    return "简单";
}
