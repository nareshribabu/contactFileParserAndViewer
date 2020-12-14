/****************************************/
/*
Name: Nareshri Babu
ID: 0956951 
Course: CIS 2750
*/
/****************************************/

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "LinkedListAPI.h"
#include "VCParser.h"

/***** Major Initializers *****/
VCardErrorCode newVCard(Card** newCardObject);
Property * newProperty();
Parameter * newParam();
DateTime * newDate();

//Major Create Helper Functions

//Date Functions
DateTime * createDate (Card** obj, char * substringStart, char * substringEnd);
int extractDateTime(DateTime *date, char * string);
int checkDateParam(char *string);

//Properties
int createOptionalProperties (Card** obj, char * substringStart, char * substringEnd, char * group);
int createFN (Card** obj, char * substringStart, char * substringEnd, char * group);

int extractCompoundValues (Property* prop, char* string);
int extractParams(Property* prop, char* string);
int extractSingleParam (Property* prop, char* string);
int extractCompoundParam (Property* prop, char* string);


//Folded Lines
int checkFoldedLine (FILE *fp, char **line, int *numFileLines);

/***** Mini helper/checker functions *****/
int caseInsensitiveCmp (char * string1, char * string2);
int whiteSpaceChar (char character);
int substringSearch (char * haystack, char * needle);
int checkFN (char * string);
int checkDates (char * string);
int excludeProp (char * string);
int checkFileName (char *string);


/******************* Assignment 2 *************************/
//Mod 3
int fillInProp(Property* prop, char * string);
int fillInDate(DateTime* date, char * string);


//Mod 2
VCardErrorCode checkDatesInOptionalList(Property * prop); //checks BDAY and ANNIVERSARY in optional properties list
VCardErrorCode checkBVEFun(Property * prop); //checks for Begin, Version and End in optional properties list
VCardErrorCode checkValidPropNames(Property * prop); //Checks if the property name matches one of the properties from Section 6.1 - 6.9.3
VCardErrorCode checkValueCardinality (Property * prop);

int checkKindProperty (List * list);
int checkKindValue (List * list);

VCardErrorCode validateOptionalPropertiesListDocs (List * list); //deals with the specs
VCardErrorCode validateFNStructure(Property * prop); //Checks if the property structure for FN is valid
VCardErrorCode validateFNDocs(Property * fn); //checks the cardinality of FN's values list
VCardErrorCode validateOptionalPropertiesStructure(List * list); //deals with the structure
VCardErrorCode validateParameterStructure(List * list); //Checks that structure is valid and parameter value and name are not empty strings
VCardErrorCode validateValuesList(List * list); //goes through the values list and checks that non of the values are NULL


VCardErrorCode validateDates(DateTime *date); //checks everything related to date from the card obj
VCardErrorCode validateDateFormat (char * date); //checks the length of dates to see if it is in the correct format
VCardErrorCode validateTimeFormat (char * time); //checks the time of dates to see if it is in the correct format

//Mod 1
char* propertyToContentLine(void* prop);
char* parameterToContentLine(void* param);
char* valueToContentLine(void* val);
char* dateToContentLine(void* date);
int checkFileNameWrite (const char* string);