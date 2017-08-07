//---------------------------------------------------------------------------
//
// sqldb.h  
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

//---------------------------------------------------------------------------
// openDb
//
// Used to open the embedded database.
//---------------------------------------------------------------------------
int openDb();

//---------------------------------------------------------------------------
// closeDb
//
// Used to close the embedded database.
//---------------------------------------------------------------------------
int closeDb();

//---------------------------------------------------------------------------
// checkRef
//
// Queries to see if there is an existing record,
// and if so checks number of references.
//---------------------------------------------------------------------------
int checkRef(char* fHash);

//---------------------------------------------------------------------------
// newEntry
//
// In the event a process is not in our records, 
// we add it with this function.
//---------------------------------------------------------------------------
int newEntry(char* name, char* hash, int allowed);
