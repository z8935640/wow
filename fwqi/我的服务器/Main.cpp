//#include "头文件.h"
//#include "mysql.h"  
//
//const char 数据库IP[] = "rm-8vb3995wnq311i3a4eo.mysql.zhangbei.rds.aliyuncs.com";		//外网IP
////const char 数据库IP[] = "rm-8vb3995wnq311i3a4.mysql.zhangbei.rds.aliyuncs.com";		//内网IP
//const char 用户名[] = "thorn_1";
//const char 密码[] = "Mh0922314";
//const char 数据库[] = "thorn";
//
//
//int 初始化数据库()
//{
//	MYSQL myCont;
//	if (mysql_init(&myCont) == NULL)//初始化mysql
//	{
//		printf("初始化mysql失败!");
//		return -11;
//	}
//	if (!mysql_real_connect(&myCont, 数据库IP, 用户名, 密码, 数据库, 3306, NULL, 0))
//	{
//		printf("登录mysql失败!");
//		mysql_close(&myCont);
//		return 0;
//	}
//}
//

#include "app.h"
int main()
{
	App app;
	app.Run("127.0.0.1", 8888);
	//app.Run("0.0.0.0", 3389);
	//getchar();
	return 0;
}