CREATE TABLE IF NOT EXISTS question(
    question_id INTEGER PRIMARY KEY AUTOINCREMENT, 
    question_text TEXT NOT NULL, 
    question_options_text TEXT NOT NULL, 
    question_correct_options TEXT NOT NULL
);

INSERT INTO question (question_text, question_options_text, question_correct_options)
VALUES ('1 + 1 = ?', '5 3 2 1', '3.');