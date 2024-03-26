#include <iostream>
#include "./include/database.hpp"


int main() {
    Database db;

    auto result = db.executeQuery("SELECT * FROM exam_question;");
    result.show();

}