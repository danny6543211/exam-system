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

struct question {                                   //问题结构体
    std::string question_text;
    std::vector<std::string> options_text;
    std::vector<int> correct_options;

    question() {}                  //无参构造

    question(                                  //有参构造
        std::string question_text, 
        std::vector<std::string> options_text, 
        std::vector<int> correct_options) 
    : question_text(question_text), options_text(options_text), correct_options(correct_options) {}    
    
    question(
        std::string question_text, 
        std::initializer_list<std::string> options_text,          //初始化列表，无法修改元素
        std::initializer_list<int> correct_options)
    : 
    question_text(question_text), options_text(options_text), correct_options(correct_options) {}

    void reset() {                                //重新set，清空现有元素
        question_text = "";
        options_text.clear();
        correct_options.clear();
    }

    ~question() {}             //析构
};


class question_database {                //数据库表格创建
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

    void add_question(question& question)  //数据库加入问题
    {    
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
        sql += "values ('" + question.question_text + "',";        //加入问题，用value保存
        sql += "'" + question_options_text_str + "',";
        sql += "'" + correct_options_str + "');";
        
        int rc = sqlite3_exec(db, sql.c_str(), 
            [](void* data, int argc, char** argv, char** colName) -> int {
                return 0;
            }, nullptr, nullptr);
        
        
        if (rc != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));
    }

    /*void delete_questions(index2)
    {

    }*/





    const std::vector<question> get_questions() {       //从数据库中拿问题
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


};                                        //数据库结束






class question_management {                 //问题管理
private:
    char buffer_read[READ_BUFFER_SIZE];

    void print_head() {
        std::cout << "------ Question Management ------\n\n";//构造屏幕
    }

    void question_management_menu() {
        system("cls");                            //清屏（上一个页面）
        print_head();
        std::cout << "1) Add a new question\n";
        std::cout << "2) Display all questions\n";
        std::cout <<"3) Delete questions\n";
        std::cout << "q) Back to menu\n";           //重新构造屏幕
    }

    void add_new_question() {           //加入问题
        question question;

        auto read_question_text = [&]()->int {      //匿名小函数，lambda名read-question=text 可在域内多次调用【&】捕获语句  返回类型int
            system("cls");
            print_head();
            std::cout << "Input the description of question.\n";
            std::cout << "\n:";
            std::cin.getline(buffer_read, READ_BUFFER_SIZE);//读文本和大小
            question.question_text = buffer_read;

            if (question.question_text.length() == 0)
                return 1;    //失败(缺打印错误)
            else
                return 0;//成功
        };
        
        auto read_question_options_text = [&]()->int {//读选项lambda函数
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
                    int index = std::stoi(buffer_read);         //将字符串转为int
                    question.correct_options.push_back(index);
                } 
                 catch (std::exception e) //描述异常的字符串
                 {
                    return 1;
                }
            }
            
            if (question.correct_options.size() == 0)
                return 1;
            else 
                return 0;
        };
        
        while (true) {
            // 还没打印错误......啥意思？？？？？这样吗
            /*if(read_question_text())  std::cout<<"Question can not be empty, please input again\n";
            else if(read_question_options_text()) std::cout<<"Options can not be empty, please input again\n";
            else if(read_correct_options()) std::cout<<"Correct_options can not be empty, please input again\n";*/
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
                if (buffer_read[0] == '1') 
                {
                    question_database question_database;
                    question_database.add_question(question);      //最终addquestion数据库函数
                    break;
                } 
                else 
                {
                    question.reset();
                } 
            }
        }
    }                        //加入问题结束

    void display_all_questions()   //展示问题
    {
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

    void delete_question()          //删除问题
   {
        question_database question_database;
        auto queryset = question_database.get_questions(); 

        system("cls");
        std::cout<<"the ID of the question you want to delete:\n";
        std::cin.getline(buffer_read, READ_BUFFER_SIZE);
        int index2 = std::stoi(buffer_read);
        /*for (int i = index2; i < queryset.size(); i++)
        {
            queryset[i].question_text=queryset[i+1].question_text;
            for (int j = 0; j < queryset[i+1].options_text.size(); j++) {
               queryset[i].options_text[j]=queryset[i+1].options_text[j];
            }
            for (int j = 0; j< queryset[i+1].correct_options.size(); j++) {
                queryset[i].correct_options[j]=queryset[i+1].correct_options[j];
            }
        }
        queryset[queryset.size()-1].question_text = "";
        queryset[queryset.size()-1].options_text.clear();
        queryset[queryset.size()-1].correct_options.clear();*/


       // question_database.delete_questions(index2);
   }




public:
    question_management() {}

    ~question_management() {}

    void run() 
    {
        while (true) 
        {
            question_management_menu();      //实际输入数字界面运行段

            std::cout << "\n:";
            std::cin.getline(buffer_read, READ_BUFFER_SIZE);
            switch (buffer_read[0]) 
            {
                case '1':
                    add_new_question();
                    break;
                
                case '2':
                    display_all_questions();
                    break;
                case '3':
                    delete_question();
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