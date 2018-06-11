// SQLTry02.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
//#include <mysql_connection.h> per usarla serve libreria boost c++
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
const sql::SQLString user = "root";
const sql::SQLString passwd = "pdsproject";
const sql::SQLString database = "pdsproject";
const sql::SQLString dbhost = "localhost:3307";

int main()
{
	sql::Driver* driver;
	sql::Connection* connection;
	sql::Statement* stmt;
	sql::ResultSet* res;
	sql::ResultSetMetaData* res_md;
	sql::SQLString query;
	// Prova a creare la tabella per i dati chiamata "data"
	try {
		query = "CREATE TABLE `data` (`id` int(11) NOT NULL,`hash` varchar(8) NOT NULL,`rssi` int(11) NOT NULL,`mac` varchar(17) NOT NULL,`timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,`ssid` varchar(64) NOT NULL,`board` varchar(17) NOT NULL);";
		// stabiliamo la connesione e selez. il db
		driver = get_driver_instance();
		connection = driver->connect(dbhost, user, passwd);
		connection->setSchema(database);

		// creiamo uno statement
		stmt = connection->createStatement();
		// eseguiamo la query
		res = stmt->executeQuery(query);
	}
	catch (sql::SQLException& e)
	{
		std::cout << e.what() << std::endl;
	}

	try {
		query = "SELECT * FROM data";
		driver = get_driver_instance();
		connection = driver->connect(dbhost, user, passwd);
		connection->setSchema(database);
		// creiamo uno statement
		stmt = connection->createStatement();
		// eseguiamo la query
		res = stmt->executeQuery(query);
		// otteniamo i metadati
		res_md = res->getMetaData();
		// contiamo le righe ottenute
		std::cout << ">> Presi " << res->rowsCount() << " record <<" << std::endl << std::endl;

		// stampiamo i nomi dei campi
		/*for (int i = 1; i <= res_md->getColumnCount(); i++)
		std::cout << res_md->getColumnName(i).c_str() << "\t\t\t";
		std::cout << std::endl << std::endl;*/


		// fetch dei dati
		sql::SQLString vv;
		while (res->next()) {
			std::cout << res->getInt("id") << " ";
			//.c_str() necessario per evitare eccezioni incomprensibili
			std::cout << res->getString("hash").c_str() << " ";
			std::cout << res->getString("rssi").c_str() << " ";
			std::cout << res->getString("mac").c_str() << " ";
			std::cout << res->getString("timestamp").c_str() << " ";
			std::cout << res->getString("ssid").c_str() << " ";
			std::cout << res->getString("board").c_str() << " " << std::endl;
		}

		// liberiamo le risorse
		delete res;
		delete stmt;
		connection->close();
		delete connection;
	}
	catch (sql::SQLException& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	getchar();
	return 0;
}

