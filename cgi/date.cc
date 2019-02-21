#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include"date.hpp"

using namespace std;
int main()
{
    string param_;
	char c;
    while(read(0, &c, 1) > 0)
    {
		param_.push_back(c);
    };
	
	int y1, m1, day1, y2, m2, day2;
	Date d1, d2;

	sscanf(param_.c_str(), "y1=%d&m1=%d&day1=%d&y2=%d&m2=%d&day2=%d", &y1, &m1, &day1,&y2, &m2, &day2);
	d1.SetDate(y1, m1, day1);
	d2.SetDate(y2, m2, day2);

	cout<<"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">";
	cout<<"<title>日期计算器</title>";
	cout<<"</head><body><div style=\"height:90px; \">";
	cout<<"</div>";
	cout<<"<b>计算两个日期之间差多少天</b>";
	cout<<"<br>";
	cout<<"<form action=\"date\" method=\"POST\">";
	cout << "<input type=\"text\" name=\"y1\" value=\"" << y1 << "\" style = \"background-color: rgb(242, 242, 242);\" size = \"5\">年 <input type = \"text\" name = \"m1\" value = \"" << m1 << "\" style = \"background-color: rgb(242, 242, 242);\" size = \"5\">月";
	cout << "<input type = \"text\" name = \"day1\" value = \"" << day1 << "\" style = \"background-color: rgb(242, 242, 242);\" size = \"5\">日<br>";
	cout << "距 <input type = \"text\" name = \"y2\" value = \"" << y2 << "\" style = \"background-color: rgb(242, 242, 242);\" size = \"5\">年";
	cout << "<input type = \"text\" name = \"m2\" value = \"" << m2 << "\" style = \"background-color: rgb(242, 242, 242);\" size = \"5\">月";
	cout << "<input type = \"text\" name = \"day2\" value = \"" << day2 << "\" style = \"background-color: rgb(242, 242, 242);\" size = \"5\">日<br>";
	cout<< "<input type = \"submit\" value = \"相差\">";
	cout << "<span>" << d1 - d2 << "天</span>";
	cout<<"</form>";
	cout<<"</body></html>";
	
    return 0;
}

