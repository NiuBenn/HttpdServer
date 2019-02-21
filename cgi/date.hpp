#pragma once

//是否为闰年，是闰年返回 真，不是闰年返回 假
bool Isleapyear(int year)//是否为闰年，是闰年返回 真，不是闰年返回 假
{
	if ((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0))
	{
		return true;
	}
	return false;
}

int GetMonthDay(int year, int month)//获取每个月对应的天数
{
	static int MonthDay[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
	int day = 28;
	if (month == 2 && Isleapyear(year))
	{
		day += 1;
		return day;
	}
	else
	{
		return MonthDay[month];
	}
}

class Date
{
private:
	int _year;//年
	int _month;//月
	int _day;//日

public:
	//构造函数(缺省参数)，默认值为当天的日期
	Date(int year = 1999, int month = 05, int day = 28)
	{
		_year = year;
		_month = month;
		_day = day;
	}
	//拷贝构造函数
	Date(const Date& d) 
	{ 
		_year = d._year;         
		_month = d._month;         
		_day = d._day; 
	}
	//赋值运算符重载
	Date& operator = (const Date& d) 
	{
		if (this != &d)         
		{             
			this->_year = d. _year;              
			this->_month = d. _month;              
			this->_day = d. _day;       
		}
		return *this;
	}
	//＋运算符重载
	Date& operator +(int day)
	{
		_day += day;
		//1.day值为负 +=后 _day 不为正( <1 )
		//2.day值为正 +=后 _day 大于当前月day数( >GetMonthDay )
		//3.day值不管为正为负 +=后  1<day <GetMonthDay (这种情况不作处理)  
		while(_day < 1)//第1种情况
		{
			_month--;
			if (_month < 1)
			{
				_year--;
				_month += 12;
			}
			_day += GetMonthDay(_year, _month);
			/*
			要注意GetMonthDay是要获取前一月day数
			所以要先将month--然后判断年，将年月调整好后才能get
			否则可能month为0时get，这样会导致数据出错
			*/
		}

		while(_day > GetMonthDay(_year, _month))//第2种情况
		{
			_day -= GetMonthDay(_year, _month);
			/*
			这里是GetMonthDay获取当前月day数
			所以这块直接减掉当前月day数
			在后面再对月调整，
			调完月，如果月数大于12，调整年同时调整月
			*/
			_month++;
			if (_month > 12)
			{
				_year++;
				_month -= 12;
			}
		}
		return *this;//返回this指针
	}
	// == 运算符重载
	bool operator == (const Date& d)
	{
		if (this->_year == d._year
			&&this->_month == d._month
			&&this->_day == d._day)
			return true;
		else
			return false;
	}
	// > 运算符重载
	bool operator > (const Date& d)
	{
		if (this->_year > d._year)
		{
			return true;
		}
		else if (this->_year < d._year)
		{
			return false;
		}
		else
		{
			if (this->_month > d._month)
			{
				return true;
			}
			else if (this->_month < d._month)
			{
				return false;
			}
			else
			{
				if (this->_day > d._day)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	// < 运算符重载
	bool operator < (const Date& d)
	{
		if (this->_year < d._year)
		{
			return true;
		}
		else if (this->_year > d._year)
		{
			return false;
		}
		else
		{
			if (this->_month < d._month)
			{
				return true;
			}
			else if (this->_month > d._month)
			{
				return false;
			}
			else
			{
				if (this->_day < d._day)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	// -运算符重载
	int operator - (const Date& d)
	{
		Date big, small;
		int count = 0;
		if (operator>(d))
		{

			big = *this;
			small = d;
		}
		else
		{
			big = d;
			small = *this;
		}
		while (!(small.operator==(big)))
		{
			small = (small + 1);
			count++;
		}
		return count;
	}
	
	int GetYear()
	{
		return _year;
	}
	int GetMonth()
	{
		return _month;
	}
	int GetDay()
	{
		return _day;
	}
	//设置日期
	bool SetDate(int year, int month, int day)//设置日期
	{
		if (year>1900 && month >= 1 && month <= 12 && day >= 1 && day <= GetMonthDay(year, month))
		{
			_year = year;
			_month = month;
			_day = day;
			return true;
		}
		else
		{
			return false;
		}
	}
};


