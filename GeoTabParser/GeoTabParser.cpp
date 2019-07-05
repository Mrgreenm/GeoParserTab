// GeoTabParser.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "windows.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <filesystem>

using namespace std;
     

vector <string> fileList;
ifstream utfFile;
string logic_type = "1";
string myPath;
bool parametr22=0, parametrGdal=0;

void GetParametrs(int paramc, char** paramv) 
{
	
	if (paramc > 0)
	{
		char p1[] = "-gdal";
		char p2[] = "-22";
		vector <string> proxy;

		for (int i = 1; i < paramc; ++i)
		{
			proxy.push_back(paramv[i]);
			//if (strcmp(paramv[i], p1)) { parametrGdal = true; cout << paramv[i] << " "<<p1<<endl; }
     		//if (strcmp(paramv[i], p2)) { parametr22 = true; }
		}

		for (string proxyParametr : proxy) {


			if (proxyParametr == "-22") { parametr22 = true; }
			if (proxyParametr == "-gdal") { parametrGdal = true; }
		}
	

	}
	
}




string getMyPath()
{
	TCHAR buffer[MAX_PATH];
	GetCurrentDirectory(sizeof(buffer), buffer);
	return buffer;
}


string GetSubsting(string beginChar, string endChar, const string enterString)
{
	int i1, i2;
	string localSubstr;


	if ((i1 = enterString.find(beginChar)+1) < 0) {i1 = 0;}
	
	localSubstr = enterString.substr(i1, string::npos);
	if ((i2 = localSubstr.find(endChar) + i1) < 0) { i2 = 0; }
	
	return enterString.substr(i1, i2-i1);
}



void AddGdalStringToFile(vector <string> utfVector)
{
	

	string gdalString, gdalwarpString;
	string Xr, Yr, Xutf, Yutf,tifName;
	string fromPath, toPath;
	
	
	tifName = utfVector[0].substr(1, utfVector[0].find("|") - 1);
	tifName.replace(tifName.find("."), string::npos, ".tif");
	if (parametr22) { tifName.replace(0, tifName.find("-") + 1, ""); }

	fromPath = myPath + "\\" + tifName;
	toPath = myPath + "\\Geotif\\" + tifName;
	cout << toPath << endl;


	CreateDirectory("Temp",NULL);
	CreateDirectory("Geotifs", NULL);

	gdalString = "gdal_translate -of GTiff";

	vector <string> localgdalStringsVector;

	for (string proxyString : utfVector) {
		
		if (proxyString.length()>2) 
		{
			gdalString = gdalString + " -gcp ";
			cout << gdalString << endl;
			Xr = GetSubsting("|", "+", proxyString);
			Yr = GetSubsting("+", ",", proxyString);


			int i;

			if ((i = proxyString.find(",") + 1) < 0) { i = 0; }
			proxyString = proxyString.substr(i, string::npos);

			Xutf = GetSubsting("*", ",", proxyString);



			if ((i = proxyString.find(",") + 1) < 0) { i = 0; }
			proxyString = proxyString.substr(i, string::npos);

			Yutf = GetSubsting("*", ",", proxyString);


			int intXutf = round(atof(Xutf.c_str()));
			gdalString = gdalString + Xr + " " + Yr + " " + to_string(intXutf) + " " + Yutf;
					

			//cout << Xr << "*" << Yr << "*" << Xutf << "*" << Yutf << endl;
		}

	}

	gdalString = gdalString + " " + fromPath + " " + myPath + "\\Temp\\" + tifName;
	//cout << gdalString << endl;
	gdalwarpString = "gdalwarp -r cubicspline -order 1 -co COMPRESS=TZW";
	gdalwarpString = gdalwarpString +" "+ myPath + "\\Temp\\" + tifName +" "+ myPath + "\\Geotifs\\" + tifName;
	    
	ofstream proxyGdalFile("./GdalFile.txt", ios_base::app);
	    proxyGdalFile << gdalString << endl;
		proxyGdalFile << gdalwarpString << endl;
	proxyGdalFile.close();
	
}


bool GetTabFileList()
{
	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile("./*.tab", &f);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			//puts(f.cFileName);
			fileList.push_back(f.cFileName);
			
		} while (FindNextFile(h, &f));

	}
	else
	{
		return false;
	}
	return true;
}


bool GetUtfFile()
{
	utfFile.open("./UTM_Coords.txt");
	return utfFile.is_open();

}


vector <string>  GetCoordsFromTab(string fileName)
{
	vector <string> LocalStrList;
	
	string path = "./" + fileName;
	ifstream proxyTabFile(path);
	if (proxyTabFile.is_open())
	{
		string str, X1Y1, X2Y2, X1, Y1, X2, Y2;
		
		bool isHeader;
		isHeader = true;

		while (getline(proxyTabFile, str))
		{
			
			string::size_type str_index;
			string::size_type str2_index;

			str_index = str.find("(");
			str2_index = str.find(")");
			
			if (str_index != -1)
			{
				
				X1Y1 = str.substr(str_index+1, str2_index- str_index-1);
				str_index = str.find("(");
				str2_index = str.find(")");
				X1 = X1Y1.substr(0,X1Y1.find(","));
				Y1 = X1Y1.substr(X1Y1.find(",")+1,string::npos);
				X1Y1 = Y1 + "," + X1;
							   
				str.erase(0,str2_index+2);
				str_index = str.find("(");
				str2_index = str.find(")");
				X2Y2 = str.substr(str_index + 1, str2_index - str_index - 1);
				X2 = X2Y2.substr(0, X2Y2.find(","));
				Y2 = X2Y2.substr(X2Y2.find(",") + 1, string::npos);
				X2Y2 = X2 + "+"+ Y2;
			
				if (isHeader)
				{
					LocalStrList.push_back("#"+fileName + "|" + X2Y2 + "," + X1Y1);
					isHeader = false;

				}
				else
				{ 
					LocalStrList.push_back(fileName + "|" + X2Y2 + "," + X1Y1);
					cout << LocalStrList[LocalStrList.size() - 1] << endl;

				}
				
			}
		}

		proxyTabFile.close();
		return LocalStrList;


	}
	else
	{
		return LocalStrList;
	}


}

//*************************************** M A I N() ****************************************************************
int main(int argc, char** argv)
{

	GetParametrs(argc,argv);

	if (parametrGdal)
	{
		logic_type = "-gdal";
	}
	else
	{
		logic_type = "-tab";
	}



	cout << parametr22 << "   " << parametrGdal << endl;


/*
	if (argc > 1)
	{
		cout<<argv[1] ;
		logic_type = argv[1];
	}
	else
	{
		logic_type ="-tab";
	}
	*/


	//From TAB To UTF*******************************************

	if (logic_type == "-tab")
	{

		
		
		if (GetTabFileList())
		{

			ofstream tabCoordsFile("./TabCoordsFile.txt");
			tabCoordsFile << "Name,X,Y" << endl;
			for (string proxyFileName : fileList) {


				vector <string> coordString = GetCoordsFromTab(proxyFileName);
				if (!coordString.empty())
				{
					for (string proxy_String : coordString) {


						tabCoordsFile << proxy_String << endl;

					}

				}

				else
				{
					fprintf(stderr, "Error get tab file list");
				}
			}

			tabCoordsFile.close();
			ofstream MyFile("./UTM_Coords.txt", ios_base::trunc);
			MyFile.close();
		

		}
		else
		{
			fprintf(stderr, "Error get tab file list");
			return 0;
		}

	}



	//From UTM To GDAL*******************************************

	if (logic_type == "-gdal")
	{

		ofstream GdalFile("./GdalFile.txt", ios_base::trunc);
		GdalFile.close();
		
		
		vector <string> currentFileCoords;
		
		if (GetUtfFile())
			
		{
			myPath = getMyPath();
			string str;
			bool startAdd = false;
			while (getline(utfFile, str))
			{
				
				string::size_type str_index;
				
				str_index = str.find("#");
				if (str_index != -1)
				{
					
					if (startAdd) {

						AddGdalStringToFile(currentFileCoords);
						currentFileCoords.clear();

					}
					
					startAdd = true;
					
				}
				
				if (startAdd) 
				{
					currentFileCoords.push_back(str);
				}


			}
			AddGdalStringToFile(currentFileCoords);

			

		}
		else
		{
				
			fprintf(stderr, "Error get UTM_Coords.txt");
			return 0;
		}

	}






	return 0;
}



	
	
	
	
	






// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
