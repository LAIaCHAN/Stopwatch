/*--包含头文件--*/
#include <reg52.h>  // 包含 8052 单片机头文件

#define GPIO_KEY P3  // 定义按键输入端口

unsigned char Key_Check(void);  // 声明按键检测函数
unsigned char min, sec, msec;   // 分，秒，毫秒
unsigned char showstr[10] = {"Stop Watch"};  // 显示 Stop Watch
unsigned char showdata[9] = {"00:00:000"};     // 显示的数据

void Delay_Ms(int);  // 声明延时函数

#define LCD1602_DB  P0      // LCD1602 数据总线端口

sbit LCD1602_RS = P2^6;  // LCD1602 RS 位
sbit LCD1602_RW = P2^5;  // LCD1602 RW 位
sbit LCD1602_EN = P2^7;  // LCD1602 EN 位

// 函数声明
void Read_Busy();               // LCD忙检测函数
void Lcd1602_Write_Cmd(unsigned char cmd);      // LCD写命令
void Lcd1602_Write_Data(unsigned char dat);     // LCD写数据
void LcdSetCursor(unsigned char x, unsigned char y);  // LCD坐标显示
void Lcd1602_Init();            // LCD1602初始化
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);  // 在LCD上显示一个字符
void Timer_Init(void);  // 定时器初始化

void main()	 
{	 
	unsigned char keynum, i;  // 定义按键值和循环变量

	Timer_Init();  // 初始化定时器
	Lcd1602_Init();  // 初始化LCD1602

	// 在LCD上显示 "Stop Watch"
	for (i = 0; i < 10; i++)
		DisplayOneChar(3 + i, 0, showstr[i]);  

	while(1)
	{
		// 按键扫描
		keynum = Key_Check();
		
		if (keynum == 1)
		{
			TR0 = 1;  // 启动定时器
		}
		else if (keynum == 2)
		{
			TR0 = 0;  // 停止定时器
			showdata[2] = ':';  // 在显示数据中设置冒号
			showdata[5] = ':';	
		}
		else if (keynum == 3)
		{
			TR0 = 0;  // 停止定时器
			min = 0;  // 复位到00分00秒00
			sec = 0;
			msec = 0;
			showdata[2] = ':';  // 在显示数据中设置冒号
			showdata[5] = ':';	
		}
		
		// 数据更新
		showdata[0] = min / 10 + '0';
		showdata[1] = min % 10 + '0';
		showdata[3] = sec / 10 + '0';
		showdata[4] = sec % 10 + '0';
		showdata[6] = msec / 100 + '0';
		showdata[7] = msec % 10 + '0';
		showdata[8] = msec %10+ '0';
		
		// 在LCD上显示时间数据
		for (i = 0; i < 9; i++)
			DisplayOneChar(4 + i, 1, showdata[i]);						
	}			
}

void Timer0() interrupt 1       //定时器中断函数 
{    
    TH0 = 0xfc;  	// 设置定时器1的高8位初值，实现定时1ms
    TL0 = 0x18;  	// 设置定时器1的低8位初值

    if (msec < 999) {
        msec++;	// 毫秒加1
    } else if (sec < 59) {
        sec++;	// 秒加1
        msec = 0;	// 毫秒复位为0
    } else if (min < 59) {
        min++;	// 分钟加1
        sec = 0;	// 秒复位为0
        msec = 0;	// 毫秒复位为0
    } else {
        TR0 = 0;	// 如果小时达到99时，停止定时器
    }
}

void Delay_Ms(int m)  // 延时m毫秒
{
    unsigned char a, b, c;  // 定义三个无符号字符型变量

    for (c = m; c > 0; c--)  // 外层循环，控制延时的总毫秒数
        for (b = 242; b > 0; b--)  // 中层循环
            for (a = 4; a > 0; a--);  // 内层循环
}

void Read_Busy()           
{
    unsigned char sta;     // 定义一个无符号字符型变量sta，用于存储LCD状态寄存器的值

    LCD1602_DB = 0xff;      // 将数据总线置高，准备读取LCD状态
    LCD1602_RS = 0;         // 将RS（寄存器选择引脚）置低，选择指令寄存器，因为我们要读取的是LCD的状态
    LCD1602_RW = 1;         // 将RW（读写控制引脚）置高，表示读操作

    do
    {
        LCD1602_EN = 1;     // 使能，将EN引脚置高，表示启用LCD
        sta = LCD1602_DB;   // 读取LCD状态寄存器的值，包括忙位（bit 7）
        LCD1602_EN = 0;     // 使能完毕后，将EN引脚拉低，释放总线
    } while(sta & 0x80);    // 当LCD处于忙状态时，sta的bit 7（忙位）为1，循环等待忙位为0
}

// LCD写命令
void Lcd1602_Write_Cmd(unsigned char cmd)     
{
    Read_Busy();            // 调用忙检测函数，确保LCD处于非忙状态
    LCD1602_RS = 0;         // 将RS引脚置低，选择指令寄存器
    LCD1602_RW = 0;         // 将RW引脚置低，表示写操作
    LCD1602_DB = cmd;       // 将命令数据写入数据总线
    LCD1602_EN = 1;         // 使能，将EN引脚置高，表示启用LCD
    LCD1602_EN = 0;         // 使能完毕后，将EN引脚拉低，释放总线
}

// LCD写数据
void Lcd1602_Write_Data(unsigned char dat)   
{
    Read_Busy();            // 调用忙检测函数，确保LCD处于非忙状态
    LCD1602_RS = 1;         // 将RS引脚置高，选择数据寄存器
    LCD1602_RW = 0;         // 将RW引脚置低，表示写操作
    LCD1602_DB = dat;       // 将数据写入数据总线
    LCD1602_EN = 1;         // 使能，将EN引脚置高，表示启用LCD
    LCD1602_EN = 0;         // 使能完毕后，将EN引脚拉低，释放总线
}

// 按指定位置显示一个字符
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
    Y &= 0x1;               // 将Y与0x1进行按位与操作，确保Y只能取0或1，只能是第一行或第二行
    X &= 0xF;               // 将X与0xF进行按位与操作，限制X不能大于15
    if (Y) X |= 0x40;       // 当要显示第二行时，将地址码X加上0x40
    X |= 0x80;              // 算出指令码，将地址码X加上0x80，显示位置命令格式为1+地址码(0x80+X)
    Lcd1602_Write_Cmd(X);   // 发送命令字，设置显示位置
    Lcd1602_Write_Data(DData); // 发送数据，显示字符
}

// LCD1602初始化
void Lcd1602_Init()              
{
    Lcd1602_Write_Cmd(0x38);    // 打开显示，设置显示模式为5*8点阵，使用8位数据总线
    Lcd1602_Write_Cmd(0x0c);    // 开启显示，关闭光标，关闭光标闪烁
    Lcd1602_Write_Cmd(0x06);    // 设置光标移动方向为右，不开启显示移位
    Lcd1602_Write_Cmd(0x01);    // 清屏
}

// 定时器初始化
void Timer_Init(void)
{
    TMOD = 0x01;    // 置T0定时工作方式1，使用模式1，16位定时器
    TH0 = 0xfc;     // T0设置初值，定时10ms，高8位初值
    TL0 = 0x18;     // T0设置初值，定时10ms，低8位初值
    ET0 = 1;        // 开T0中断允许，允许T0中断
    TR0 = 0;        // 默认关闭定时器，先置0，等待需要时再打开
    EA = 1;         // 打开总中断，允许所有中断
}

unsigned char Key_Check(void)  // 按键检测函数
{
    static flag;                    // 静态变量，用于记录按键状态，实现消抖
    unsigned char KeyValue = 0;    // 按键值初始化为0

    if ((GPIO_KEY & 0x0f) != 0x0f) // 读取低4位，判断按键是否按下
    {
        if (!flag)
        {
            Delay_Ms(10);            // 延时10ms进行消抖
            if ((GPIO_KEY & 0x0f) != 0x0f) // 再次检测键盘是否按下
            {
                flag = 1;          // 设置flag为1，表示按键已经被检测到
                switch (GPIO_KEY & 0x0f)
                {
                    case (0x0e): KeyValue = 2; break; // 如果按键是0x0e，设置KeyValue为2
                    case (0x0d): KeyValue = 1; break; // 如果按键是0x0d，设置KeyValue为1
                    case (0x0b): KeyValue = 3; break; // 如果按键是0x0b，设置KeyValue为3
                    case (0x07): KeyValue = 4; break; // 如果按键是0x07，设置KeyValue为4
                }
            }
        }
    }
    else
    {
        flag = 0; // 如果按键没有被按下，将flag重置为0，表示可以进行下一次按键检测
    }

    return KeyValue; // 返回按键值
}
