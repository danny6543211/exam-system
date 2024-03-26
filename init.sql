create table if not exists exam_question(
    id int primary key not null, 
    title text not null,
    question_text text not null, 
    options text not null, 
    correct_option_index text not null
);

insert into exam_question (id, title, question_text, options, correct_option_index)
values (1, 'qweqweqwe', 'qweqweqwe', 'qweqweqwe', 'qweqweqwe');
