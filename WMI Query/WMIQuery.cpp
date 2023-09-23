// WMIQuery.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "WMISearcher.h"

int main()
{
	try {
		const CWMISearcher wmi_searcher(L"ROOT\\CIMV2");
		std::wstring result = wmi_searcher.ExecuteQuery(L"SELECT * FROM Win32_Process", L"Name");
		std::wcout << L"Query Result:" << std::endl << result << std::endl;
		result = wmi_searcher.ExecuteQuery(L"SELECT * FROM Win32_Processor", L"ProcessorId");
		std::wcout << L"Query Result:" << std::endl << result << std::endl;
		result = wmi_searcher.ExecuteQuery(L"SELECT * FROM Win32_BaseBoard ", L"SerialNumber");
		std::wcout << L"Query Result:" << std::endl << result << std::endl;
	}

	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
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
