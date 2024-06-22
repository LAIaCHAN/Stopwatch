/*--����ͷ�ļ�--*/
#include <reg52.h>  // ���� 8052 ��Ƭ��ͷ�ļ�

#define GPIO_KEY P3  // ���尴������˿�

unsigned char Key_Check(void);  // ����������⺯��
unsigned char min, sec, msec;   // �֣��룬����
unsigned char showstr[10] = {"Stop Watch"};  // ��ʾ Stop Watch
unsigned char showdata[9] = {"00:00:000"};     // ��ʾ������

void Delay_Ms(int);  // ������ʱ����

#define LCD1602_DB  P0      // LCD1602 �������߶˿�

sbit LCD1602_RS = P2^6;  // LCD1602 RS λ
sbit LCD1602_RW = P2^5;  // LCD1602 RW λ
sbit LCD1602_EN = P2^7;  // LCD1602 EN λ

// ��������
void Read_Busy();               // LCDæ��⺯��
void Lcd1602_Write_Cmd(unsigned char cmd);      // LCDд����
void Lcd1602_Write_Data(unsigned char dat);     // LCDд����
void LcdSetCursor(unsigned char x, unsigned char y);  // LCD������ʾ
void Lcd1602_Init();            // LCD1602��ʼ��
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);  // ��LCD����ʾһ���ַ�
void Timer_Init(void);  // ��ʱ����ʼ��

void main()	 
{	 
	unsigned char keynum, i;  // ���尴��ֵ��ѭ������

	Timer_Init();  // ��ʼ����ʱ��
	Lcd1602_Init();  // ��ʼ��LCD1602

	// ��LCD����ʾ "Stop Watch"
	for (i = 0; i < 10; i++)
		DisplayOneChar(3 + i, 0, showstr[i]);  

	while(1)
	{
		// ����ɨ��
		keynum = Key_Check();
		
		if (keynum == 1)
		{
			TR0 = 1;  // ������ʱ��
		}
		else if (keynum == 2)
		{
			TR0 = 0;  // ֹͣ��ʱ��
			showdata[2] = ':';  // ����ʾ����������ð��
			showdata[5] = ':';	
		}
		else if (keynum == 3)
		{
			TR0 = 0;  // ֹͣ��ʱ��
			min = 0;  // ��λ��00��00��00
			sec = 0;
			msec = 0;
			showdata[2] = ':';  // ����ʾ����������ð��
			showdata[5] = ':';	
		}
		
		// ���ݸ���
		showdata[0] = min / 10 + '0';
		showdata[1] = min % 10 + '0';
		showdata[3] = sec / 10 + '0';
		showdata[4] = sec % 10 + '0';
		showdata[6] = msec / 100 + '0';
		showdata[7] = msec % 10 + '0';
		showdata[8] = msec %10+ '0';
		
		// ��LCD����ʾʱ������
		for (i = 0; i < 9; i++)
			DisplayOneChar(4 + i, 1, showdata[i]);						
	}			
}

void Timer0() interrupt 1       //��ʱ���жϺ��� 
{    
    TH0 = 0xfc;  	// ���ö�ʱ��1�ĸ�8λ��ֵ��ʵ�ֶ�ʱ1ms
    TL0 = 0x18;  	// ���ö�ʱ��1�ĵ�8λ��ֵ

    if (msec < 999) {
        msec++;	// �����1
    } else if (sec < 59) {
        sec++;	// ���1
        msec = 0;	// ���븴λΪ0
    } else if (min < 59) {
        min++;	// ���Ӽ�1
        sec = 0;	// �븴λΪ0
        msec = 0;	// ���븴λΪ0
    } else {
        TR0 = 0;	// ���Сʱ�ﵽ99ʱ��ֹͣ��ʱ��
    }
}

void Delay_Ms(int m)  // ��ʱm����
{
    unsigned char a, b, c;  // ���������޷����ַ��ͱ���

    for (c = m; c > 0; c--)  // ���ѭ����������ʱ���ܺ�����
        for (b = 242; b > 0; b--)  // �в�ѭ��
            for (a = 4; a > 0; a--);  // �ڲ�ѭ��
}

void Read_Busy()           
{
    unsigned char sta;     // ����һ���޷����ַ��ͱ���sta�����ڴ洢LCD״̬�Ĵ�����ֵ

    LCD1602_DB = 0xff;      // �����������øߣ�׼����ȡLCD״̬
    LCD1602_RS = 0;         // ��RS���Ĵ���ѡ�����ţ��õͣ�ѡ��ָ��Ĵ�������Ϊ����Ҫ��ȡ����LCD��״̬
    LCD1602_RW = 1;         // ��RW����д�������ţ��øߣ���ʾ������

    do
    {
        LCD1602_EN = 1;     // ʹ�ܣ���EN�����øߣ���ʾ����LCD
        sta = LCD1602_DB;   // ��ȡLCD״̬�Ĵ�����ֵ������æλ��bit 7��
        LCD1602_EN = 0;     // ʹ����Ϻ󣬽�EN�������ͣ��ͷ�����
    } while(sta & 0x80);    // ��LCD����æ״̬ʱ��sta��bit 7��æλ��Ϊ1��ѭ���ȴ�æλΪ0
}

// LCDд����
void Lcd1602_Write_Cmd(unsigned char cmd)     
{
    Read_Busy();            // ����æ��⺯����ȷ��LCD���ڷ�æ״̬
    LCD1602_RS = 0;         // ��RS�����õͣ�ѡ��ָ��Ĵ���
    LCD1602_RW = 0;         // ��RW�����õͣ���ʾд����
    LCD1602_DB = cmd;       // ����������д����������
    LCD1602_EN = 1;         // ʹ�ܣ���EN�����øߣ���ʾ����LCD
    LCD1602_EN = 0;         // ʹ����Ϻ󣬽�EN�������ͣ��ͷ�����
}

// LCDд����
void Lcd1602_Write_Data(unsigned char dat)   
{
    Read_Busy();            // ����æ��⺯����ȷ��LCD���ڷ�æ״̬
    LCD1602_RS = 1;         // ��RS�����øߣ�ѡ�����ݼĴ���
    LCD1602_RW = 0;         // ��RW�����õͣ���ʾд����
    LCD1602_DB = dat;       // ������д����������
    LCD1602_EN = 1;         // ʹ�ܣ���EN�����øߣ���ʾ����LCD
    LCD1602_EN = 0;         // ʹ����Ϻ󣬽�EN�������ͣ��ͷ�����
}

// ��ָ��λ����ʾһ���ַ�
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
    Y &= 0x1;               // ��Y��0x1���а�λ�������ȷ��Yֻ��ȡ0��1��ֻ���ǵ�һ�л�ڶ���
    X &= 0xF;               // ��X��0xF���а�λ�����������X���ܴ���15
    if (Y) X |= 0x40;       // ��Ҫ��ʾ�ڶ���ʱ������ַ��X����0x40
    X |= 0x80;              // ���ָ���룬����ַ��X����0x80����ʾλ�������ʽΪ1+��ַ��(0x80+X)
    Lcd1602_Write_Cmd(X);   // ���������֣�������ʾλ��
    Lcd1602_Write_Data(DData); // �������ݣ���ʾ�ַ�
}

// LCD1602��ʼ��
void Lcd1602_Init()              
{
    Lcd1602_Write_Cmd(0x38);    // ����ʾ��������ʾģʽΪ5*8����ʹ��8λ��������
    Lcd1602_Write_Cmd(0x0c);    // ������ʾ���رչ�꣬�رչ����˸
    Lcd1602_Write_Cmd(0x06);    // ���ù���ƶ�����Ϊ�ң���������ʾ��λ
    Lcd1602_Write_Cmd(0x01);    // ����
}

// ��ʱ����ʼ��
void Timer_Init(void)
{
    TMOD = 0x01;    // ��T0��ʱ������ʽ1��ʹ��ģʽ1��16λ��ʱ��
    TH0 = 0xfc;     // T0���ó�ֵ����ʱ10ms����8λ��ֵ
    TL0 = 0x18;     // T0���ó�ֵ����ʱ10ms����8λ��ֵ
    ET0 = 1;        // ��T0�ж���������T0�ж�
    TR0 = 0;        // Ĭ�Ϲرն�ʱ��������0���ȴ���Ҫʱ�ٴ�
    EA = 1;         // �����жϣ����������ж�
}

unsigned char Key_Check(void)  // ������⺯��
{
    static flag;                    // ��̬���������ڼ�¼����״̬��ʵ������
    unsigned char KeyValue = 0;    // ����ֵ��ʼ��Ϊ0

    if ((GPIO_KEY & 0x0f) != 0x0f) // ��ȡ��4λ���жϰ����Ƿ���
    {
        if (!flag)
        {
            Delay_Ms(10);            // ��ʱ10ms��������
            if ((GPIO_KEY & 0x0f) != 0x0f) // �ٴμ������Ƿ���
            {
                flag = 1;          // ����flagΪ1����ʾ�����Ѿ�����⵽
                switch (GPIO_KEY & 0x0f)
                {
                    case (0x0e): KeyValue = 2; break; // ���������0x0e������KeyValueΪ2
                    case (0x0d): KeyValue = 1; break; // ���������0x0d������KeyValueΪ1
                    case (0x0b): KeyValue = 3; break; // ���������0x0b������KeyValueΪ3
                    case (0x07): KeyValue = 4; break; // ���������0x07������KeyValueΪ4
                }
            }
        }
    }
    else
    {
        flag = 0; // �������û�б����£���flag����Ϊ0����ʾ���Խ�����һ�ΰ������
    }

    return KeyValue; // ���ذ���ֵ
}
