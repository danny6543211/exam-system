#include <iostream>
#include <windows.h>
#include "./include/question.h"
// #include "./include/exam.h"


class ExamSystem {
public:
    ExamSystem() {}
    ~ExamSystem() {}
    
    void run() {
        char bufferRead[READ_BUFFER_SIZE];
        QuestionManagement questionManagement;

        while (true) {
            system("cls");
            printHead();
            printMenu();
            
            std::cin.getline(bufferRead, READ_BUFFER_SIZE);
            switch (bufferRead[0]) {
            case '1':
                // exam...
                break;
            case '2':
                questionManagement.run();
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
        std::cout << "------ Exam System ------\n\n";
    }

    void printMenu() {
        system("cls");
        printHead();
        std::cout << "1) Commence Exam\n";
        std::cout << "2) Question Management\n";
        std::cout << "q) Quit\n";
        std::cout << "\n";
    }
};


int main() {
    ExamSystem examSystem;
    examSystem.run();

    return 0;
}