#include <iostream>
#include <string>
//#include <sstring>
#include "stdio.h"
#include "mysql.h"
#include "stdlib.h"
#include "sql.h"

using namespace std;


bool userIsExist(char* userName,MYSQL mysql){
	
	MYSQL_RES *res = nullptr;
	MYSQL_ROW row;
	
	char sql[1024]{0};
	string insert = userName;
	//snprintf(sql,1024,"select * from alluserinfo");
	
	string tmp = "";
	tmp += "SELECT count(*) ";
	tmp += "FROM alluserinfo ";
	tmp += "WHERE user_id ='";
	tmp += insert;
	tmp += "' limit 1";
	strcpy(sql,tmp.c_str());
	
	if(mysql_real_query(&mysql,sql,(unsigned int)strlen(sql))){
		cout<<"userIsExist fail: "<<mysql_errno(&mysql)<<endl;
	}
	else{
		cout<<"userIsExist got in"<<endl;
		res = mysql_store_result(&mysql);
		if(nullptr == res){
			cout<<"userIsExist failed: "<<mysql_errno(&mysql)<<endl;
		}
		else{
				row = mysql_fetch_row(res);
				if(atoi(row[0]) == 1){
					printf("The data exists!\n");
					mysql_free_result(res);
					return true;
				}
				else{
					printf("The data not exists!\n");
				}
		}
	}
	mysql_free_result(res);
	return false;
}


bool passwordIsRight(char* userName,char* userPassword,MYSQL mysql){
	MYSQL_RES *res = nullptr;
	MYSQL_ROW row;
	
	char sql[1024]{0};
	string insert = userName;
	string cmp = userPassword;
	//snprintf(sql,1024,"select * from alluserinfo");
	
	string tmp = "";
	tmp += "SELECT user_password ";
	tmp += "FROM alluserinfo ";
	tmp += "WHERE user_id ='";
	tmp += insert;
	tmp += "'";
	strcpy(sql,tmp.c_str());
	
	if(mysql_real_query(&mysql,sql,(unsigned int)strlen(sql))){
		cout<<"passwordIsRight fail: "<<mysql_errno(&mysql)<<endl;
	}
	else{
		cout<<"passwordIsRight got in"<<endl;
		res = mysql_store_result(&mysql);
		if(nullptr == res){
			cout<<"passwordIsRight failed: "<<mysql_errno(&mysql)<<endl;
		}
		else{
				cout<<"passwordIsRight success"<<endl;
				row = mysql_fetch_row(res);
				mysql_free_result(res);
				cout<<"OK here"<<endl;
				string take(row[0]);
				if(take == cmp) return true;
		}
	}
	mysql_free_result(res);
	return false;
}

double checkAmount(char* userName,MYSQL mysql){
	MYSQL_RES *res = nullptr;
	MYSQL_ROW row;
	
	char sql[1024]{0};
	double amount = -1;
	string insert = userName;
	//snprintf(sql,1024,"select * from alluserinfo");
	
	string tmp = "";
	tmp += "SELECT amount ";
	tmp += "FROM alluserinfo ";
	tmp += "WHERE user_id ='";
	tmp += insert;
	tmp += "'";
	strcpy(sql,tmp.c_str());
	
	if(mysql_real_query(&mysql,sql,(unsigned int)strlen(sql))){
		cout<<"checkAmount fail: "<<mysql_errno(&mysql)<<endl;
	}
	else{
		cout<<"checkAmount got in "<<endl;
		res = mysql_store_result(&mysql);
		if(nullptr == res){
			cout<<"CheckAmount fail: "<<mysql_errno(&mysql)<<endl;
		}
		else{
			cout<<"checkAmount success"<<endl;
			row = mysql_fetch_row(res);
			amount = strtod(row[0],NULL);
		}
	}
	mysql_free_result(res);
	return amount;
}

bool changeAmount(char* userName,double amount,MYSQL mysql){
	
	MYSQL_RES *res = nullptr;
	MYSQL_ROW row;
	
	char sql[1024]{0};
	string amo = std::to_string(amount);
	string insert = userName;
	//snprintf(sql,1024,"select * from alluserinfo");
	
	string tmp = "";
	tmp += "UPDATE alluserinfo SET amount=";
	tmp += amo;
	tmp += " WHERE user_id='";
	tmp += insert;
	tmp += "'";
	strcpy(sql,tmp.c_str());
	
	if(mysql_real_query(&mysql,sql,(unsigned int)strlen(sql))){
		cout<<"changeAmount fail: "<<mysql_errno(&mysql)<<endl;
	}
	else{
		cout<<"changeAmount got in "<<endl;
		res = mysql_store_result(&mysql);
		if(mysql_real_query(&mysql,sql,(unsigned int)strlen(sql))){
		cout<<"ChangeAmount fail: "<<mysql_errno(&mysql)<<endl;
		}
		else{
		res = mysql_store_result(&mysql);
		printf("ChangeAmount success.\n");
		mysql_free_result(res);
		return true;
		}
	}
	mysql_free_result(res);
	return false;
}

void createAccount(char* userName,char* userPassword, MYSQL mysql){
	MYSQL_RES *res = nullptr;
	MYSQL_ROW row;
	
	char sql[1024]{0};
	string tmp = "";
	string name = userName;
	string word = userPassword;
	tmp += "INSERT INTO alluserinfo (user_id,user_password) VALUES";
	tmp += "('";
	tmp += name;
	tmp += "','";
	tmp += word;
	tmp += "')";
	
	strcpy(sql,tmp.c_str());
	if(mysql_real_query(&mysql,sql,(unsigned int)strlen(sql))){
		cout<<"Create fail: "<<mysql_errno(&mysql)<<endl;
	}
	else{
		res = mysql_store_result(&mysql);
		printf("Create success.\n");
	}
	mysql_free_result(res);
	
}
