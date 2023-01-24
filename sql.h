#pragma once


bool userIsExist(char* userName,MYSQL mysql);
bool passwordIsRight(char* userName,char* userPassword,MYSQL mysql);
double checkAmount(char* userName,MYSQL mysql);
bool changeAmount(char* userName,double amount,MYSQL mysql);
void createAccount(char* userName,char* userPassword, MYSQL mysql);
