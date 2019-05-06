/*Driver	Consente la connessione al database
Connection	Rappresenta la connessione al database
Statement	Rappresenta una semplice query
PreparedStatement	Rappresenta query con parametri settabili successivamente
ResultSet	Rappresenta il valore di ritorno di una query
ResultSetMetaData	Contiene i metadati relativi al resultset*/
//CONNECTORS https://www.mysql.com/products/connector/


#include <string>
#include <iostream>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
//PER CREAZIONE E MODIFICA
#include <my_global.h>
#include <mysql.h>
MYSQL *conn;
//
using namespace std;
const string user = "";
const string passwd = "";
const string database = "test";
const string dbhost = "localhost";



int main()

{
	sql::Driver* driver;
	sql::Connection* connection;
	sql::Statement* stmt;
	sql::ResultSet* res;
	sql::ResultSetMetaData* res_md;
    //
	con = mysql_init(NULL);

	if (con == NULL)
	{
      //fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}

	if (mysql_real_connect(con, dbhost, user, passwd,
		NULL, 0, NULL, 0) == NULL)
	{
      //fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		exit(1);
	}

	if (mysql_query(con, "CREATE DATABASE data"))
	{
      //fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		exit(1);
	}
	stmt->execute("CREATE TABLE `data` (
		`id` int(11) NOT NULL,
		`hash` varchar(8) NOT NULL,
		`rssi` int(11) NOT NULL,
		`mac` varchar(17) NOT NULL,
		`timestamp` timestamp NOT NULL,
		`ssid` varchar(64) NOT NULL,
		`board` varchar(17) NOT NULL
		);");

	if (mysql_query(con, "INSERT INTO base VALUES( '"++"', '")) {
    //fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		exit(1);
	}


    //


	string query =
	"SELECT * FROM * ORDER BY ";

	try

	{

        // stabiliamo la connesione e selez. il db

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

		cout << ">> Presi "

		<< res->rowsCount()

		<< " record <<"

		<< endl << endl;



        // stampiamo i nomi dei campi

		for(int i = 1; i <= res_md->getColumnCount(); i++)

			cout << res_md->getColumnLabel(i) << "\t\t";

		cout << endl << endl;



        // fetch dei dati

		while(res->next())

		{

			for(int i = 1; i <= res_md->getColumnCount(); i++)

				cout << res->getString(i) << "\t\t";

			cout << endl;

		}



        // liberiamo le risorse

		delete res;

		delete stmt;

		connection->close();

		delete connection;

	}

	catch(sql::SQLException& e)

	{
		cout << e.what() << endl;

	}

}
