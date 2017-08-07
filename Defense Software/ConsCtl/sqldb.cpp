//---------------------------------------------------------------------------
//
// sqldb.cpp  
//
// DESCRIPTION: Database connection and querying.
//
// AUTHOR:		Justin Jones
//                                                                         
//---------------------------------------------------------------------------

#include <stdio.h>
#include <iostream>
#include <sql3.h>
#include <string.h>
#include <sstream>

#if !defined(_SQLDB_H_)
#define _SQLDB_H_

//global
sqlite3 *db;

//---------------------------------------------------------------------------
// openDb
//
// Used to open the embedded database.
//---------------------------------------------------------------------------
int openDb() {
	char *zErrMsg = 0;
	int rc;
	try {
		rc = sqlite3_open("vdb.db", &db);
		if (rc) {
			printf("Unable to open database!\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return -1;
		}
		return 0;
	}
	catch (CppSQLiteException& e) {
		std::cerr << e.errorCode() << ":" << e.errorMesssage() << std::endl;
	}
}

//---------------------------------------------------------------------------
// closeDb
//
// Used to close the embedded database.
//---------------------------------------------------------------------------
int closeDb() {
	printf("Closing database!\n");
	try {
		sqlite3_close(db);
	}
	catch (CppSQLiteException& e)
	{
		std::cerr << e.errorCode() << ":" << e.errorMessage() << std::endl;
	}
	return 0;
}

//---------------------------------------------------------------------------
// checkRef
//
// Queries to see if there is an existing record,
// and if so checks number of references.
//---------------------------------------------------------------------------
int checkRef(char* fHash) {
	sqlite3_stmt *selectStmt;
	char* query;
	int ret;
	query = strcat("SELECT allowed FROM vdb WHERE hash = ", fHash);
	sqlite3_prepare(db, query, strlen(query)+1, &selectStmt, NULL);
	if (sqlite3_step(stmt) == SQLITE_ROW) { //if result
		if (sqlite3_column_text(selectStmt, 0) > 5) {
			ret = 1;	//malicious
		}
		else ret = 0;	//likely not malicious
	}
	else ret = -1;	//not found
	sqlite3_finalize(selectStmt);
	return ret;
}

//---------------------------------------------------------------------------
// newEntry
//
// In the event a process is not in our records, 
// we add it with this function.
//---------------------------------------------------------------------------
int newEntry(char* name, char* hash, int allowed) {
	sqlite3_stmt *insertStmt;
	std::stringstream createQ;
	createQ << "INSERT INTO vdb (name, hash, allowed) VALUES ('" << name
		<< "', '" << hash
		<< "', " << allowed << ")";
	try {
		sqlite3_prepare(db, createQ.str().c_str(), createQ.seekg(0, ios::end).tellg(), &insertStmt, NULL);
	}
	catch (CppSQLiteException& e)
	{
		std::cerr << e.errorCode() << ":" << e.errorMessage() << std::endl;
	}
	return 0;
}

#endif