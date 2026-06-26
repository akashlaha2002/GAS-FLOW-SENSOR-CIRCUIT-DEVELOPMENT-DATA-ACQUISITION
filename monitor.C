#include <iostream>
#include <stdio.h>
#include<math.h>
using namespace std;
#include <stdint.h>
#include <TArrayD.h>
#include <TGraph.h>
#include <TH1D.h>
#include <vector>
#include <sstream>  // for string streams
#include <string>  // for string

void monitor()
{
	ifstream infile;
	char bytec, bytec1, bytec2, bytec3, bytec4, tempc,tempc1;
	int runNo;
	cout<<"Enter the run No. : ";
	cin>>runNo;
	char runfile[100];
	sprintf(runfile, "data/data%d.txt",runNo);
	infile.open(runfile);
	char byteArray[500];
	uint8_t byte, byte1, byte2, byte3, byte4, temp,temp1;
	uint8_t dat[4]={0};
	int ch=1, ch1=1, b1,b2,b3,b4;
	int chNo, ADC, MCM, trigNo, dataCount, tempInt1, tempInt2, parity;
	int tempADC[200],tempChNo[200],dataVol=0,dataError=0,TrigChange=0;
	int bit1, bit2, bit3, bit4, bit5, bit6, bit7, bit8;
	int count = 0;
	int Data1[64]={0}, Data2[64]={0};
	TH1F *pedData= new TH1F("Pedestal","Pedestal",128,-0.5,127.5);	
	TH1F *pedestal= new TH1F("PiedestalVal","Pedestal Value",128,-0.5,127.5);	
	infile.get(bytec);
	byte = bytec;
	while(!infile.eof( ) && byte == 0xff)
	{
		byte1 = byte;
		infile.get(bytec);
		byte = bytec;
	}
	while(!infile.eof( ))
	{
		while(!infile.eof( ) && byte == 0xff)
		{
			byte1 = byte;
			infile.get(bytec);
			byte = bytec;
		}
		if(byte1 == 0xff && byte == 0xbb) ///trigger number
		{
			count = 0;
			infile.get(bytec1);
			infile.get(bytec2);
			infile.get(bytec3);
			infile.get(bytec4);
			byte1 = bytec1;
			byte2 = bytec2;
			byte3 = bytec3;
			byte4 = bytec4;
			b1 = byte1; b2 = byte2; b3 = byte3; b4 = byte4;
			trigNo = (b1*16777216) + (b2*65536) + (b3*256) + b4;
			cout<<"Trigger No :"<<trigNo<<"  ";
			infile.get(bytec);
			byte = bytec;
			infile.get(bytec);
			byte1 = bytec;
			if(byte == 0xbb && byte1 == 0xff)
			{
				if(dataError == 0)
				{
					for(int i =0; i<dataVol; i++)
					{
						Data1[tempChNo[i]] = Data1[tempChNo[i]] + tempADC[i];
						Data2[tempChNo[i]]++;
						pedData->Fill(tempChNo[i],tempADC[i]);
					}
					dataVol = 0;
				}
				else
				{
					cout<<" Previous trigger in error "<<endl;
					dataError = 0;
				}
				cout<<"Trigger check ok "<<endl;
			}
			else
			{
				cout<<"Trigger check error, searching for next trigger"<<endl;
				while(!infile.eof())
				{
					byte1 = byte;
					infile.get(bytec);
					byte = bytec;
					//cout<<byte<<" ";
					if(byte1 == 0xff && byte == 0xbb) break; 
				}
				infile.get(bytec1);
				infile.get(bytec2);
				infile.get(bytec3);
				infile.get(bytec4);
				byte1 = bytec1;
				byte2 = bytec2;
				byte3 = bytec3;
				byte4 = bytec4;
				trigNo = (b1*16777216) + (b2*65536) + (b3*256) + b4;
				cout<<"Trigger No :"<<trigNo<<"  ";
				infile.get(bytec);
				byte = bytec;
				infile.get(bytec);
				byte1 = bytec;
				if(byte == 0xbb && byte1 == 0xff)
				{
					cout<<"Trigger check ok "<<endl;
				}
			}
			byte = byte1;
			while(!infile.eof( ) && byte == 0xff)
			{
				byte1 = bytec;
				infile.get(bytec);
				byte = bytec;
			}

		}
		if(byte1 == 0xff && byte != 0xff)   ///data starts here
		{
			//cout<<((byte & 0xF0)>>4)<<" "<<(byte & 0x0F)<<endl;
			//cout<<((byte1 & 0xF0)>>4)<<" "<<(byte1 & 0x0F)<<endl;
			dat[count] = byte;
			count ++;
			//cout<<((dat[count] & 0xF0)>>4)<<" "<<(dat[count] & 0x0F)<<" " << count<<endl;
			if(count == 4)
			{
				count = 0;
			}
			while(!infile.eof( ))
			{
				int ffCount = 0;
				while(!infile.eof( ) && count < 4)
				{
					infile.get(bytec);
					byte = bytec;
					dat[count] = byte;
					//cout<<((dat[count] & 0xF0)>>4)<<" "<<(dat[count] & 0x0F)<<" " << count<<endl;
					if(dat[count] == 0xff)
					{
						ffCount++;
					}
					count ++;

				}
				if(count == 4)
				{
					count = 0;
				}
				if(ffCount > 1)
				{
					if(dat[0] == 0xff)
					{
						count = 0 ;
					}
					else if(dat[1] == 0xff)
					{
						count = 1;
					}
					else if(dat[2] == 0xff)
					{
						count = 2;
					}
					else if(dat[3] == 0xff)
					{
						count = 3;
					}
				}

				if(ffCount > 1) break;

				temp = dat[2] & 0x0F;
				ADC = ((uint16_t)temp << 8) | dat[3];
				temp = (dat[2] & 0xF0)>>4;
				temp1 = (dat[1] & 0x03)<<4;
				chNo = temp1 | temp;
				MCM = dat[1]>>2;
				//cout<<((dat[0] & 0xF0)>>4)<<" "<<(dat[0] & 0x0F)<<"  "<<endl;
			//	cout<<MCM<<" "<<chNo<<" "<<ADC<<"  LineNo "<<ch1<<endl;

				if(MCM > 0) dataError =1; 
				if(dataError == 1) 
				{
					dataVol = 0;
				}
				else
				{
					tempChNo[dataVol] = chNo;
					tempADC[dataVol] = ADC;
					dataVol++;
				}

			//	pedData->Fill(chNo,ADC);
				ch1++;
			}
		}
	}
	for(int i=0; i<64; i++)
	{
		if(Data2[i]>0)
		{
			pedestal->Fill(i,int(Data1[i]/Data2[i]));
		}
		else
		{
			pedestal->Fill(i,0);
		}
	}
	TCanvas *c1 = new TCanvas("c1","",1200,800);
	c1->Divide(1,2);
	c1->cd(1);
	pedData->Draw("HIST");
	c1->cd(2);
	pedestal->Draw("HIST");
}
