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


void data_frame_reader()
{

 	ifstream infile;
 	
 	int runNo;
 	cout<< "Enter the run No. : ";
 	cin>> runNo;
 	char runfile[100]; 
 	sprintf(runfile, "data/data_%d.dat",runNo);
 	infile.open(runfile);
 	char bytec, bytec1, bytec2, bytec3, bytec4, tempc,tempc1;
 	uint8_t byte, byte1, byte2, byte3, byte4, byte5, byte6, byte7, temp,temp1;
 	int port, data, dataCount;
 	uint32_t time;
 	int b1, b2, b3, b4, b5, b6, b7, b8;
 	int firstTime=0, gettime;
	TH2F *DataADC= new TH2F("ADC_Plot","ADC_Plot",1000,-0.5,999.5,4096,-0.5,4095.5);	
	//TH1F *DataADC= new TH1F("ADC_Plot","ADC_Plot",100000,-0.5,9999.5);	
 	while(!infile.eof( ))
	{
		infile.get(bytec);
		byte = bytec;
		if(byte == 0xf8)
		{
			infile.get(bytec);
			byte1 = bytec;
			infile.get(bytec);
			byte2 = bytec;
			infile.get(bytec);
			byte3 = bytec;
			infile.get(bytec);
			byte4 = bytec;
			infile.get(bytec);
			byte5 = bytec;
			infile.get(bytec);
			byte6 = bytec;
			infile.get(bytec);
			byte7 = bytec;
			b1 = byte; b2 = byte1; b3 = byte2; b4 = byte3;
			b5 = byte4; b6 = byte5; b7 = byte6; b8 = byte7;
			//time = (b2*16777216) + (b3*65536) + (b4*256) + b5;
			time = (b3*65536) + (b4*256) + b5;
			if(firstTime < 10)
			{
				firstTime = firstTime + 1;
				gettime = time;
			}
			//time = time - gettime;
			dataCount = b2;
			port = b6;
			data = (b7*256) + b8;
			cout<<"Command type:"<<b1<<"  DataCount:"<<dataCount<<"  Time:"<<time<<"  Port:"<<port<<"  Data:"<<data<<endl;
			DataADC->Fill(time, data);
		}
	}
	TCanvas *c1 = new TCanvas("c1","",1200,800);	
 	c1->cd();
 	DataADC->Draw();
// 	DataADC->Draw("HIST");
}
