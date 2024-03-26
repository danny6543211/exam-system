#include <iostream>
#include "./include/database.hpp"


int main() {
    Database db;

    auto result = db.executeQuery("select * from exam_question;");
    result.show();

}