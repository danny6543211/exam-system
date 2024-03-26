CREATE TABLE IF NOT EXISTS exam_question(
    id INTEGER PRIMARY KEY AUTOINCREMENT, 
    question_text TEXT NOT NULL, 
    options TEXT NOT NULL, 
    correct_option_index TEXT NOT NULL
);

INSERT INTO exam_question (question_text, options, correct_option_index)
VALUES ('test_question_text', 'test_options', 'test_correct_option_index');

