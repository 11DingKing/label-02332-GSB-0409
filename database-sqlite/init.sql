-- 游戏掌机 历史记录数据库初始化脚本
-- 数据库: SQLite (嵌入式)
-- 编码: UTF-8

CREATE TABLE IF NOT EXISTS game_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    game_name TEXT NOT NULL,
    difficulty TEXT NOT NULL,
    score INTEGER DEFAULT 0,
    result TEXT NOT NULL,
    played_at TEXT NOT NULL,
    extra_info TEXT DEFAULT '{}'
);

CREATE INDEX IF NOT EXISTS idx_game_name ON game_history(game_name);
CREATE INDEX IF NOT EXISTS idx_played_at ON game_history(played_at DESC);

-- 示例数据: 扫雷
INSERT INTO game_history (game_name, difficulty, score, result, played_at, extra_info) VALUES
('minesweeper', 'easy', 45, 'win', '2026-02-20 14:30:00', '{"rows":9,"cols":9,"mines":10}'),
('minesweeper', 'medium', 120, 'win', '2026-02-21 10:15:00', '{"rows":16,"cols":16,"mines":40}'),
('minesweeper', 'hard', 0, 'lose', '2026-02-22 16:45:00', '{"rows":16,"cols":30,"mines":99}'),
('minesweeper', 'easy', 32, 'win', '2026-02-23 09:00:00', '{"rows":9,"cols":9,"mines":10}'),
('minesweeper', 'medium', 0, 'lose', '2026-02-24 20:30:00', '{"rows":16,"cols":16,"mines":40}');

-- 示例数据: 贪吃蛇
INSERT INTO game_history (game_name, difficulty, score, result, played_at, extra_info) VALUES
('snake', 'easy', 180, 'score_only', '2026-02-20 15:00:00', '{"speed":200}'),
('snake', 'medium', 320, 'score_only', '2026-02-21 11:30:00', '{"speed":150}'),
('snake', 'hard', 85, 'score_only', '2026-02-22 17:00:00', '{"speed":100}'),
('snake', 'easy', 250, 'score_only', '2026-02-23 10:00:00', '{"speed":200}'),
('snake', 'medium', 410, 'score_only', '2026-02-25 14:20:00', '{"speed":150}');

-- 示例数据: 五子棋
INSERT INTO game_history (game_name, difficulty, score, result, played_at, extra_info) VALUES
('gomoku', 'easy', 0, 'win', '2026-02-20 16:00:00', '{"board_size":15,"winner":"black"}'),
('gomoku', 'medium', 0, 'lose', '2026-02-21 12:00:00', '{"board_size":15,"winner":"white"}'),
('gomoku', 'easy', 0, 'win', '2026-02-22 18:00:00', '{"board_size":15,"winner":"black"}'),
('gomoku', 'medium', 0, 'win', '2026-02-24 09:30:00', '{"board_size":15,"winner":"black"}'),
('gomoku', 'hard', 0, 'lose', '2026-02-25 15:00:00', '{"board_size":19,"winner":"white"}');

-- 示例数据: 黑白棋
INSERT INTO game_history (game_name, difficulty, score, result, played_at, extra_info) VALUES
('reversi', 'easy', 38, 'win', '2026-02-20 17:00:00', '{"black":38,"white":26}'),
('reversi', 'medium', 28, 'lose', '2026-02-21 13:00:00', '{"black":28,"white":36}'),
('reversi', 'easy', 40, 'win', '2026-02-23 11:00:00', '{"black":40,"white":24}'),
('reversi', 'medium', 32, 'win', '2026-02-24 10:30:00', '{"black":32,"white":32}'),
('reversi', 'hard', 25, 'lose', '2026-02-25 16:00:00', '{"black":25,"white":39}');

-- 示例数据: 俄罗斯方块
INSERT INTO game_history (game_name, difficulty, score, result, played_at, extra_info) VALUES
('tetris', 'easy', 2400, 'score_only', '2026-02-20 18:00:00', '{"level":4,"lines":24}'),
('tetris', 'medium', 5600, 'score_only', '2026-02-21 14:00:00', '{"level":7,"lines":42}'),
('tetris', 'hard', 1200, 'score_only', '2026-02-22 19:00:00', '{"level":3,"lines":12}'),
('tetris', 'easy', 3800, 'score_only', '2026-02-23 12:00:00', '{"level":5,"lines":30}'),
('tetris', 'medium', 7200, 'score_only', '2026-02-25 17:00:00', '{"level":9,"lines":56}');
