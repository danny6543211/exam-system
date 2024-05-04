#include <iostream>
#include <windows.h>
#include "./include/question.h"
#include "./include/exam.h"


int main() {
    const size_t READ_BUFFER_SIZE = 256;

    char buffer_read[READ_BUFFER_SIZE];
    question_management question_management;

    while (true) {
        system("cls");
        std::cout << "------ Exam System ------\n\n";
        std::cout << "1) Commence Exam\n";
        std::cout << "2) Question Management\n";
        std::cout << "q) Quit\n";


        std::cout << "\n:";
        std::cin.getline(buffer_read, READ_BUFFER_SIZE);
        switch (buffer_read[0]) {
            case '1':
                // 添加考试模块......
                //exam.run1();
                break;
            
            case '2':
                question_management.run();
                break;

            case 'q':
            case 'Q':
                return 0;
                break;

            default:
                break;
        }
    }

    return 0;
}