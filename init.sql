CREATE TABLE IF NOT EXISTS exam_question(
    id INTEGER PRIMARY KEY AUTOINCREMENT, 
    title TEXT NOT NULL,
    question_text texTEXTt NOT NULL, 
    options TEXT NOT NULL, 
    correct_option_index TEXT NOT NULL
);

INSERT INTO exam_question (title, question_text, options, correct_option_index)
VALUES ('test_title', 'test_question_text', 'test_options', 'test_correct_option_index');