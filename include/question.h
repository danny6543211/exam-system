#ifndef QUESTION_H
#define QUESTION_H

#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <windows.h>
#include "sqlite3.h"


const size_t READ_BUFFER_SIZE = 256;

struct Question {
    int id = -1;
    std::string subject;
    std::string questionText;
    std::vector<std::string> options;
    std::string answer; 
    
    Question(std::string questionText, std::vector<std::string> options, std::string answer
    ) : questionText(questionText), options(options), answer(answer) {}

    Question() {}
    ~Question() {}

    void reset() {
        id = -1;
        subject = "";
        questionText = "";
        options.clear();
        answer.clear();
    }
};


class QuestionDatabase {
private:
    sqlite3* db;

public:
    QuestionDatabase() {
        int rc = sqlite3_open("db.sqlite3", &db);
        sqliteCheck(db, rc, SQLITE_OK);
        
        const char* sql = "CREATE TABLE IF NOT EXISTS questions("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "subject INTEGER TEXT NOT NULL, "
                          "question_text TEXT NOT NULL, "
                          "answer TEXT NOT NULL);";
        rc = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
        sqliteCheck(db, rc, SQLITE_OK);

        sql = "CREATE TABLE IF NOT EXISTS options("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "a TEXT NOT NULL, "
              "b TEXT NOT NULL, "
              "c TEXT NOT NULL, "
              "d TEXT NOT NULL, "
              "e TEXT);";
        rc = sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
        sqliteCheck(db, rc, SQLITE_OK);
    }

    ~QuestionDatabase() {}

    void insertQuestion(const Question& question) {
        const char* sql = "INSERT INTO options (a, b, c, d, e) VALUES (?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqliteCheck(db, rc, SQLITE_OK);

        for (int i = 0; i < question.options.size(); i++)
            sqlite3_bind_text(stmt, i+1, question.options[i].c_str(), -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        sqliteCheck(db, rc, SQLITE_DONE);
        
        sqlite3_reset(stmt);
        sql = "INSERT INTO questions (subject, question_text, answer) VALUES (?, ?, ?);";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqliteCheck(db, rc, SQLITE_OK);

        sqlite3_bind_text(stmt, 1, question.subject.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, question.questionText.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, question.answer.c_str(), -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        sqliteCheck(db, rc, SQLITE_DONE);

        sqlite3_finalize(stmt);
    }

    const std::vector<Question> getAllQuesions() {
        const char* questionsSql = "SELECT * FROM questions;";
        const char* optionsSql = "SELECT * FROM options WHERE id = ?;";
        sqlite3_stmt* questionsStmt;
        sqlite3_stmt* optionsStmt;
        int rc = sqlite3_prepare_v2(db, questionsSql, -1, &questionsStmt, nullptr);
        sqliteCheck(db, rc, SQLITE_OK);
        rc = sqlite3_prepare_v2(db, optionsSql, -1, &optionsStmt, nullptr);
        sqliteCheck(db, rc, SQLITE_OK);

        std::vector<Question> questions;
        while (sqlite3_step(questionsStmt) == SQLITE_ROW) {
            Question question;
            auto id = sqlite3_column_int(questionsStmt, 0);
            question.id = id;
            question.subject = (char*)sqlite3_column_text(questionsStmt, 1);
            question.questionText = (char*)sqlite3_column_text(questionsStmt, 2);
            question.answer = (char*)sqlite3_column_text(questionsStmt, 3);
            
            rc = sqlite3_reset(optionsStmt);
            sqliteCheck(db, rc, SQLITE_OK);
            rc = sqlite3_bind_int(optionsStmt, 1, id);
            sqliteCheck(db, rc, SQLITE_OK);
            rc = sqlite3_step(optionsStmt);
            sqliteCheck(db, rc, SQLITE_ROW);
            
            for (int i = 1; i < sqlite3_column_count(optionsStmt); i++) {
                if (sqlite3_column_text(optionsStmt, i) == nullptr)
                    break;
                else 
                    question.options.push_back((char*)sqlite3_column_text(optionsStmt, i));
            }

            questions.push_back(question);
        }

        sqlite3_finalize(questionsStmt);
        sqlite3_finalize(optionsStmt);
        return questions;
    }   

    void deleteQuestion(int id) {
        auto questions = getAllQuesions();
        std::string sql = "DELETE FROM questions where id = " + std::to_string(id) + ";";
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
        sqliteCheck(db, rc, SQLITE_OK);
        sql = "DELETE FROM options where id = " + std::to_string(id) + ";";
        rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
        sqliteCheck(db, rc, SQLITE_OK);
    }

private:
    void sqliteCheck(sqlite3* db, int rc, int check_status) {
        if (rc != check_status)
            throw std::runtime_error(sqlite3_errmsg(db));
    }
};


class QuestionManagement {
private:
    char bufferRead[READ_BUFFER_SIZE];

public:
    QuestionManagement() {}
    ~QuestionManagement() {}

    void run() {
        while (true) {
            system("cls");
            printMenu();
            std::cin.getline(bufferRead, READ_BUFFER_SIZE);
            switch (bufferRead[0]) {
            case '1':
                addNewQuestion();
                break;
            case '2':
                displayAllQuestions();
                break;
            case '3':
                deleteQuestion();
                break;
            case 'q':
            case 'Q':
                return;
            default:
                break;
            }
        }
    }

private:
    void printHead() {
        std::cout << "------ Question Management ------\n\n";
    }

    void printMenu() {
        system("cls");
        printHead();
        std::cout << "1) Add a new question\n";
        std::cout << "2) Display all questions\n";
        std::cout << "3) Delete questions\n";
        std::cout << "q) Back to menu\n";   
        std::cout << "\n";
    }
    
    void addNewQuestion() {
        Question question;

        auto readSubject = [&]()->int {
            system("cls");
            printHead();
            std::cout << "Input the subject of question.\n";
            std::cout << "\n:";
            std::cin.getline(bufferRead, READ_BUFFER_SIZE);
            question.subject = bufferRead;

            if (question.subject.length() == 0)
                return 1;
            else 
                return 0;
        };

        auto readQuestionText = [&]()->int {
            system("cls");
            printHead();
            std::cout << "Input the description of question.\n";
            std::cout << "\n:";
            std::cin.getline(bufferRead, READ_BUFFER_SIZE);
            question.questionText = bufferRead;

            if (question.questionText.length() == 0)
                return 1;
            else 
                return 0;
        };

        auto readOptions = [&]()->int {
            question.options.clear();
            system("cls");
            printHead();
            while (true) {
                std::cout << "Input the option of question, enter '#' to quit\n";
                std::cout << "\n:";
                std::cin.getline(bufferRead, READ_BUFFER_SIZE);
                if (bufferRead[0] == '#')
                    break;               
                question.options.push_back(bufferRead);
            }
            if (question.options.size() == 0 || question.options.size() < 4)
                return 1;
            else 
                return 0;
        };

        auto readAnswer = [&]()->int {    
            system("cls");
            printHead();
            while (true) {
                std::cout << "Input the answer, like 'a' or 'abc\n\n";
                std::cin.getline(bufferRead, READ_BUFFER_SIZE);
                question.answer = bufferRead;
                if (question.answer.size() == 0)
                    return 1;
                else 
                    return 0;
            }
        };

        while (true) {
            if (readSubject() || readQuestionText() || readOptions() || readAnswer())
                std::cout << "Text can not be empty, please input again\n";
            else {
                printHead();
                std::cout << "Please check the input question:\n\n";
                std::cout << "Question text:\n" << question.questionText << "\n\n";
                std::cout << "Question options:\n";
                for (int i = 0; i < question.options.size(); i++) {
                    std::cout << (char)('a'+i) << ") " << question.options[i] << "\n";
                }
                std::cout << "\nCorrect answer:\n";
                for (int i = 0; i< question.answer.size(); i++) {
                    std::cout << question.answer[i] << " ";
                } 
                std::cout << "\n\n";
                std::cout << "1) Confirm submission\n";
                std::cout << "2) Enter again\n";
                std::cout << "\n:";

                std::cin.getline(bufferRead, READ_BUFFER_SIZE);
                if (bufferRead[0] == '1') 
                {
                    QuestionDatabase questionDatabase;
                    questionDatabase.insertQuestion(question);      
                    break;
                } 
                else 
                    question.reset();    
            }
        }
    }

    void displayAllQuestions() {
        QuestionDatabase questionDatabase;
        auto queryset = questionDatabase.getAllQuesions();

        system("cls");
        std::cout << "------ All Questions ------\n";
        for (int i = 0; i < queryset.size(); i++) {
            std::cout << "ID: " << queryset[i].id << "\n\n";
            std::cout << "Question subject: \n"<< queryset[i].subject << "\n\n";
            std::cout << "Question text:\n" << queryset[i].questionText << "\n\n";
            std::cout << "Question options:\n";
            for (int j = 0; j < queryset[i].options.size(); j++) {
                std::cout << (char)('a'+j) << ") " << queryset[i].options[j] << "\n";
            }
            std::cout << "\nCorrect options:\n";
            for (int j = 0; j< queryset[i].answer.size(); j++) {
                std::cout << queryset[i].answer[j] << " ";
            }
            std::cout << "\n";
            std::cout << "---------------------------\n";
        }
        system("pause");
    }

    void deleteQuestion() {
        QuestionDatabase questionDatabase;
        auto queryset = questionDatabase.getAllQuesions();

        system("cls");
        std::cout << "------ All Questions ------\n";
        for (int i = 0; i < queryset.size(); i++) {
            std::cout << "ID: " << queryset[i].id << "\n\n";
            std::cout << "Question text:\n" << queryset[i].questionText << "\n\n";
            std::cout << "Question options:\n";
            for (int j = 0; j < queryset[i].options.size(); j++) {
                std::cout << (char)('a'+j) << ") " << queryset[i].options[j] << "\n";
            }
            std::cout << "\nCorrect options:\n";
            for (int j = 0; j< queryset[i].answer.size(); j++) {
                std::cout << queryset[i].answer[j] << " ";
            }
            std::cout << "\n";
            std::cout << "---------------------------\n";
        }

        std::cin.getline(bufferRead, READ_BUFFER_SIZE);
        questionDatabase.deleteQuestion(std::stoi(bufferRead));
    }

};





#endif