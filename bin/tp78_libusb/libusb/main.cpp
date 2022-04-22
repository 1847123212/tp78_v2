#include "lusb0_usb.h"
#include "../Hal/keyboard.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

#define _idVender	0x434A
#define _idProduct	0x5540

#define CONFIG_FILE_PATH	"./config.txt"
#define BUF_LEN		512

using namespace std;

usb_dev_handle * my_device;
struct usb_bus * bus;
struct usb_device * device;

int EP_IN = 0;
int EP_OUT = 0;

char writeByte[BUF_LEN] = { 0 };
char receiveByte[BUF_LEN] = { 0 };

// declare
bool find_my_device(UINT32 VID, UINT32 PID);

int main()
{
	string op;
	int count = 0;
	if (keyboard_read_configuration(CONFIG_FILE_PATH) < 0) {
		cout << "��ȡ�����ļ�ʧ�ܣ�Ԥ��·��Ϊ��" << CONFIG_FILE_PATH << endl;
		system("pause");
		return 0;
	}
	usb_set_debug(1);

	if (find_my_device(_idVender, _idProduct))
	{
		cout << "TP78 ������" << endl << "S - �鿴���̲���" << endl << "R - ��ȡ�豸������" << endl << "W - д������" << endl
			 << "C <row> <col> <key> - �ı��1����" << endl << "X <row> <col> <key> - �ı��2����" << endl
			 << "L <LED Style> - �ı�LEDĬ����ʽ(0~4)" << endl << "D <BLE Device> - �ı�����Ĭ�������豸ID(1~6)" << endl 
			 << ">�������";
		while (true)
		{
			getline(cin, op);
			if (op[0] == 'S')
			{
				cout << "Layer 1:" << endl;
				for (unsigned char i = 0; i < Col; i++) {
					for (unsigned char j = 0; j < Row; j++) {
						if (Custom_Layer.size() > i && Custom_Layer[i].size() > j) {
							cout << Custom_Layer[i][j];
						}
						else {
							cout << "KEY_None";
						}
						if (j == Row - 1) {
							cout << "\n";
						}
						else {
							cout << "\t";
						}
					}
				}
				cout << "Layer 2:" << endl;
				for (unsigned char i = 0; i < Col; i++) {
					for (unsigned char j = 0; j < Row; j++) {
						if (EX_Layer.size() > i && EX_Layer[i].size() > j) {
							cout << EX_Layer[i][j];
						}
						else {
							cout << "KEY_None";
						}
						if (j == Row - 1) {
							cout << "\n";
						}
						else {
							cout << "\t";
						}
					}
				}
				cout << ">�������";
			}
			else if (op[0] == 'R')
			{
				writeByte[0] = 3;
				writeByte[1] = 'R';

				bool is_ok = true;
				for (unsigned char i = 0; i < Col; i++) {
					writeByte[2] = i;
					count = usb_bulk_write(my_device, EP_OUT, (char*)writeByte, writeByte[0], 10000);
					count = usb_bulk_read(my_device, EP_IN, receiveByte, BUF_LEN, 10000);
					if (receiveByte[1] == 'R' && count == 3 + 2 * Row) {
						for (unsigned char j = 0; j < Row; j++) {
							Custom_Layer[i][j] = KEY_r_hash.at(receiveByte[3 + j]);
							EX_Layer[i][j] = KEY_r_hash.at(receiveByte[3 + Row + j]);
						}
					}
					else {
						is_ok = false;
					}
				}
				if (is_ok == false) cout << "����ʧ�ܣ�" << endl;
				else cout << "���ճɹ���" << endl;
				cout << ">�������";
			}
			else if (op[0] == 'W')
			{
				keyboard_write_configuration(CONFIG_FILE_PATH);
				cout << "д�����óɹ���" << endl << ">�������";
			}
			else if (op[0] == 'C')
			{
				volatile unsigned char r_val, c_val, k_val;

				stringstream ss;
				string str;
				unsigned char cnt = 0;
				ss << op;
				while (getline(ss, str, ' ')) {
					if (++cnt == 2) r_val = stoi(str);
					else if (cnt == 3) c_val = stoi(str);
					else if (cnt == 4) {
						k_val = KEY_hash.at(str);
						break;
					}
				}

				Custom_Layer[c_val][r_val] = str;

				// д������
				writeByte[0] = 3 + Row;
				writeByte[1] = 'C';
				writeByte[2] = c_val;
				for (int i = 3; i < 3 + Row; i++)
				{
					writeByte[i] = KEY_hash.at(Custom_Layer[c_val][i-3]);
				}

				count = usb_bulk_write(my_device, EP_OUT, (char*)writeByte, writeByte[0], 10000);
				std::cout << "д��������:" << count << endl;

				// ���
				count = usb_bulk_read(my_device, EP_IN, receiveByte, BUF_LEN, 2000);
				if (receiveByte[1] == 'S') std::cout << "���ĳɹ���" << endl;
				else std::cout << "����ʧ�ܣ�" << endl;

				cout << ">�������";
			}
			else if (op[0] == 'X')
			{
				unsigned char r_val, c_val, k_val;

				stringstream ss;
				string str;
				unsigned char cnt = 0;
				ss << op;
				while (getline(ss, str, ' ')) {
					if (++cnt == 2) r_val = stoi(str);
					else if (cnt == 3) c_val = stoi(str);
					else if (cnt == 4) {
						k_val = KEY_hash.at(str);
						break;
					}
				}

				EX_Layer[c_val][r_val] = str;

				// д������
				writeByte[0] = 3 + Row;
				writeByte[1] = 'X';
				writeByte[2] = c_val;
				for (int i = 3; i < 3 + Row; i++)
				{
					writeByte[i] = KEY_hash.at(EX_Layer[c_val][i - 3]);
				}

				count = usb_bulk_write(my_device, EP_OUT, (char*)writeByte, writeByte[0], 10000);
				std::cout << "д��������:" << count << endl;

				// ���
				count = usb_bulk_read(my_device, EP_IN, receiveByte, BUF_LEN, 2000);
				if (receiveByte[1] == 'S') std::cout << "���ĳɹ���" << endl;
				else std::cout << "����ʧ�ܣ�" << endl;

				cout << ">�������";
			}
			else if (op[0] == 'L')
			{
				unsigned char led_style;

				stringstream ss;
				string str;
				unsigned char cnt = 0;
				ss << op;
				while (getline(ss, str, ' ')) {
					if (++cnt == 2) {
						led_style = stoi(str);
					}
				}

				// д������
				writeByte[0] = 4;
				writeByte[1] = 'L';
				writeByte[3] = led_style;
				count = usb_bulk_write(my_device, EP_OUT, (char*)writeByte, writeByte[0], 10000);
				std::cout << "д��������:" << count << endl;

				// ���
				count = usb_bulk_read(my_device, EP_IN, receiveByte, BUF_LEN, 2000);
				if (receiveByte[1] == 'S') std::cout << "���ĳɹ���" << endl;
				else std::cout << "����ʧ�ܣ�" << endl;

				cout << ">�������";
			}
			else if (op[0] == 'D')
			{
				unsigned char ble_device;

				stringstream ss;
				string str;
				unsigned char cnt = 0;
				ss << op;
				while (getline(ss, str, ' ')) {
					if (++cnt == 2) {
						ble_device = stoi(str);
					}
				}

				// д������
				writeByte[0] = 4;
				writeByte[1] = 'D';
				writeByte[3] = ble_device;
				count = usb_bulk_write(my_device, EP_OUT, (char*)writeByte, writeByte[0], 10000);
				std::cout << "д��������:" << count << endl;

				// ���
				count = usb_bulk_read(my_device, EP_IN, receiveByte, BUF_LEN, 2000);
				if (receiveByte[1] == 'S') std::cout << "���ĳɹ���" << endl;
				else std::cout << "����ʧ�ܣ�" << endl;

				cout << ">�������";
			}
			else {
				cout << "δʶ������" << endl << ">�������";
			}
			/*
			else if (op[0] == 'w')
			{
				char writeByte[10];
				for (int i = 0; i < sizeof(writeByte); i++)
				{
					writeByte[i] = 50;
				}

				count = usb_bulk_write(my_device, EP_OUT, (char *)writeByte, 10, 10000);
				std::cout << "д��������:" << count << endl << endl;
			}
			else if (op[0] == 'r')
			{
				count = usb_bulk_read(my_device, EP_IN, receiveByte, 10, 100);
				std::cout << "�յ�����������" << count << endl << "�յ����ݣ�" << receiveByte << endl << endl;
				memset(receiveByte, 0, count);
			}
			else if (op[0] == 't')
			{

				char writeByte[10];
				for (int i = 0; i < sizeof(writeByte); i++)
				{
					writeByte[i] = -66;
				}

				SYSTEMTIME time1, time2;
				GetLocalTime(&time1);

				for (int i = 0; i < 1; i++)
				{

					count = usb_bulk_write(my_device, EP_OUT, (char *)writeByte, 10, 0);
					if (count != 10)
					{
						std::cout << "д�����" << endl;
						break;
					}

					count = usb_bulk_read(my_device, EP_IN, receiveByte, 10, 500);
					if (count != 10)
					{
						std::cout << "error:" << count << endl;
					}
					receiveByte[11] = '\0';
					std::cout << "�յ�����������" << count << endl << "�յ����ݣ�" << receiveByte << endl << endl;
				}
				GetLocalTime(&time2);

				//std::cout << "��ȡ 1MB ������ʱ:" << time2.wMilliseconds - time1.wMilliseconds << "ms" << endl;
				//std::cout << "�ٶ�:" << 1000.0f / (time2.wMilliseconds - time1.wMilliseconds) << "MB/s" << endl << endl;
			}
			else if (op[0] == 'e')
			{
				break;
			}
			*/
		}
	}
	else
	{
		std::cout << "û�ҵ��豸������USB���Ӻ��������У�" << endl;

		system("pause");
		return 0;
	}

	//ע���ӿڣ��ͷ���Դ����usb_claim_interface����ʹ�á�
	usb_release_interface(my_device, 0);
	usb_close(my_device);
	std::cout << "USB�豸�ѹرգ�" << endl;

	system("pause");
	return 0;

}


bool find_my_device(UINT32 VID, UINT32 PID)
{
	bool isFind = false;
	// ������ʼ��
	usb_init();
	// Ѱ��usb����
	usb_find_busses();
	// ���������ϵ��豸
	usb_find_devices();

	// ��������
	for (bus = usb_busses; bus; bus->next)
	{
		// ���������ϵ��豸�ҵ���Ҫ���豸
		for (device = bus->devices; device; device->next)
		{
			//Ѱ��ָ����PID,VID�豸
			if (device->descriptor.idProduct == PID && device->descriptor.idVendor == VID)
			{
				std::cout << "�ҵ�����豸!" << endl;
				my_device = usb_open(device);

				//USB���������

				char str[64];

				usb_get_string_simple(my_device, device->descriptor.iManufacturer, str, sizeof(str));
				std::cout << "�豸������ : " << str << endl;
				usb_get_string_simple(my_device, device->descriptor.iProduct, str, sizeof(str));
				std::cout << "��Ʒ���� : " << str << endl;

				int ret = usb_claim_interface(my_device, 2);

				// ��ȡ�˵�ĵ�ַ
				int ep = device->config->interface->altsetting->endpoint->bEndpointAddress;

				if (ep > 0x0f)
				{
					EP_IN = ep;
					EP_OUT = ep - 0x80;
				}
				else
				{
					EP_OUT = ep;
					EP_IN = ep + 0x80;
				}
				std::cout << "EP_IN:" << nouppercase << EP_IN << ", EP_OUT:" << nouppercase << EP_OUT << endl << endl;


				return true;
			}
		}
	}


	return isFind;
}
