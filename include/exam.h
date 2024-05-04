#ifndef EXAM_H         //确保只定义一次
#define EXAM_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <windows.h>
#include "sqlite3.h"
using namespace std;



class exam 
{
    private:
    char buffer_read[READ_BUFFER_SIZE];
    std::set<int> r;
    std::vector<int> useranswer;
    int N;



    void print_head() {
        std::cout << "------ About exam ------\n\n";//构造屏幕
    }

    void exam_menu() {
        system("cls");                            //清屏（上一个页面）
        print_head();
        std::cout << "1) choose your subjects\n";
        std::cout << "2) begin! \n";
        std::cout << "q) Back to menu\n";           //重新构造屏幕
    }

    void choice_subject(){
        //这个也要加标签，数据库的，不会。。。。
    }



    
    void select_N()         //可构建子类
    {
        question_database question_database;
    auto queryset = question_database.get_questions();
        while(1)
      {

        system("cls");
        print_head();
        std::cout << "请输入n:\n:";
        std::cin.getline(buffer_read, READ_BUFFER_SIZE);
         N= std::stoi(buffer_read);
        if (N>queryset.size()) 
        {
            std::cout<<"your n is so big!\n";
        }
        else
        {
              auto read_question_options_text = [&]()->int 
              { 
                //生成有序随机数r队列 
		       int m,r_length;
              
   		            while(N)
   		        {
   	 		        m=rand()%(queryset.size()+1);    //a,b为第一题下标和最后一题下标 
   	 		        r_length=r.size();
   	 		        r.insert(m);
   	 		        if(r.size()!=r_length) N--;
   		        }
                if(r.size()==0) return 1;
                else return 0;
	        };

            break;
        }

      }
    }



        void answer_question()
        {
            question_database question_database;
            auto queryset = question_database.get_questions();
            system("cls");
            std::cout<<"Let us begin!\n\n";
            std::cout << "Press enter to continue\n:";
            std::cin.getline(buffer_read, READ_BUFFER_SIZE);
            int correct1=0;
            for(auto r3 : r)              //答题
            {
                system("cls");
                print_head();
               std::cout << "ID: " << r3+1 << "\n\n";
               std::cout << "Question text:\n" << queryset[r3].question_text << "\n\n";
               std::cout << "Question options:\n";
               for (int i = 0; i < queryset[r3].options_text.size(); i++) {
                std::cout << i+1 << ") " << queryset[r3].options_text[i] << "\n";
            }

                //以下作答
                int k=0,count=0;
                 while (true) 
                 {
                    system("cls");
                   print_head();
                    std::cout<<"\n\nwrite down your answer\n(if there are more than one,please write in turn:1-A,2-B,3-C,4-D)\n:";
                    std::cin.getline(buffer_read, READ_BUFFER_SIZE);
                    if (buffer_read[0] == '#') break;
                    else
                   {   
                       int index = std::stoi(buffer_read);       
                       if(queryset[r3].correct_options[k++]==index) count++;
                   }
                 }
                 if(count==queryset[r3].correct_options.size()) 
                 {
                    system("cls");
                    print_head();
                    std::cout<<"\n\n\nYou are right!\n\n\n";
                    std::cout << "Press enter to continue:";
                    std::cin.getline(buffer_read, READ_BUFFER_SIZE);
                    correct1++;
                }
                else {
                    system("cls");
                    print_head();
                    std::cout<<"\n\n\nYou are wrong!\n\n\n";
                    std::cout << "Press enter to continue:";
                    std::cin.getline(buffer_read, READ_BUFFER_SIZE);
                }


                //useranswer.push_back(index);
            }
            system("cls");
            print_head();
            std::cout << "The exam is finished!\n\nPress enter to see you score\n:";
            std::cin.getline(buffer_read, READ_BUFFER_SIZE);

            system("cls");
            print_head();
            double correct2=correct1;
            std::cout<<"你的正确率为:";
            printf("%.2f",correct2/N);
            std::cout<<"%\n你的最终得分为:"<<(int)(correct2/N)<<"\n";

        }
      
        void userscore()
        {
            int partscore;
            //可以细分，但是要加单选多选标签

        }



public:
    exam() {}

    ~exam() {}

    void run1() 
    {
        while (true) 
        {
            exam_menu();      //实际输入数字界面运行段

            std::cout << "\n:";
            std::cin.getline(buffer_read, READ_BUFFER_SIZE);
            switch (buffer_read[0]) 
            {
                case '1':
                    choice_subject();
                    break;
                
                case '2':
                    select_N();
                    system("cls");
                    print_head();
                    std::cout << "N道题已经准备好\n\nPress enter to continue:\n";
                    std::cin.getline(buffer_read, READ_BUFFER_SIZE);
                    answer_question();
                    userscore();
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
















/*class choose{//第三个类 ：试题选取,组卷 
private:
	
public:
	int N;
	set<int> r;    //随机题号 
	cin>>N;
	void random1()  //生成随机数（下标未完成） 
	{
		int m,r2;
   		while(N)
   		{
   	 		m=rand()%(b-a+1)+a;    //a,b为第一题下标和最后一题下标 
   	 		r2=r.size();
   	 		r.insert(m);
   	 		if(k.size()!=r2) N--;
   		}
	}
	void choice()		//放到coluName3和data3里面 
	{
		for(auto randnum:r)
		{
			columnName3.pushback()=querySet->columnName[r];
			data3.pushback()=			//复制data内容 
		}
	}
	void show3()
	{
		for (int i = 0; i < columnName3.size(); i++) //输出coluName3
		{
            cout << columnName3[i] << "|";
        } 
        cout << "\n";
        
        for (auto row : querySet->data3) //输出data内容，row（行）第一个维度,item第二个维度vector<string> 
		{
            for (auto item : row) 
			{
                cout << item << "|";
            }
            cout << "\n";
        }
	}
};      

class answer //第四个类：作答
{
	
};    

class correct   //第五个类，改卷，评分 
{
	
};*/

#endif