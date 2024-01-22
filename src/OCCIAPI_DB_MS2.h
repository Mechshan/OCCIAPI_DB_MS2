#pragma once
#include <iostream>
#include <occi.h>
using oracle::occi::Environment;
using oracle::occi::Connection;

struct Employee {
	int employeeNumber;
	char lastName[50];
	char firstName[50];
	char email[100];
	char phone[50];
	char extension[10];
	char reportsTo[100];
	char jobTitle[50];
	char city[50];
};

int menu(void);

int findEmployeeMenu(Connection* conn);

int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp);

void displayEmployee(Connection *conn, struct Employee emp);

void displayAllEmployees(Connection *conn);

Employee createEmployee();

int inputEmployeeNum();

void insertEmployee(Connection *conn, Employee emp);

void updateEmployee(Connection *conn, int employeeNumber);

void deleteEmployee(Connection *conn, int employeeNumber);

