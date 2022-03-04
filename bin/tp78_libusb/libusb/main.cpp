#include "lusb0_usb.h"
#include <time.h>
#include <iostream>
#include <stdio.h>


using namespace std;

usb_dev_handle * my_device;
struct usb_bus * bus;
struct usb_device * device;

int EP_IN = 0;
int EP_OUT = 0;


bool find_my_device(UINT32 PID, UINT32 VID);


int main()
{

	char receiveByte[512];
	memset(receiveByte, 0, sizeof(receiveByte));
	char op[10];
	int count = 0;

	usb_set_debug(1);

	if (find_my_device(0x434A, 0x5540))
	{
		while (true)
		{

			std::cout << "�������";

			cin >> op;
			if (op[0] == 'w')
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
		}
	}
	else
	{
		std::cout << "û�ҵ��豸" << endl;
		return 0;
	}

	//ע���ӿڣ��ͷ���Դ����usb_claim_interface����ʹ�á�
	usb_release_interface(my_device, 0);
	usb_close(my_device);
	std::cout << "�豸�ѹرգ�" << endl;

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
