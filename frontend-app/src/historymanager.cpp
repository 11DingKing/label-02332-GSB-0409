// -*- coding: utf-8 -*-
#include "historymanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug>

HistoryManager &HistoryManager::instance()
{
    static HistoryManager inst;
    return inst;
}

HistoryManager::HistoryManager(QObject *parent)
    : QObject(parent)
{
}

bool HistoryManager::initDatabase()
{
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    QString dbPath = dataPath + "/game_history.db";

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qWarning() << "无法打开数据库:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS game_history ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  game_name TEXT NOT NULL,"
        "  difficulty TEXT NOT NULL,"
        "  score INTEGER DEFAULT 0,"
        "  result TEXT NOT NULL,"
        "  played_at TEXT NOT NULL,"
        "  extra_info TEXT DEFAULT '{}'"
        ")"
    );
    if (!ok) {
        qWarning() << "建表失败:" << query.lastError().text();
        return false;
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_game_name ON game_history(game_name)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_played_at ON game_history(played_at DESC)");

    loadSeedDataIfEmpty();
    qInfo() << "数据库初始化完成:" << dbPath;
    return true;
}

void HistoryManager::loadSeedDataIfEmpty()
{
    QSqlQuery countQuery(m_db);
    countQuery.exec("SELECT COUNT(*) FROM game_history");
    if (countQuery.next() && countQuery.value(0).toInt() > 0) {
        return;
    }

    QSqlQuery q(m_db);
    // 扫雷示例数据
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('minesweeper','easy',45,'win','2026-02-20 14:30:00','{\"rows\":9,\"cols\":9,\"mines\":10}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('minesweeper','medium',120,'win','2026-02-21 10:15:00','{\"rows\":16,\"cols\":16,\"mines\":40}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('minesweeper','hard',0,'lose','2026-02-22 16:45:00','{\"rows\":16,\"cols\":30,\"mines\":99}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('minesweeper','easy',32,'win','2026-02-23 09:00:00','{\"rows\":9,\"cols\":9,\"mines\":10}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('minesweeper','medium',0,'lose','2026-02-24 20:30:00','{\"rows\":16,\"cols\":16,\"mines\":40}')");

    // 贪吃蛇示例数据
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('snake','easy',180,'score_only','2026-02-20 15:00:00','{\"speed\":200}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('snake','medium',320,'score_only','2026-02-21 11:30:00','{\"speed\":150}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('snake','hard',85,'score_only','2026-02-22 17:00:00','{\"speed\":100}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('snake','easy',250,'score_only','2026-02-23 10:00:00','{\"speed\":200}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('snake','medium',410,'score_only','2026-02-25 14:20:00','{\"speed\":150}')");

    // 五子棋示例数据
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('gomoku','easy',0,'win','2026-02-20 16:00:00','{\"board_size\":15,\"winner\":\"black\"}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('gomoku','medium',0,'lose','2026-02-21 12:00:00','{\"board_size\":15,\"winner\":\"white\"}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('gomoku','easy',0,'win','2026-02-22 18:00:00','{\"board_size\":15,\"winner\":\"black\"}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('gomoku','medium',0,'win','2026-02-24 09:30:00','{\"board_size\":15,\"winner\":\"black\"}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('gomoku','hard',0,'lose','2026-02-25 15:00:00','{\"board_size\":19,\"winner\":\"white\"}')");

    // 黑白棋示例数据
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('reversi','easy',38,'win','2026-02-20 17:00:00','{\"black\":38,\"white\":26}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('reversi','medium',28,'lose','2026-02-21 13:00:00','{\"black\":28,\"white\":36}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('reversi','easy',40,'win','2026-02-23 11:00:00','{\"black\":40,\"white\":24}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('reversi','medium',32,'win','2026-02-24 10:30:00','{\"black\":32,\"white\":32}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('reversi','hard',25,'lose','2026-02-25 16:00:00','{\"black\":25,\"white\":39}')");

    // 俄罗斯方块示例数据
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('tetris','easy',2400,'score_only','2026-02-20 18:00:00','{\"level\":4,\"lines\":24}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('tetris','medium',5600,'score_only','2026-02-21 14:00:00','{\"level\":7,\"lines\":42}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('tetris','hard',1200,'score_only','2026-02-22 19:00:00','{\"level\":3,\"lines\":12}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('tetris','easy',3800,'score_only','2026-02-23 12:00:00','{\"level\":5,\"lines\":30}')");
    q.exec("INSERT INTO game_history (game_name,difficulty,score,result,played_at,extra_info) VALUES "
           "('tetris','medium',7200,'score_only','2026-02-25 17:00:00','{\"level\":9,\"lines\":56}')");

    qInfo() << "已加载示例数据 (25条)";
}

bool HistoryManager::addRecord(const QString &gameName,
                               const QString &difficulty,
                               int score,
                               const QString &result,
                               const QString &extraInfo)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO game_history (game_name, difficulty, score, result, played_at, extra_info) "
              "VALUES (?, ?, ?, ?, ?, ?)");
    q.addBindValue(gameName);
    q.addBindValue(difficulty);
    q.addBindValue(score);
    q.addBindValue(result);
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    q.addBindValue(extraInfo);

    if (!q.exec()) {
        qWarning() << "插入记录失败:" << q.lastError().text();
        return false;
    }
    return true;
}

QVariantList HistoryManager::getRecords(const QString &gameName) const
{
    QVariantList records;
    QSqlQuery q(m_db);
    q.prepare("SELECT id, game_name, difficulty, score, result, played_at, extra_info "
              "FROM game_history WHERE game_name = ? ORDER BY played_at DESC LIMIT 50");
    q.addBindValue(gameName);

    if (q.exec()) {
        while (q.next()) {
            QVariantMap row;
            row["id"] = q.value(0);
            row["game_name"] = q.value(1);
            row["difficulty"] = q.value(2);
            row["score"] = q.value(3);
            row["result"] = q.value(4);
            row["played_at"] = q.value(5);
            row["extra_info"] = q.value(6);
            records.append(row);
        }
    }
    return records;
}

QVariantList HistoryManager::getAllRecords() const
{
    QVariantList records;
    QSqlQuery q(m_db);
    q.exec("SELECT id, game_name, difficulty, score, result, played_at, extra_info "
           "FROM game_history ORDER BY played_at DESC LIMIT 100");

    while (q.next()) {
        QVariantMap row;
        row["id"] = q.value(0);
        row["game_name"] = q.value(1);
        row["difficulty"] = q.value(2);
        row["score"] = q.value(3);
        row["result"] = q.value(4);
        row["played_at"] = q.value(5);
        row["extra_info"] = q.value(6);
        records.append(row);
    }
    return records;
}

bool HistoryManager::clearRecords(const QString &gameName)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM game_history WHERE game_name = ?");
    q.addBindValue(gameName);
    return q.exec();
}

QString HistoryManager::gameDisplayName(const QString &gameName)
{
    static const QMap<QString, QString> names = {
        {"minesweeper", "扫雷"},
        {"snake", "贪吃蛇"},
        {"gomoku", "五子棋"},
        {"reversi", "黑白棋"},
        {"tetris", "俄罗斯方块"}
    };
    return names.value(gameName, gameName);
}
