#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <occi.h>
#include "OCCIAPI_DB_MS2.h"


using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

int main(void)
{
	/* OCCI Variables */
	Environment* env = nullptr;
	Connection* conn = nullptr;
	/* Used Variables */
	string str;
	string user = "***";
	string pass = "***";
	string srv = "***";
	int selection = 0;


	try {
		//OPENING CONN
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, srv);
		std::cout << "Connected." << endl;

		do { // until select 0
			std::cout << endl << "********************* HR Menu *********************" << endl;

			selection = menu();

			switch (selection) {
			case 1:
				std::cout << ">>>> Finding Employee" << endl << endl;
				findEmployeeMenu(conn);
				break;
			case 2:
				std::cout << ">>>> Employee Report" << endl;
				displayAllEmployees(conn);
				break;
			case 3:
				std::cout << ">>>> Adding Employee" << endl;
				insertEmployee(conn, createEmployee());
				break;
			case 4:
				std::cout << ">>>> Updating Employee" << endl;
				updateEmployee(conn, inputEmployeeNum());
				break;
			case 5:
				std::cout << ">>>> Removing Employee" << endl;
				deleteEmployee(conn, inputEmployeeNum());
				break;
			case 0:
				std::cout << ">>>> Exiting" << endl;
				break;
			default:
				std::cout << "Invalid choice" << endl;
				break;
			}
		} while (selection);

		std::cout << "Terminating Connection." << endl;
		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);

	}
	catch (SQLException & sqlExcp) { // catch and display any sql errors
		std::cout << "SQL Exception Error: " << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
	return 0;
}

int menu(void) {

	string input;
	int selection = 0;
	int valid = 0;

	std::cout << "1)	Find Employee" << endl
		<< "2)	Employees Report" << endl
		<< "3)	Add Employee" << endl
		<< "4)	Update Employee" << endl
		<< "5)	Remove Employee" << endl
		<< "0)	Exit" << endl << ">";


	do { // until input = 0

		cin >> input;

		try { // validate user input

			selection = stoi(input); //get int from user

			if (selection >= 0 && selection <= 5) { // valid range 0-5
				valid = 1;
			}
			else { //reprompt
				std::cout << "[0<=value<=5], retry: > ";
			}

		}
		catch (const std::invalid_argument & ia) { //input not an int
			std::cout << "Invalid Integer, try again: ";
		}

	} while (!valid);

	return selection;
}

int findEmployeeMenu(Connection* conn)
{
	std::string input;
	int en = 0;
	int found = 0;
	struct Employee e;

	std::cout << "Search for employee number: ";
	std::cin >> input;

	try { //validate user input
		en = std::stoi(input); //get int from user
	}
	catch (const std::invalid_argument & ia) { //input not an int
		std::cout << "Invalid Integer." << std::endl;
		return 0; // exit early if input is invalid
	}

	found = findEmployee(conn, en, &e);

	if (found == 0) {
		std::cout << "Employee " << en << " does not exist." << std::endl;
	}
	else {
		std::cout << endl;
		displayEmployee(conn, e);
	}

	return found;
}

int findEmployee(Connection* conn, int employeeNumber, Employee* emp)
{
	int found = 0;

	try {
		//STATEMENT BEGINS
		Statement* stmt = conn->createStatement();

		ResultSet* rs = stmt->executeQuery(
			"SELECT e.employeenumber, e.lastname, e.firstname, e.email, o.phone, e.extension, e.reportsto, e.jobtitle, o.city "
			"FROM employees e "
			"JOIN offices o USING(officecode) "
			"WHERE employeenumber = " + to_string(employeeNumber)); //


		while (rs->next()) { // parse the results from sql
			found = 1;

			//read the results
			string employeenum = rs->getString(1);
			string lastname = rs->getString(2);
			string firstname = rs->getString(3);
			string email = rs->getString(4);
			string phone = rs->getString(5);
			string extension = rs->getString(6);
			string reportsto = rs->getString(7);
			string jobtitle = rs->getString(8);
			string city = rs->getString(9);

			//apply results to emp
			if (emp) {
				// emp is not null, so it's safe to dereference it
				emp->employeeNumber = stoi(employeenum);
				strcpy(emp->lastName, rs->getString(2).c_str());
				strcpy(emp->firstName, rs->getString(3).c_str());
				strcpy(emp->email, rs->getString(4).c_str());
				strcpy(emp->phone, rs->getString(5).c_str());
				strcpy(emp->extension, rs->getString(6).c_str());
				strcpy(emp->reportsTo, rs->getString(7).c_str());
				strcpy(emp->jobTitle, rs->getString(8).c_str());
				strcpy(emp->city, rs->getString(9).c_str());
			}
		}

		conn->terminateStatement(stmt);
		//STATEMENT ENDS
	}
	catch (SQLException & sqlExcp) {// catch any sql errors
		std::cout << "SQL Exception Error: " << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}

	return found;
}

void displayEmployee(Connection* conn, Employee emp)
{
	std::cout << "employeeNumber = " << emp.employeeNumber << std::endl;
	std::cout << "lastName = " << emp.lastName << std::endl;
	std::cout << "firstName = " << emp.firstName << std::endl;
	std::cout << "email = " << emp.email << std::endl;
	std::cout << "phone = " << emp.phone << std::endl;
	std::cout << "extension = " << emp.extension << std::endl;
	std::cout << "reportsTo = " << emp.reportsTo << std::endl;
	std::cout << "jobTitle = " << emp.jobTitle << std::endl;
	std::cout << "city = " << emp.city << std::endl;
}

void displayAllEmployees(Connection* conn)
{
	try {
		// STATEMENT BEGINS

		Statement* stmt = conn->createStatement();

		ResultSet* rs = stmt->executeQuery(
			"SELECT e.employeenumber, e.firstname || ' ' || e.lastname, e.email, o.phone, e.extension, m.firstname || ' ' || m.lastname "
			"FROM employees e "
			"JOIN offices o USING(officecode) "
			"LEFT JOIN employees m ON e.reportsto = m.employeenumber "
			"ORDER BY e.employeenumber");


		cout << "-------------------------------------------------------------------------------------------------------------------------" << endl;
		cout << setw(10) << left << "Employee" << setw(30) << left << "Name" << setw(35) << left << "Email" << setw(20) << left << "Phone" << setw(10) << left << "Ext" << setw(25) << left << "Manager" << endl;
		cout << "-------------------------------------------------------------------------------------------------------------------------" << endl;

		bool anyRows = false;

		while (rs->next()) { // parse the results from sql
			string employeenum = rs->getString(1);
			string name = rs->getString(2);
			string email = rs->getString(3);
			string phone = rs->getString(4);
			string extension = rs->getString(5);
			string manager = rs->getString(6);
			cout << setw(10) << left << employeenum << setw(30) << left << name << setw(35) << left << email << setw(20) << left << phone << setw(10) << left << extension << setw(25) << left << manager << endl;
			anyRows = true;
		}

		if (!anyRows) {
			cout << "There is no employee information to be displayed." << endl;
		}

		conn->terminateStatement(stmt);
		// STATEMENT ENDS
	}
	catch (SQLException & sqlExcp) {// catch any sql errors
		std::cout << "SQL Exception Error: " << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
}

Employee createEmployee()
{


	std::string input;

	struct Employee e;


	e.employeeNumber = inputEmployeeNum();

	std::cout << "Last Name: ";
	std::cin >> input;
	strcpy(e.lastName, input.c_str());

	std::cout << "First Name: ";
	std::cin >> input;
	strcpy(e.firstName, input.c_str());

	std::cout << "Email: ";
	std::cin >> input;
	strcpy(e.email, input.c_str());

	std::cout << "Extension: ";
	std::cin >> input;
	strcpy(e.extension, input.c_str());

	std::cout << "Job Title: ";
	std::cin.ignore();
	std::cin.clear();
	std::getline(std::cin, input);
	strcpy(e.jobTitle, input.c_str());

	std::cout << "City: ";
	//std::cin.ignore();
	std::cin.clear();
	std::getline(std::cin, input);
	strcpy(e.city, input.c_str());

	return e;
}

int inputEmployeeNum()
{
	string input;
	int valid = 0;
	int en = 0;
	cout << "Employee Number: ";
	do {
		try { //validate user input
			cin >> input;
			en = stoi(input); //get int from user
			valid = 1;
		}
		catch (const invalid_argument & ia) { //input not an int
			cout << "Invalid Integer." << endl << "Retry: ";

		}
	} while (!valid);
	return en;
}


void insertEmployee(Connection* conn, Employee emp)
{
	if (findEmployee(conn, emp.employeeNumber, nullptr)) {
		cout << endl << "That employee number is already in use." << endl;
	}
	else
	{
		cout << endl << "Inserting this employee :" << endl << endl;
		cout << "Employee Number : " << emp.employeeNumber << endl <<
			"Last Name : " << emp.lastName << endl <<
			"First Name : " << emp.firstName << endl <<
			"Email : " << emp.email << endl <<
			"Extension : " << emp.extension << endl <<
			"Job Title : " << emp.jobTitle << endl <<
			"City : " << emp.city << endl;

		try {
			//STATEMENT BEGINS
			Statement* stmt = conn->createStatement();

			stmt->executeQuery(
			"INSERT INTO employees (employeenumber, lastName, firstName, extension, email, officeCode, reportsTo, jobTitle)"
				"VALUES("+to_string(emp.employeeNumber)+", '"+emp.lastName+"', '"+emp.firstName+"', '"+emp.extension+"', '"+emp.email+"', '1', 1002, '"+emp.jobTitle+"')");

			cout << "Employee Added." << endl;

			conn->terminateStatement(stmt);
			//STATEMENT ENDS
		}
		catch (SQLException & sqlExcp) {// catch any sql errors
			std::cout << "SQL Exception Error: " << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
		}

	}
}

void updateEmployee(Connection* conn, int employeeNumber)
{
	string input;

	if (findEmployee(conn, employeeNumber, nullptr)) { // found the specified employee

		cout << "Set the new extension: ";
		cin >> input;

		try {
			//STATEMENT BEGINS
			Statement* stmt = conn->createStatement();

			stmt->executeQuery("UPDATE employees SET extension='" + input +
				"' WHERE employeenumber=" + to_string(employeeNumber));

			cout << "Extension Updated." << endl;

			conn->terminateStatement(stmt);
			//STATEMENT ENDS
		}
		catch (SQLException & sqlExcp) {// catch any sql errors
			std::cout << "SQL Exception Error: " << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
		}

	}
	else //did not find
	{
		cout << endl << "That employee does not exist." << endl;
	}
}

void deleteEmployee(Connection* conn, int employeeNumber)
{
	Employee result;
	char input;

	if (findEmployee(conn, employeeNumber, &result)) { // found the specified employee
		cout << endl << "Delete this employee?" << endl<<endl;
		displayEmployee(conn, result); // show what's getting deleted
		cout << endl << "(y/n): ";
		cin >> input;
		if (input == 'y') { //user confirmed

			try {
				//STATEMENT BEGINS
				Statement* stmt = conn->createStatement();

				stmt->executeQuery("DELETE FROM employees WHERE employeenumber="+to_string(employeeNumber));

				conn->terminateStatement(stmt);
				//STATEMENT ENDS
			}
			catch (SQLException & sqlExcp) {// catch any sql errors
				std::cout << "SQL Exception Error: " << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
			}

			cout << "Deleted." << endl;
		}
	}
	else
	{
		cout << endl << "That employee does not exist." << endl;
	}
}
