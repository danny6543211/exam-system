#ifndef QUESTION_H
#define QUESTION_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
#include "sqlite3.h"

const size_t READ_BUFFER_SIZE = 256;
const char* DATABASE_FILE = "db.sqlite3";

struct question {
    std::string question_text;
    std::vector<std::string> options_text;
    std::vector<int> correct_options;

    question() {}

    question(
        std::string question_text, 
        std::vector<std::string> options_text, 
        std::vector<int> correct_options) 
    : question_text(question_text), options_text(options_text), correct_options(correct_options) {}    
    
    question(
        std::string question_text, 
        std::initializer_list<std::string> options_text, 
        std::initializer_list<int> correct_options)
    : 
    question_text(question_text), options_text(options_text), correct_options(correct_options) {}

    void reset() {
        question_text = "";
        options_text.clear();
        correct_options.clear();
    }

    ~question() {}
};


class question_database {
private:
    sqlite3* db;

public:
    question_database() {
        int rc = sqlite3_open(DATABASE_FILE, &db);
        if (rc != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));
    }

    ~question_database() {
        sqlite3_close(db);
    }

    void add_question(question& question) {
        std::string correct_options_str;
        for (int i : question.correct_options) {
            correct_options_str += std::to_string(i) + ".";
        } 
        correct_options_str.pop_back();

        std::string question_options_text_str;
        for (std::string s : question.options_text) {
            question_options_text_str += s + " ";
        } 
        question_options_text_str.pop_back();
        
        std::string sql = "insert into question (question_text, question_options_text, question_correct_options)";
        sql += "values ('" + question.question_text + "',";
        sql += "'" + question_options_text_str + "',";
        sql += "'" + correct_options_str + "');";
        
        int rc = sqlite3_exec(db, sql.c_str(), 
            [](void* data, int argc, char** argv, char** colName) -> int {
                return 0;
            }, nullptr, nullptr);
        
        
        if (rc != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));
    }

    const std::vector<question> get_questions() {
        std::vector<question> questions;

        int rc = sqlite3_exec(db, "select * from question", 
            [](void* data, int argc, char** argv, char** colName) -> int {
                std::vector<question>* questions = static_cast<std::vector<question>*>(data);
                question container;
                container.question_text = argv[1];

                std::string options_text_str = argv[2];
                size_t pos = 0;
                std::string delimiter = " ";
                std::string token;
                while ((pos = options_text_str.find(delimiter)) != std::string::npos) {
                    token = options_text_str.substr(0, pos);
                    container.options_text.push_back(token);
                    options_text_str.erase(0, pos + delimiter.length());
                }

                std::string correct_options_str = argv[3];
                pos = 0;
                delimiter = ".";
                while ((pos = correct_options_str.find(delimiter)) != std::string::npos) {
                    token = correct_options_str.substr(0, pos);
                    container.correct_options.push_back(std::stoi(token));
                    correct_options_str.erase(0, pos + delimiter.length());
                }

                questions->push_back(container);
                
                return 0;
            }, &questions, nullptr);
        
        if (rc != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));

        return std::move(questions);
    } 


};

class question_management {
private:
    char buffer_read[READ_BUFFER_SIZE];

    void print_head() {
        std::cout << "------ Question Management ------\n\n";
    }

    void question_management_menu() {
        system("cls");
        print_head();
        std::cout << "1) Add a new question\n";
        std::cout << "2) Display all questions\n";
        std::cout << "q) Back to menu\n";
    }

    void add_new_question() {
        question question;

        auto read_question_text = [&]()->int {
            system("cls");
            print_head();
            std::cout << "Input the description of question.\n";
            std::cout << "\n:";
            std::cin.getline(buffer_read, READ_BUFFER_SIZE);
            question.question_text = buffer_read;

            if (question.question_text.length() == 0)
                return 1;
            else
                return 0;
        };
        
        auto read_question_options_text = [&]()->int {
            system("cls");
            print_head();
            while (true) {
                std::cout << "Input the option of question, enter '#' to quit\n";
                std::cout << "\n:";
                std::cin.getline(buffer_read, READ_BUFFER_SIZE);
                if (buffer_read[0] == '#')
                    break;
                
                question.options_text.push_back(buffer_read);
            }
            
            if (question.options_text.size() == 0)
                return 1;
            else 
                return 0;
        };

        auto read_correct_options = [&]()->int {
            system("cls");
            print_head();
            while (true) {
                std::cout << "Input the index of correct option, enter '#' to quit\n";
                std::cout << "\n:";
                std::cin.getline(buffer_read, READ_BUFFER_SIZE);
                if (buffer_read[0] == '#')
                    break;
                try {
                    int index = std::stoi(buffer_read);
                    question.correct_options.push_back(index);
                } catch (std::exception e) {
                    return 1;
                }
            }
            
            if (question.correct_options.size() == 0)
                return 1;
            else 
                return 0;
        };
        
        while (true) {
            // 还没打印错误......
            if (read_question_text() || read_question_options_text() || read_correct_options())
                std::cout << "Text can not be empty, please input again\n";
            else {
                print_head();
                std::cout << "Please check the input question:\n\n";
                std::cout << "Question text:\n" << question.question_text << "\n\n";
                std::cout << "Question options:\n";
                for (int i = 0; i < question.options_text.size(); i++) {
                    std::cout << i+1 << ") " << question.options_text[i] << "\n";
                }
                std::cout << "\nCorrect options:\n";
                for (int i = 0; i< question.correct_options.size(); i++) {
                    std::cout << question.correct_options[i] << " ";
                } 
                std::cout << "\n\n";
                std::cout << "1) Confirm submission\n";
                std::cout << "2) Enter again\n";
                std::cout << "\n:";
                std::cin.getline(buffer_read, READ_BUFFER_SIZE);
                if (buffer_read[0] == '1') {
                    question_database question_database;
                    question_database.add_question(question);
                    break;
                } else {
                    question.reset();
                } 
            }
        }
    }   

    void display_all_questions() {
        question_database question_database;
        auto queryset = question_database.get_questions(); 

        system("cls");
        std::cout << "------ All Questions ------\n";
        for (int i = 0; i < queryset.size(); i++) {
            std::cout << "ID: " << i+1 << "\n\n";
            std::cout << "Question text:\n" << queryset[i].question_text << "\n\n";
            std::cout << "Question options:\n";
            for (int i = 0; i < queryset[i].options_text.size(); i++) {
                std::cout << i+1 << ") " << queryset[i].options_text[i] << "\n";
            }
            std::cout << "\nCorrect options:\n";
            for (int i = 0; i< queryset[i].correct_options.size(); i++) {
                std::cout << queryset[i].correct_options[i] << " ";
            }
            std::cout << "\n";
            std::cout << "---------------------------\n";
        }

        std::cout << "Press enter to continue\n:";
        std::cin.getline(buffer_read, READ_BUFFER_SIZE);
    } 

public:
    question_management() {}

    ~question_management() {}

    void run() {
        while (true) {
            question_management_menu();

            std::cout << "\n:";
            std::cin.getline(buffer_read, READ_BUFFER_SIZE);
            switch (buffer_read[0]) {
                case '1':
                    add_new_question();
                    break;
                
                case '2':
                    display_all_questions();
                    break;

                case 'q':
                case 'Q':
                    return;
                    break;

                default:
                    break;
            }
        }
    }
};


#endif