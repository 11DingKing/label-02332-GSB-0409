// -*- coding: utf-8 -*-
#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>
#include <QVariantMap>

/**
 * HistoryManager — 游戏历史记录管理器（单例）
 * 使用 SQLite 嵌入式数据库存储游戏成绩
 */
class HistoryManager : public QObject
{
    Q_OBJECT

public:
    static HistoryManager &instance();

    bool initDatabase();

    bool addRecord(const QString &gameName,
                   const QString &difficulty,
                   int score,
                   const QString &result,
                   const QString &extraInfo = "{}");

    QVariantList getRecords(const QString &gameName) const;
    QVariantList getAllRecords() const;
    bool clearRecords(const QString &gameName);

    static QString gameDisplayName(const QString &gameName);

private:
    explicit HistoryManager(QObject *parent = nullptr);
    ~HistoryManager() override = default;
    HistoryManager(const HistoryManager &) = delete;
    HistoryManager &operator=(const HistoryManager &) = delete;

    void loadSeedDataIfEmpty();

    QSqlDatabase m_db;
};

#endif // HISTORYMANAGER_H
