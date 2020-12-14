/****************************************/
/*
Name: Nareshri Babu
ID: 0956951 
Course: CIS 2750
*/
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "VCParser.h"
#include "LinkedListAPI.h"
#include "VCHelpers.h"

// *************************************************************************

// *************             Major Initializers              *************** 

// ************************************************************************/

/*****Allocates a new VCard object*****/
VCardErrorCode newVCard(Card** newCardObject) {


    //malloc space for a new card object
    (*newCardObject) = malloc(sizeof(Card *) * 500);
    if ((*newCardObject) == NULL) {
        return OTHER_ERROR;

    }

    //new property object for fn
    (*newCardObject)->fn = newProperty();
    if (((*newCardObject)->fn) == NULL) {
        return OTHER_ERROR;
    }
 
 
    //initalize the optional property list
    void (*deleteProp)(void *) = &deleteProperty;
    int (*cmpProp)(const void*, const void*) = &compareProperties;
    char* (*propToStr)(void*) = &propertyToString;

    (*newCardObject)->optionalProperties = initializeList(propToStr, deleteProp, cmpProp); 

    //Individual's birthday
    //Must be NULL, so I will assign NULL to it until we find a date, in which case I will malloc it (call newDate)
    (*newCardObject)->birthday = NULL;

    //Individual's marriage, or equivalent, anniversary
    //Must be NULL, so I will assign NULL to it until we find a date, in which case I will malloc it (Call newDate)
    (*newCardObject)->anniversary = NULL;

    return OK;
}

/*****Initalizes a property*****/
Property * newProperty() {

    //malloc property object
    Property * prop = malloc(sizeof(Property));
    if (prop == NULL) {
        return NULL;
    }

    //malloc property name
    prop->name = malloc(sizeof(char) * 500);
    if (prop->name == NULL) {
        return NULL;
    }


    //malloc property group name
    prop->group = malloc(sizeof(char) * 500); 
    if (prop->group == NULL) {
        return NULL;
    }

    //initialize parameter list
    void (*deleteParam)(void *) = &deleteParameter;
    int (*cmpParam)(const void*, const void*) = &compareParameters;
    char* (*paramToStr)(void*) = &parameterToString;

    prop->parameters = initializeList(paramToStr, deleteParam, cmpParam);

    //initialize value list
    void (*deleteVal)(void *) = &deleteValue;
    int (*cmpVal)(const void*, const void*) = &compareValues;
    char* (*ValToStr)(void*) = &valueToString;

    prop->values = initializeList(ValToStr, deleteVal, cmpVal);

    return prop;
}

/*****Initalizes a parameter*****/
Parameter * newParam() {

    Parameter * param = malloc(sizeof(Parameter));
    if (param == NULL) {
        return NULL;
    }

    param->name = malloc(sizeof(char) * 500);
    if (param->name == NULL) {
        return NULL;
    }

    param->value = malloc(sizeof(char) * 500);
    if (param->value == NULL) {
        return NULL;
    }

    return param;

}


/*****Initalizes Date time*****/
DateTime * newDate() { //call this if I find a date for the card struct, it is not malloced and set to NULL so I HAVE to call this

    DateTime * dateVar = malloc(sizeof(DateTime));
    if (dateVar == NULL) {
        return NULL;
    }

    dateVar->date = malloc(sizeof(char) * 500);
    if (dateVar->date == NULL) {
        return NULL;
    }

    dateVar->time = malloc(sizeof(char) * 500);
    if (dateVar->time == NULL) {
        return NULL;
    }

    dateVar->text = malloc(sizeof(char) * 500);
    if (dateVar->text == NULL) {
        return NULL;
    }


    return dateVar;
    
}

// *************************************************************************

// *************          Create Helper Functions            *************** 

// ************************************************************************/

// put everythign inside the date structs 
DateTime * createDate (Card** obj, char * substringStart, char * substringEnd) {

    //check if any of these are null, if so return
    if ((obj == NULL) || (substringStart == NULL) || (substringEnd == NULL)) {
        return NULL; //E
    }


    //initialize a new date object
    DateTime *date = newDate(); //GOTTA FREE THIS WHEN UR DONE
    if (date == NULL) {
        return NULL;
    }


    //check to see if string has Z or not (UTC or not)
	//bool	UTC;
    int lenOfString = strlen(substringEnd);

    char *result = strstr(substringEnd, "Z"); 
    char *result2 = strstr(substringEnd, "z");

    if((result != NULL) || (result2 != NULL)) { //if the string has the letter Z at the end of it //! if there is an issue

        date->UTC = true;

        int indexColon = 0;

        if (result != NULL) {
            indexColon = (int)(result - substringEnd);
        }
        else if (result2 != NULL) {
            indexColon = (int)(result2 - substringEnd);
        }
        else {
            return NULL; //E 
        }
        

        //GET RID OF THE Z
        char *new = calloc((lenOfString + 256), sizeof(char));

        strncpy(new, substringEnd, indexColon);
        
        strcpy(substringEnd, new);

        free(new);
        
    }
    else { //the string does not have the letter z in it
        date->UTC = false;
    }

    //if the second half of the substring has a ; means there is a param
    if (substringEnd[0] == ';') {

        int endLen = (strlen(substringEnd)) + 256;
        char value[endLen];
        char param[endLen];
        
        //get rid of the ; from substringEnd
        substringEnd = substringEnd + 1;

        char *colon = strstr(substringEnd, ":"); 
        if (colon != NULL) { //if substringend has a : (value)
            int indexColon = (int)(colon - substringEnd);

            //fill in the first half not including the : (Param)
            memset(param, '\0', endLen);
            strncpy(param, substringEnd, (indexColon));
            
            //fill in the second half of the substring (Value)
            memset(value, '\0', endLen);
            strncpy(value, substringEnd + indexColon + 1, (strlen(substringEnd) - indexColon));

        }
        else {
            deleteDate(date);
            return NULL; //E substringend does not have an :, no value, invalid date
        }


        //check to see if the param value is text
        int resultDateParam = checkDateParam(param);
        if (resultDateParam == 1) { //date is text
            //if it is then set bool isText to true
            date->isText = true;

            strcpy(date->date, ""); //date and time must be empty string if date is text
            strcpy(date->time, "");

            int dateValLength = strlen(value);
            date->text = (char *)realloc(date->text, (dateValLength + 256));
            strcpy(date->text, value); //store the text date time into the struct //realloc

        }
        else if (resultDateParam == 0) { //date/time is NOT text

            //else set isText to false
            date->isText = false;

            //Set text to ""
            strcpy(date->text, "");

            //extract date and time
            int extractResult = extractDateTime(date, value);
            if (extractResult == -1) { 
                deleteDate(date);
                return NULL; //E 
            }
        }
        else {
            deleteDate(date);
            return NULL; //E not a proper date param
        }


    }
    else if (substringEnd[0] == ':') { //no param just date, NOT text

        //else set isText to false
        date->isText = false;

        //Set text to ""
        strcpy(date->text, "");

        //get rid of the ; from substringEnd
        substringEnd = substringEnd + 1;
        
        //extract date and time
        int extractResult = extractDateTime(date, substringEnd);
        if (extractResult == -1) { 
            deleteDate(date);
            return NULL; //E 
        }
    }
    else {
        deleteDate(date);
        return NULL; //E no ; or : invalid date format
    }

   
 
    return date;


}



//puts everything inside the optional properties list
int createOptionalProperties (Card** obj, char * substringStart, char * substringEnd, char * group) { 

    //check if any of these are null, if so return
    if ((obj == NULL) || (substringStart == NULL) || (substringEnd == NULL) || (group == NULL)) {
        return -1; //E
    }

    //create a new property object
    Property* prop = newProperty();
    if (prop == NULL) {
        return 6;
    }

    //Fill in group
    int propGrpLen = strlen(group);
    prop->group = (char *)realloc(prop->group, (propGrpLen + 256));
    strcpy(prop->group, group);

    //Fill in name
    int propNameLen = strlen(substringStart);
    prop->name = (char *)realloc(prop->name, (propNameLen + 256));
    strcpy(prop->name, substringStart);



    //Fill in list of parameters and Fill in list of values

        if (substringEnd[0] == ';') { //parameters and then values
            //call the function that helps grab parameters from the second substring 
            int returnValParam = extractParams(prop, substringEnd + 1);
            if (returnValParam == -1) {
                //param is missing a = is invalid so return -1
                //E
                deleteProperty(prop);
                return -1;
            }
            else if (returnValParam == 6) {
                deleteProperty(prop);
                return 6;
            }

        }
        else if (substringEnd[0] == ':') { //just values

            //if ; is in substringEnd then that means its a compound value
            if (strstr(substringEnd, ";") != NULL) {
              
                int returnVal = extractCompoundValues(prop, substringEnd + 1); 
                if (returnVal == -1) {
                    //param is missing a = is invalid so return -1
                    //E
                    deleteProperty(prop);
                    return -1;
                }
                else if (returnVal == 6) {
                    deleteProperty(prop);
                    return 6;
                }
            }
            else { //not a compound value so just stick it in!

                if (substringEnd[0] != '\0') { //checking if this does not contain a value (string is null)
                    char *valueString = malloc(sizeof(char) * (strlen(substringEnd) + 256)); //we are mallocing this because we have not malloced individual strings in the list, just the list is initalized
                    if (valueString == NULL) {
                        return 6;
                    }


                    strcpy(valueString, (substringEnd + 1));
                    insertBack(prop->values, valueString);
                }
                else { //No value return error code
                    deleteProperty(prop);
                    return -1; //E
                }
        
            }
        }

    insertBack((*obj)->optionalProperties, prop);

    return 1;
}


//puts everything in the FN object in the card object
int createFN (Card** obj, char * substringStart, char * substringEnd, char * group) {

    //check if any of these are null, if so return
    if ((obj == NULL) || (substringStart == NULL) || (substringEnd == NULL) || (group == NULL)) {
        return -1; //E
    }

    //Fill in group
    int grpLen = strlen(group);
    (*obj)->fn->group = (char *)realloc((*obj)->fn->group, (grpLen + 256));
    strcpy((*obj)->fn->group, group);

    //Fill in name
    int fnLen = strlen(substringStart);
    (*obj)->fn->name = (char *)realloc((*obj)->fn->name, (fnLen + 256));
    strcpy((*obj)->fn->name, substringStart);

    
    //Fill in list of parameters and Fill in list of values

        if (substringEnd[0] == ';') { //parameters and then values
            //call the function that helps grab parameters from the second substring 
            int returnValParam = extractParams((*obj)->fn, substringEnd + 1);
            if (returnValParam == -1) {
                //param is missing a = is invalid so return -1
                //E
                return -1;
            }
            else if (returnValParam == 6) {
                return 6;
            }

        }
        else if (substringEnd[0] == ':') { //just values

            //if ; is in substringEnd then that means its a compound value
            if (strstr(substringEnd, ";") != NULL) {
              
                int returnVal = extractCompoundValues((*obj)->fn, substringEnd + 1); 
                if (returnVal == -1) {
                    //param is missing a = is invalid so return -1
                    //E
                    return -1;
                }
                else if (returnVal == 6) {
                    return 6;
                }
            }
            else { //not a compound value so just stick it in!

                //check if the second half of the string is NOT just : and NOT ; and NOT ""
                if ((strcmp(substringEnd, ":") != 0) && (strcmp(substringEnd, ";") != 0) && (strcmp(substringEnd, "") != 0)) { //checking if this does NOT contain a value (string is null) (Already checked before)
                    char *valueString = malloc(sizeof(char) * (strlen(substringEnd) + 256)); //we are mallocing this because we have not malloced individual strings in the list, just the list is initalized
                    if (valueString == NULL) {
                        return 6;
                    }


                    memset(valueString, '\0', (sizeof(char) * (strlen(substringEnd) + 256)));
                    strcpy(valueString, (substringEnd + 1));

                    insertBack((*obj)->fn->values, valueString);
                }
                else { //No value return error code
                    return -1; //E
                }
        
            }
        }

    return 1;
}

//extracts the date and time given a string: returns 1 if everything went ok, else returns -1
int extractDateTime(DateTime *date, char * string) {

    if ((date == NULL) || (string == NULL)) {
        return -1; //E null
    }

    
    if (strcmp(string, "\r\n") == 0) {
        return -1; //E there is no date and/or time
    }
    
    int resultIndex = substringSearch (string, "t");
    if (resultIndex != -1) { //has T (or could be null but checked for it before sending it in so its okay)

        //if first letter is T, no date, just a time
        if (string[0] == 't') {
            strcpy(date->date, ""); //date is set to an empty string

            //Get rid of the T
            string = string + 1;

            //check if time is valid
            //check if after T there is values
            if ((isdigit(string[0]) != 0 ) || (string[0] == '-')) { //if the first char (after the T) is a digit

                int dateLenInFn = strlen(string) + 256;
                date->time = (char *)realloc(date->time, dateLenInFn);
                strcpy(date->time, string); //fill in time value

            }
            else { //if the first char (after the T) is NOT a digit
                return -1; //E there is nothign after the T, even though T indicated there would be a time

            }
        
        }
        else { //first letter is NOT T, both date and time

            int dateLen = strlen(string);
            int size = dateLen + 256;
            char dateBuff[size];
            char timeBuff[size]; 

            //fill in the first half (after groups) not including the :
            memset(dateBuff, '\0', size);
            strncpy(dateBuff, string, (resultIndex));
            
            //fill in the second half of the substring
            memset(timeBuff, '\0', size);
            strncpy(timeBuff, string + resultIndex + 1, (strlen(string) - resultIndex));

            //copy the date into the date struct object
            int dateDateLen = strlen(dateBuff) + 256;
            date->date = (char *)realloc(date->date, dateDateLen);
            strcpy(date->date, dateBuff);


            //check if after T there is values
            if ((isdigit(timeBuff[0]) != 0 ) || (string[0] == '-')) { //if the first char (after the T) is a digit
                int dateTimeLen = strlen(timeBuff) + 256;
                date->time = (char *)realloc(date->time, dateTimeLen);
                strcpy(date->time, timeBuff); //fill in time value
            }
            else { //if the first char (after the T) is NOT a digit
                return -1; //E there is nothing after the T, even though T indicated there would be a time
            }
        }

    }
    else { //does NOT have T, so only date

        strcpy(date->time, ""); //time is set to empty

        //copy the date into the date struct object
        int onlyDateLen = strlen(string) + 256;
        date->date = (char *)realloc(date->date, onlyDateLen);
        strcpy(date->date, string);

    }

    return 1;
}

//a string that MAY include parameters and values is sent through as well as the property (need to access the list)
int extractParams(Property* prop, char* string) {

    if ((prop == NULL) || (string == NULL)) {
        return -1;
    }

    int indexColon = 0;
    int indexSemi = 0;
    char* resultColon; 
    char* resultSemi;
    char* checkVal;


    int lenExtractParam = 0;
    lenExtractParam = strlen(string) + 256;


    char firstSubString[lenExtractParam];
    char secondSubString[lenExtractParam];

    //Index of colon :
    resultColon = strstr(string, ":");
    if(resultColon != NULL) { //if : exists in the string

        indexColon = (int)(resultColon - string);

    }
    else { //if : does not exist in the string then it is INVALID
        return -1; //E
    }


    //Index of semicolon ;
    resultSemi = strstr(string, ";");
    if(resultSemi != NULL) { //if ; exists in the string (means there is either compound params or compound values)
        indexSemi = (int)(resultSemi - string);

        //check which one comes first ; or :
        if (indexColon < indexSemi) { // if : comes first that means its the ; comes after the : which means its compound value and single param

            //fill in the first half substring
            memset(firstSubString, '\0', lenExtractParam);
            strncpy(firstSubString, string, (indexColon));

            //send it to the function that extracts a single parameter
            int returnVal = extractSingleParam (prop, firstSubString);
            if (returnVal == -1) {
                //param is missing a = is invalid so return -1
                //E
                return -1;
            }
            else if (returnVal == 6) {
                return 6;
            }

            //fill in the second half of the substring
            memset(secondSubString, '\0', lenExtractParam);
            strncpy(secondSubString, string + indexColon + 1, (strlen(string) - indexColon));

            //Secondsubstring is a compound value - pass it into the function for compound value)
            int exCompReturn = extractCompoundValues (prop, secondSubString); 
            if (exCompReturn == -1) {
                //param is missing a = is invalid so return -1
                //E
                return -1;
            }
            else if (exCompReturn == 6) {
                return 6;
            }


        }
        else { //if ; comes before : then it is a compound param (could be a compound or a single value)

            //grab the first half substring containing the compount params and send it to the compound param function
            memset(firstSubString, '\0', lenExtractParam);
            strncpy(firstSubString, string, (indexColon));

            //call the compound param function
            int val = extractCompoundParam (prop, firstSubString);
            if (val == -1) {
                //param is missing a = is invalid so return -1
                //param doesn't contain any words after the ;
                //E
                return -1;
            }
            else if (val == 6) {
                return 6;
            }
            

            //grab the second half substring containing the values- compound or single
            memset(secondSubString, '\0', lenExtractParam);
            strncpy(secondSubString, string + indexColon + 1, (strlen(string) - indexColon));

            //check if after the : there is a ; or not (compound value - pass it into the function for compound value)
            checkVal = strstr(secondSubString, ";");
            if(checkVal != NULL) { //if ; exists in the string - compound value

                //int indexCheckVal = (int)(checkVal - secondSubString);
                int cValResult = extractCompoundValues (prop, secondSubString);
                if (cValResult == -1) {
                    //param is missing a = is invalid so return -1
                    //param doesn't contain any words after the ;
                    //E
                    return -1;
                }
                else if (cValResult == 6) {
                    return 6;
                }

            }
            else { //if ; does not exist in the string - single value

                //if after : there is no ; then stick in the value into the linked list
                char *valueString = malloc(sizeof(char) * (strlen(secondSubString) + 256)); //we are mallocing this because we have not malloced individual strings in the list, just the list is initalized
                if (valueString == NULL) {
                    return 6;
                }


                strcpy(valueString, secondSubString);
                insertBack(prop->values, valueString);

            }
        }
    }
    else { //another ; does not exist in this string, this is a single param with a single value 

        //fill in the first half substring containing the single param
        memset(firstSubString, '\0', lenExtractParam);
        strncpy(firstSubString, string, (indexColon));

        //send it to the function that extracts a single parameter
        int returnVal = extractSingleParam (prop, firstSubString);
        if (returnVal == -1) {
            //param is missing a = is invalid so return -1
            //E
            return -1;
        }
        else if (returnVal == 6) {
            return 6;
        }

        //grab the second half substring containing the single value and stick it in the linked list
        memset(secondSubString, '\0', lenExtractParam);
        strncpy(secondSubString, string + indexColon + 1, (strlen(string) - indexColon));

        if (secondSubString[0] != '\0') { //checking if this does not contain a value (string is null)
            char *valueStringElse = malloc(sizeof(char) * (strlen(secondSubString) + 256)); //we are mallocing this because we have not malloced individual strings in the list, just the list is initalized
            if (valueStringElse == NULL) {
                return 6;
            }

            strcpy(valueStringElse, secondSubString);
            insertBack(prop->values, valueStringElse);
        }
        else { //No value return error code
            return -1; //E
        }
        

    }
    
    return 1;

}

int extractCompoundParam (Property* prop, char* string) {

    if ((prop == NULL) || (string == NULL)) {
        return -1;
    }

    char* end = NULL;
    int index = 0;
    int lenCP = 0;
    lenCP = strlen(string) + 256;

    char buffer[lenCP]; //used for collecting the word and sending it in the single param function

    //grab the first param outside because there is no ; to guide the while loop (the string comes in without the first ;)
    //tells it where the next ; is
    end = strstr(string, ";");
    if(end != NULL) {
        index = (int)(end - string);
    }
    else {
        return -1; //E !! there isnt a ; so it is not a compound value
    }

    //extract the first param starting at the begining of the string and ending where the next semicolon is
    memset(buffer, '\0', lenCP);
    strncpy(buffer, string, index);

    if (strcmp(buffer, "") == 0) { //check if buffer is null, if it is that means that there is nothing after the ; INV_PROP
        return -1; //E Invalid param
    }

    //send extracted buffer into the single param function
    int returnVal = extractSingleParam (prop, buffer);
    if (returnVal == -1) {
        //param is missing a = is invalid so return -1
        //E
        return -1; 
    }
    else if (returnVal == 6) {
        return 6;
    }


    //iterate the string so that it now excludes the word sent into the list and the ;   
    string = string + index + 1;

    //finds the first ; and calculates the index of that ; (calculating it again because the string has now been updated that means index has to as well)
    end = strstr(string, ";");
    if(end != NULL) {
        index = (int)(end - string);
    }


    //iterate while there is a ;
    while (end != NULL) {

        //clear buffer and copy the string before the ; and put it inside the list
        memset(buffer, '\0', lenCP);
        strncpy(buffer, string, index);
        
        if (strcmp(buffer, "") == 0) { //check if buffer is null, if it is that means that there is nothing after the ; INV_PROP
            return -1; //E Invalid param
        }

        //send extracted string into the single param function
        returnVal = extractSingleParam (prop, buffer);
        if (returnVal == -1) {
            //param is missing a = is invalid so return -1
            //E
            return -1; 
        }
        else if (returnVal == 6) {
            return 6;
        }
 

        //iterate the string so that it now excludes the word sent into the list and the ;   
        string = string + index + 1;

        //re-find the next ; (if there is no more ; then end will be NULL and loop will exit)
        end = strstr(string, ";");
        index = (int)(end - string);

    }

        memset(buffer, '\0', lenCP);
        strcpy(buffer, string);
        if (strcmp(buffer, "") == 0) {
            return -1; //E Invalid param
        }

        returnVal = extractSingleParam (prop, buffer);
        if (returnVal == -1) {
            //param is missing a = is invalid so return -1
            //E
            return -1; 
        }
        else if (returnVal == 6) {
            return 6;
        }

    return 1;
}


int extractSingleParam (Property* prop, char* string) {

    char* result;
    int index;

    int lenSP = 0;
    lenSP = strlen(string) + 256;

    char firstSubString[lenSP];
    char secondSubString[lenSP];

    //prepare a param struct object
    Parameter* param = newParam();
    if (param == NULL) {
        return 6;
    }

    result = strstr(string, "=");
    if(result != NULL) { //if = exists in the string

        index = (int)(result - string);

        //fill in the first half substring
        memset(firstSubString, '\0', lenSP);
        strncpy(firstSubString, string, (index));

        if (strcmp(firstSubString, "") != 0) { //if it is not an empty string
            int paramNameLen = strlen(firstSubString);
            param->name = (char *)realloc(param->name, (paramNameLen + 256));
            strcpy(param->name, firstSubString);
        }
        else { //if it is an empty string
            deleteParameter(param);
            return -1; //invalid first half of param does not exist
        }

        //fill in the second half of the substring
        memset(secondSubString, '\0', lenSP);
        strncpy(secondSubString, string + index + 1, (strlen(string) - index));

        if (strcmp(secondSubString, "") != 0) { //if it is not an empty string
            int paramValueLen = strlen(secondSubString);
            param->value = (char *)realloc(param->value, (paramValueLen + 256));
            strcpy(param->value, secondSubString);
        }
        else {
            deleteParameter(param);
            return -1; //invalid second half of param does not exist 
        }

        //send in the parameter node into the linkedlist
        insertBack(prop->parameters, param);
        

    }
    else { //if = does not exist in the string then param is INVALID
        deleteParameter(param);
        return -1; //E 
    }

    return 1;
}

//checks for date params: 
//returns a 1 if the date param value is text
//if the param is not formatted correctly returns a -1
//returns a 0 if the date param value is not a text

int checkDateParam(char *string) {

    if (string == NULL) {
        return -1; //E this is INV_PROP check for this 
    }

    char* result;
    int index;

    int datel = 0;
    datel = strlen(string) + 256;

    char firstSubString[datel];
    char secondSubString[datel];

    result = strstr(string, "=");
    if(result != NULL) { //if = exists in the string

        index = (int)(result - string);

        //fill in the first half substring
        memset(firstSubString, '\0', datel);
        strncpy(firstSubString, string, (index));

        //fill in the second half of the substring
        memset(secondSubString, '\0', datel);
        strncpy(secondSubString, string + index + 1, (strlen(string) - index));

        if (strcmp(firstSubString, "") == 0) { //first part of the param is empty, invalid property
            return -1;
        }

        if (strcmp(secondSubString, "") == 0) { //second part of the param is empty, invalid property
            return -1;
        }

        if ((strcmp(secondSubString, "text") == 0) || (strcmp(secondSubString, "TEXT") == 0) || (strcmp(secondSubString, "Text") == 0)){
            return 1;
        }
        else if ((substringSearch (secondSubString, "date-and-or-time")) == -1) { //-1 means it does not have
            return -1; //if it does not have this as a param value then it is invalid
        }

        

    }
    else { //if = does not exist in the string then param is INVALID
        return -1; //E 
    }

    return 0;

}



// a string of compound values is sent through and each value is extracted and inserted into the linked list
int extractCompoundValues (Property* prop, char* string) {

    char* end = NULL;
    int index = 0;

    int lenBuf = 0;
    lenBuf = (strlen(string)) + 256;

    char buffer[lenBuf]; //used for collecting the word and sending it in the linkedlist
    char *stringPointer = string; //this is the pointer to the start of the original string
                                  // the string will get modified but at the end of the function we will set it back to the original string

    //finds the first ; and calculates the index of that ;
    end = strstr(string, ";");
    if(end != NULL) {
        index = (int)(end - string);
    }
    else {
        return -1; //E !! CHECK THE RETURN TYPE LIKE WHAT HAPPENS IF IT DOES RETURN IN THE CHAIN OF FUNCTIONS
    }

    while (end != NULL) {

        //checks for single semi colons without any words -> those are considered as ""
        if (index == 0) {
            
            memset(buffer, '\0', lenBuf);
            strcpy(buffer, "");

            int copyBufferLength = strlen(buffer) + 256;
            char *copy = malloc(sizeof(char*) * copyBufferLength);
            if (copy == NULL) {
                return 6;
            }
        
            strcpy(copy, buffer);
            insertBack(prop->values, copy);

        }
        else {
    
            //clear buffer and copy the string before the ; and put it inside the list
            memset(buffer, '\0', lenBuf);
            strncpy(buffer, string, index);

            int copyBufferLength2 = strlen(buffer) + 256;
            char *copy = malloc(sizeof(char*) * copyBufferLength2);
            if (copy == NULL) {
                return 6;
            }
            strcpy(copy, buffer);
            insertBack(prop->values, copy);

        }


        //iterate the string so that it now excludes the word sent into the list and the ;   
        string = string + index + 1;

        //re-find the next ; (if there is no more ; then end will be NULL and loop will exit)
        end = strstr(string, ";");
        index = (int)(end - string);


    }

    //grab the remaining word (last string) and put that inside the list
    int copy2Length = strlen(string) + 256;
    char *copy2 = malloc(sizeof(char*) * copy2Length);
    if (copy2 == NULL) {
        return 6;
    }

    strcpy(copy2, string);
    insertBack(prop->values, copy2);
    
    //set the original string back to where it was pointing (might help with the freeing part)
    string = stringPointer;

    return 1;
}


// int checkFoldedLine (FILE *fp, char *line, int *numFileLines) {

//     char unFoldedChar = '\0';
//     char foldedLine[256];
//     int length = strlen(line);

//     unFoldedChar = fgetc(fp);

//     if (unFoldedChar != EOF) {//if the next char is not EOF (you have not hit the end of the file)
//         if (whiteSpaceChar (unFoldedChar) == 1) { //if the next char is a white space

//             //gets the next line in the file
//             fgets(foldedLine, sizeof(foldedLine), fp); //E check if fgets returns normal or if its NULL

//             //E check if this line contains atleast one character or else its an invalid line/card
//             if (strlen(foldedLine) < 3) { //the next line does not have /r/n removed
//                 return -1; 
//             }

//             //remove the /r and /n from the line before (line)
//             //!! Check if this needs to be checked for every line
//             if ((line[length - 2] == '\r') && (line[length - 1] == '\n')) { //line's last to chars are \r\n
//                 line[length - 2] = '\0';
//             }
//             // else { //line's last chars do not contain BOTH /r/n (not valid according to the documentation)
                
//             //     line[length] = '\0';

//             //     //return -1; //E
//             // }

//             //remove \r and \n from the folded line as well
//             int lengthOfFullLine = 0;
//             lengthOfFullLine = strlen(foldedLine);

//             if ((foldedLine[lengthOfFullLine - 2] == '\r') && (foldedLine[lengthOfFullLine - 1] == '\n')) { 
//                 foldedLine[lengthOfFullLine - 2] = '\0';
//             }
//             else { //line's last chars do not contain BOTH /r/n (not valid according to the documentation)
//                 return -1;
//             }

//             strcat(line, foldedLine);


//             (*numFileLines) = (*numFileLines) - 1;

//         }
//         else {

//             //inside the if statement we increased the file pointer by 1 char so we need to decrease the file pointer back 1 char (or else the first letter is deleted)
//             fseek(fp, -1, SEEK_CUR); //E check if this function executed properly or not
//         }
//     }

//     return 1;

// }

/*************** Experimental functions: if it doesnt work remove it *******************/

int checkFoldedLine (FILE *fp, char **line, int *numFileLines) {

    char unFoldedChar = ' '; //initlaize it with a space so it goes inside the while loop
    char foldedLine[2000] = ""; 
    int length = 0;


    while (whiteSpaceChar(unFoldedChar) == 1) {
 
        unFoldedChar = fgetc(fp);

        if (unFoldedChar != EOF) {//if the next char is not EOF (you have not hit the end of the file)
            if (whiteSpaceChar (unFoldedChar) == 1) { //if the next char is a white space

                //gets the next line in the file
                char *fgetReturn = fgets(foldedLine, sizeof(foldedLine), fp); //E check if fgets returns normal or if its NULL


                if (fgetReturn == NULL) { //reached end of file //!! Maybe remove this
                    return -1; //E end of file or fgets error
                }

                //E check if this line contains atleast one character or else its an invalid line/card
                if (strlen(foldedLine) < 3) { //the next line does not have /r/n removed
                    return -1; 
                }

            
                //remove the \r and \n from the line before (line)
                length = strlen((*line));
                if (((*line)[length - 2] == '\r') && ((*line)[length - 1] == '\n')) { //line's last to chars are \r\n
                    (*line)[length - 2] = '\0';
                }


                //remove \r and \n from the folded line as well
                int lengthOfFullLine = 0;
                lengthOfFullLine = strlen(foldedLine);

                if ((foldedLine[lengthOfFullLine - 2] == '\r') && (foldedLine[lengthOfFullLine - 1] == '\n')) { 
                    foldedLine[lengthOfFullLine - 2] = '\0';
                }
                else { //line's last chars do not contain BOTH /r/n (not valid according to the documentation)
                    return -1;
                }


                (*line) = (char *)realloc((*line), (lengthOfFullLine + length + 256));
                strcat((*line), foldedLine);

                //Reduce the number of files count by 1 for every line unfolded
                (*numFileLines) = (*numFileLines) - 1;

            }
            else {

               //inside the if statement we increased the file pointer by 1 char so we need to decrease the file pointer back 1 char (or else the first letter is deleted)
               fseek(fp, -1, SEEK_CUR); //E check if this function executed properly or not
            }
        }
    }



    return 1;

}






// *************************************************************************

// *************          Small Helper Functions             *************** 

// ************************************************************************/

/***** check if 2 strings are same or not, case insensitive: return 1 if word is same, return -1 if word is not same *****/
int caseInsensitiveCmp (char * string1, char * string2) {

    if ((string1 == NULL) || (string2 == NULL)) {
        return 0;
    }

    //Change both strings to lowercase
    for(int i = 0; string1[i]; i++){
        string1[i] = tolower(string1[i]);
    }

    // for(int i = 0; string2[i]; i++){
    //     string2[i] = tolower(string2[i]);
    // }

    if (strcmp(string1, string2) == 0) { //if the 2 words are the same
        return 1;
    }
    else { //if the 2 words are NOT the same
        return -1;
    }

}

/***** Checks if a character is a space or a horizontal tab, returns 1 if it is a space or tab, otherwise it returns a -1 *****/
int whiteSpaceChar (char character) {

    if ((character == ' ') || (character == '\t')) {  
        return 1;
    }
    else {
        return -1;
    }
}


/***** checks if a string contains another string - case insensitive, returns index if word is contained, else returns -1 *****/
int substringSearch (char * haystack, char * needle) {

    if ((haystack == NULL) || (needle == NULL)) {
        return 0; //!! //E check this
    }

    char *resultStr;
    int index = 0;

    for(int i = 0; haystack[i]; i++){
        haystack[i] = tolower(haystack[i]);
    }

    resultStr = strstr(haystack, needle);

    if(resultStr != NULL) {
        index = (int)(resultStr - haystack);
        return index;
    }
    else {
        return -1;
    }

}

/******** Checks if the property (char) sent through is FN - case insensitive. 1 if it is FN and -1 if it is not FN ******/
int checkFN (char * string) {

    if (string == NULL) {
        return 0;
    }

    //if the string passed in is contains an FN then it is the property FN and return 1
    if((strcmp(string, "FN") == 0) 
    || (strcmp(string, "fn") == 0)
    || (strcmp(string, "Fn") == 0)
    || (strcmp(string, "fN") == 0))  {

        return 1;
    }
    else {
        return -1;
    }

}

/******* Checks if the property passed in is a date (BDAY or ANNIVERSARY), if it is it sends back a 1, if not then a -1 *****/
int checkDates (char * string) {

    if (string == NULL) {
        return 0;
    }

    char *copy = malloc(sizeof(char) * (strlen(string) + 256));
    if (copy == NULL) {
        return 6;
    }

    strcpy(copy, string);
    

    for(int i = 0; copy[i]; i++){
        copy[i] = tolower(copy[i]);
    }


    //if the string contains a bday OR anniversary
    if ((strcmp(copy, "bday") == 0)
    || (strcmp(copy, "anniversary") == 0)) {
    
        free(copy);
        return 1;

    }
    else { //if the string IS NOT a bday or anniversary

        free(copy);
        return -1;
    }

}


/******** Checks if the string sent through IS begin, version or end and returns 1 if it IS, else returns -1*********/
int excludeProp (char * string) {

    if (string == NULL) {
        return 0;
    }

    char *copy = malloc(sizeof(char) * (strlen(string) + 256));
    if (copy == NULL) {
        return 6;
    }

    strcpy(copy, string);

    for(int i = 0; copy[i]; i++){
        copy[i] = tolower(copy[i]);
    }

    //is begin, version, end
    if ((strcmp(copy, "begin") == 0)
    || (strcmp(copy, "version") == 0)
    || (strcmp(copy, "end") == 0)) {
        free(copy);
        return 1;
    }
    else { //is not begin, version, end
        free(copy);
        return -1;
    } 
}

/********** Checks the file name to see if the last 3 characters are vcf (case insensitive) *****************/
int checkFileName (char *string) {

    if (string == NULL) {
        return -1;
    }

    int len = strlen(string);

    //.vcf  //.vcard
    if ((((string[len - 1] == 'f') || (string[len - 1] == 'F')) 
    && ((string[len - 2] == 'c') || (string[len - 2] == 'C'))
    && ((string[len - 3] == 'v') || (string[len - 3] == 'V'))
    && (string[len - 4] == '.')) 
    || (((string[len - 1] == 'd') || (string[len - 1] == 'D'))
    && ((string[len - 2] == 'r') || (string[len - 2] == 'R'))
    && ((string[len - 3] == 'a') || (string[len - 3] == 'A'))
    && ((string[len - 4] == 'c') || (string[len - 4] == 'C'))
    && ((string[len - 5] == 'v') || (string[len - 5] == 'V'))
    && (string[len - 6] == '.'))) {
        return 1;
    }
    else {
        return -1; //E error with not matching file extension
    }

}



/****************************************************************************/
/******************************* Assignment 2 *******************************/
/****************************************************************************/

//Module 3
//***************** For JSONtoDT: extracts the date and puts in in the structure *****************/
int fillInDate(DateTime* date, char * string) {

    if (string == NULL) {
        return -1;
    }
    if (date == NULL) {
        return -1;
    }
    if (date->date == NULL) {
        return -1;
    }
    if (date->time == NULL) {
        return -1;
    }
    if (date->text == NULL) {
        return -1;
    }

    //divide the string in two, before and after :
    char* end = strstr(string, ":");
    if (end != NULL) {

        int index = (int)(end - string);

        int length = 0;
        length = (strlen(string)) + 256;
        
        //first substring
        char first[length];
        memset(first, '\0', length);
        strncpy(first, (string), (index));
        first[strlen(first)] = '\0';

    

        //second substring
        char second[length];
        memset(second, '\0', length);
        strncpy(second, (string + index + 1), (length - index));
        second[strlen(second)] = '\0';

    

        //isText
        if (caseInsensitiveCmp (first, "\"istext\"") == 1) { //if first IS isText (made it all lower case cause of caseInsensitivity)

            if (caseInsensitiveCmp (second, "true") == 1) { //if it is true
                date->isText = true;
            }
            else if (caseInsensitiveCmp (second, "false") == 1) {
                date->isText = false;
            }
            else {
                return -1; //isText can only be true or false, anything else is invalid
            }

        }

        //Date
        if (caseInsensitiveCmp (first, "\"date\"") == 1) {

            //remove "" from second (IF ANYTHING GOES WRONG CHECK THIS)
            char newSecond[length];
            memset(newSecond, '\0', length);
            strncpy(newSecond, (second + 1), (strlen(second) - 2));
         

            //add that to the property structure
            if (strcmp(newSecond, "") == 0) {
                strcpy(date->date, "");
            }
            else {
                strcpy(date->date, newSecond);
            }
            

        }

        //Time
        if (caseInsensitiveCmp (first, "\"time\"") == 1) {

            //remove "" from second (IF ANYTHING GOES WRONG CHECK THIS)
            char newSecond[length];
            memset(newSecond, '\0', length);
            strncpy(newSecond, (second + 1), (strlen(second) - 2));
         

            //add that to the property structure
            if (strcmp(newSecond, "") == 0) {
                strcpy(date->time, "");
            }
            else {
                strcpy(date->time, newSecond);
            }
            
        }

        //Text
        if (caseInsensitiveCmp (first, "\"text\"") == 1) {

            //remove "" from second (IF ANYTHING GOES WRONG CHECK THIS)
            char newSecond[length];
            memset(newSecond, '\0', length);
            strncpy(newSecond, (second + 1), (strlen(second) - 2));
          

            //add that to the property structure
            if (strcmp(newSecond, "") == 0) {
                strcpy(date->text, "");
            }
            else {
                strcpy(date->text, newSecond);
            }

        }

        //isUTC
        if (caseInsensitiveCmp (first, "\"isutc\"") == 1) { //if first IS isUTC (made it all lower case cause of caseInsensitivity)

            if (caseInsensitiveCmp (second, "true") == 1) { //if it is true
                date->UTC = true;
            }
            else if (caseInsensitiveCmp (second, "false") == 1) {
                date->UTC = false;
            }
            else {
                return -1; //isText can only be true or false, anything else is invalid
            }

        }

    }
    else { //does not have :
        return -1;
    }


    return 1;

}



//***************** For JSONtoProp: extracts the name,group,values list and puts in in the structure *****************/
int fillInProp(Property* prop, char * string) {

    if (prop == NULL) {
        return -1;
    }

    if (string == NULL) {
        return -1;
    }

    if (prop->name == NULL) {
        return -1;
    }

    if (prop->group == NULL) {
        return -1;
    }

    if (prop->values == NULL) {
        return -1;
    }

    //divide the string in two, before and after :
    char* end = strstr(string, ":");
    if (end != NULL) {

        int index = (int)(end - string);

        int length = 0;
        length = (strlen(string)) + 256;
        
        //first substring
        char first[length];
        memset(first, '\0', length);
        strncpy(first, (string), (index));
        first[strlen(first)] = '\0';

        

        //second substring
        char second[length];
        memset(second, '\0', length);
        strncpy(second, (string + index + 1), (length - index));
        second[strlen(second)] = '\0';

        

        if (caseInsensitiveCmp (first, "\"name\"") == 1) { //if first IS name

            //remove "" from second (IF ANYTHING GOES WRONG CHECK THIS)
            char newSecond[length];
            memset(newSecond, '\0', length);
            strncpy(newSecond, (second + 1), (strlen(second) - 2));
           

            //add that to the property structure
            strcpy(prop->name, newSecond);
        }

        if (caseInsensitiveCmp (first, "\"group\"") == 1) { //if first IS group

            //remove "" from second (IF ANYTHING GOES WRONG CHECK THIS)
            char newSecond[length];
            memset(newSecond, '\0', length);
            strncpy(newSecond, (second + 1), (strlen(second) - 2));
            

            //add that to the property structure
            if (strcmp(newSecond, "") == 0) {
                strcpy(prop->group, "");
            }
            else {
                strcpy(prop->group, newSecond);
            }
            
        }

        if (caseInsensitiveCmp (first, "\"values\"") == 1) { //if first IS values

            
            List * list = JSONtoStrList(second);
            if (list != NULL) {
                freeList(prop->values); //have to free this because its already malloced and the list is also malloced, prop->value looses its pointer to memory
                prop->values = list;
            }
            else { //list is NULL some kind of error
                return -1;
            }

        }

    }
    else { //does not have :
        return -1;
    }

    return 1;
}


//Module 2

//checks if a kind value exists in the entire optional list
//if it does then checks it's value for the word group
//returns 1 if kind exists and kind does have the value group
//returns -1 otherwise
int checkKindProperty (List * list) {

    int counterForKind = 0;

    //traverse the optional list and check if kind exists
    ListIterator iter = createIterator(list);

    Property* elem;
	while((elem = nextElement(&iter)) != NULL) {

        char *copy = malloc(sizeof(char) * (strlen(elem->name) + 256)); //so string does not get changed
        strcpy(copy, elem->name);

        if (caseInsensitiveCmp (copy, "kind") == 1) {
            counterForKind++;
            int check = checkKindValue (elem->values);
            if (check == 1) {
                free(copy);
                return 1;
            }

        }

        free(copy);
    }

    //if there is no kind at the end of the whole optional list
    if (counterForKind == 0) {
        return -1;
    }

    return -1;
}

//Checks if inside the kind list the value is group or not
//returns 1 if group exists
//returns -1 otherwise
int checkKindValue (List * list) {

    //traverse the optional list and check if kind exists
    ListIterator iter = createIterator(list);

    char* elem;
	while((elem = nextElement(&iter)) != NULL) {

        char *copy = malloc(sizeof(char) * (strlen(elem) + 256)); //so string does not get changed
        strcpy(copy, elem);

        if (caseInsensitiveCmp (copy, "group") == 1) {
            free(copy);
            return 1;
        }

        free(copy);

    }

    return -1;

}




//NC ***************** Validates the optional Properties List **************/
VCardErrorCode validateOptionalPropertiesListDocs (List * list) {

    int kindCount = 0;
    int nCount = 0;
    int genderCount = 0;
    int prodidCount = 0;
    int revCount = 0;
    int uidCount = 0;

    ListIterator iter = createIterator(list);

    Property* elem;
	while((elem = nextElement(&iter)) != NULL) {

        //check if its a valid prop name based on Sections 6.1 to 6.9.3 (include ALL that could be in the list (e.g. bday and anniversary))
        VCardErrorCode errorValidNames = checkValidPropNames(elem);
        if (errorValidNames == INV_PROP) {
          
            return INV_PROP;
        }

        //check if begin, version or end is in here (cardinality) - INV_CARD
        VCardErrorCode errorBVE = checkBVEFun(elem);
        if (errorBVE == INV_CARD) {
           
            return INV_CARD;
        }

        //Check that BDAY and ANNIVERSARY are not in the optional property list (cardinality)
        VCardErrorCode errorDates = checkDatesInOptionalList(elem);
        if (errorDates == INV_DT) {
           
            return INV_DT;
        }
        else if (errorDates == INV_PROP) {
            return INV_PROP;
        }
        else if (errorDates == OTHER_ERROR) {
            return OTHER_ERROR;
        }

        //Check if its valid cardinality for property (cardinality)
            // *  No need for error checking
            // 1* Check if it is in the card atleast once
            // *1 If there is more than 1 in optional list/card it is INVALID
            // 1  Only 1 in entire op list/card (usually begin, end, version) -> already checked
            char *copy = malloc(sizeof(char) * (strlen(elem->name) + 256)); //so string does not get changed
            strcpy(copy, elem->name);

            if (caseInsensitiveCmp (copy, "kind") == 1) { //if property IS kind
                kindCount++;
                if (kindCount > 1) { //if this is the second kind then its invalid
                    free(copy);
                    return INV_PROP;
                }
            }

            if (caseInsensitiveCmp (copy, "n") == 1) {
                nCount++;
                if (nCount > 1) {
                    free(copy);
                    return INV_PROP;
                }
            }

            if (caseInsensitiveCmp (copy, "gender") == 1) {
                genderCount++;
                if (genderCount > 1) {
                    free(copy);
                    return INV_PROP;
                }
            }

            if (caseInsensitiveCmp (copy, "prodid") == 1) {
                prodidCount++;
                if (prodidCount > 1) {
                    free(copy);
                    return INV_PROP;
                }
            }

            if (caseInsensitiveCmp (copy, "rev") == 1) {
                revCount++;
                if (revCount > 1) {
                    free(copy);
                    return INV_PROP;
                }
            }

            if (caseInsensitiveCmp (copy, "uid") == 1) {
                uidCount++;
                if (uidCount > 1) {
                   
                    free(copy);
                    return INV_PROP;
                }
            }

            //Special Case: Member
            //MUST NOT be present unless the value of the KIND property is "group"
            //*
            if (caseInsensitiveCmp (copy, "member") == 1) {

                int checkMemberOcc = checkKindProperty(list);
                if (checkMemberOcc != 1) { //if kind does not have the value group or does not exist
                   
                    free(copy);
                    return INV_PROP;
                }
            }


            free(copy);

        //Check if its valid cardinality for values
        VCardErrorCode valueCardinality = checkValueCardinality(elem);
        if (valueCardinality == INV_PROP) {
           
            return INV_PROP;
        }

        //Check if the values list contains any NULL items
        VCardErrorCode valueListError = validateValuesList(elem->values);
        if (valueListError == INV_PROP) {
           
            return INV_PROP;
        }

        //If it has parameters, check that the parameter names are not empty string
        //If it has parameters, check that the parameter values are not empty string
        if (elem->parameters == NULL) {
            return INV_PROP;
        }

        if (getLength(elem->parameters) > 0) { //if the parameter list has items in it then validate the parameter struct
            VCardErrorCode paramError = validateParameterStructure(elem->parameters);
            if (paramError == INV_PROP) {
            
                return INV_PROP;
            }
        }

        
    }


    return OK;
}

//***************** Checks that none of the items inside the Values list is NULL *****************/
VCardErrorCode validateValuesList(List * list) {

    if (list == NULL) {
        return INV_PROP;
    }

    int length = 0;
    length = getLength(list);
    if (length < 1) {
        return INV_PROP;
    }

    int count = 0;

    ListIterator iter = createIterator(list);

    char* elem;
	while((elem = nextElement(&iter)) != NULL) {
        count++;
        

        if (elem == NULL) {
            return INV_PROP;
        }

    }

    
    if (count != length) { //ANYTHIGN GOES WRONG CHECK HERE
        return INV_PROP;
    }

    return OK;

}


//***************** Validates the cardinality of Value list *****************/
//Fill in TEL
//Fix all the values that are URI to whatever the prof says
VCardErrorCode checkValueCardinality (Property * prop) {

    if (prop == NULL) {
        return INV_PROP;
    }
    if (prop->values == NULL) {
        return INV_PROP;
    }
    if (prop->name == NULL) {
        return INV_PROP;
    }

    char *copyForVal = malloc(sizeof(char) * (strlen(prop->name) + 256)); //so string does not get changed
    strcpy(copyForVal, prop->name);


    //SOURCE (URI value)
    if (caseInsensitiveCmp (copyForVal, "source") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //if not 1 or greater INV
            free(copyForVal);
            return INV_PROP;
        }
    }


    //KIND (single text)
    if (caseInsensitiveCmp (copyForVal, "kind") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //XML (single text)
    if (caseInsensitiveCmp (copyForVal, "xml") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //FN (1 Fn is complusary but can have more in which case we have to verify it's value cardinality) (single text)
    if (caseInsensitiveCmp (copyForVal, "fn") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //N (5 values)
    if (caseInsensitiveCmp (copyForVal, "n") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 5) {
            free(copyForVal);
           
            return INV_PROP;
        }
    }

    //NICKNAME (1 text value separated by commas, considered 1 in our case)
    if (caseInsensitiveCmp (copyForVal, "nickname") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //PHOTO (URI value)
    if (caseInsensitiveCmp (copyForVal, "photo") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //less than 1 invalid (has to be 1 or greater)
            free(copyForVal);
            return INV_PROP;
        }
    }

    //GENDER (Can have only the sex component (M/F/"") and/or can have the gender identity nothing else)
    if (caseInsensitiveCmp (copyForVal, "gender") == 1) {
        int valLen = getLength(prop->values);
        if ((valLen != 1) && (valLen != 2)){ //max 2 min 1
            free(copyForVal);
            return INV_PROP;
        }
    }

    //ADR (MUST have 7 values)
    if (caseInsensitiveCmp (copyForVal, "adr") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 7) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //TEL (URI value, greater than or equal to 1)
    if (caseInsensitiveCmp (copyForVal, "tel") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //if it is less than 1 (i.e. 0), NOT greater than 1 and NOT equal to 1
            free(copyForVal);
            return INV_PROP;
        }
    }    


    //EMAIL (single text)
    if (caseInsensitiveCmp (copyForVal, "email") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //IMPP (URI Value)
    if (caseInsensitiveCmp (copyForVal, "impp") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //if less than 1 then invalid (has to be greater than 1 or equal to 1)
            free(copyForVal);
            return INV_PROP;
        }
    }

    //LANG (single language tag value)
    if (caseInsensitiveCmp (copyForVal, "lang") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //TZ (text/URI/utc-offset)
    if (caseInsensitiveCmp (copyForVal, "tz") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //if less than 1 then its invalid (only valid if it is greater than 1 or equal to 1)
            free(copyForVal);
            return INV_PROP;
        }
    }

    //GEO (URI value)
    if (caseInsensitiveCmp (copyForVal, "geo") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //if less than 1 then its invalid (has to be 1 or greater)
            free(copyForVal);
            return INV_PROP;
        }
    }

    //TITLE (single text)
    if (caseInsensitiveCmp (copyForVal, "title") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //ROLE (single text)
    if (caseInsensitiveCmp (copyForVal, "role") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //LOGO (URI value)
    if (caseInsensitiveCmp (copyForVal, "logo") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //if it is less than 1 it is invalid (has to be greater than or equal to 1)
            free(copyForVal);
            return INV_PROP;
        }
    }

    //ORG (MUST have 1 component, but may have one or more extra ones) (If its less than 1 item then its invalid)
    //Has to have 1 component and can have 0 or more of other components
    //!! Check this
    if (caseInsensitiveCmp (copyForVal, "org") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //if it is less than 1 then its invalid (can't have 0 values) (can have 1, 2, 3...)
            free(copyForVal);
            return INV_PROP;
        }
    }
    
    //MEMBER (URI Value)
    if (caseInsensitiveCmp (copyForVal, "member") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //can't be less than one (has to be greater than or equal to 1)
            free(copyForVal);
            return INV_PROP;
        }
    }

    //RELATED (URI Value)
    if (caseInsensitiveCmp (copyForVal, "related") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //can't be less than one (has to be greater than or equal to 1)
            free(copyForVal);
            return INV_PROP;
        }
    }

    //CATEGORIES (text value separated by comma, we consider this as 1)
    if (caseInsensitiveCmp (copyForVal, "categories") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //NOTE (single text value)
    if (caseInsensitiveCmp (copyForVal, "note") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //PRODID (single text value)
    if (caseInsensitiveCmp (copyForVal, "prodid") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //REV (single timestamp value)
    if (caseInsensitiveCmp (copyForVal, "rev") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //SOUND (URI value)
    if (caseInsensitiveCmp (copyForVal, "sound") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //has to be greater than or equal to 1 (can't be 0)
            free(copyForVal);
            return INV_PROP;
        }
    }

    //UID (URI value or free form text)
    if (caseInsensitiveCmp (copyForVal, "uid") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //CLIENTPIDMAP (1*DIGIT ";" URI) (So must have 1 or more DIGIT and must have URI (which can also have multiple semicolon) so 2 or more) //!! check this
    if (caseInsensitiveCmp (copyForVal, "clientpidmap") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 2) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //URL (URI value)
    if (caseInsensitiveCmp (copyForVal, "url") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //has to have 1, can have more (invalid if it is less than 1 (ie. 0))
            free(copyForVal);
            return INV_PROP;
        }
    }

    //KEY (URI value, can be reset to text)
    if (caseInsensitiveCmp (copyForVal, "key") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //must be 1 ore greater
            free(copyForVal);
            return INV_PROP;
        }
    }

    //FBURL (URI value)
    if (caseInsensitiveCmp (copyForVal, "fburl") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //must be 1 ore greater
            free(copyForVal);
            return INV_PROP;
        }
    }

    //CALADRURI
    if (caseInsensitiveCmp (copyForVal, "caladruri") == 1) {
        int valLen = getLength(prop->values);
        if (valLen != 1) {
            free(copyForVal);
            return INV_PROP;
        }
    }

    //CALURI (URI value)
    if (caseInsensitiveCmp (copyForVal, "caluri") == 1) {
        int valLen = getLength(prop->values);
        if (valLen < 1) { //must be 1 or greater
            free(copyForVal);
            return INV_PROP;
        }
    }

    free(copyForVal);

    return OK;
}






//***************** Validates the dates *****************/
//Need to verify all the different formats of date and time (lengths)
//No Need to verify if the dates are appropriate day, month, years (Prof's answer on forum)
//No Need to verify if the times are appropriate hours, minutes, seconds (Prof's answer on forum)
VCardErrorCode validateDates(DateTime *date) {

    if (date == NULL) {
        return INV_DT;
    }

    if (date->date == NULL) {
       
        return INV_DT;
    }

    if (date->time == NULL) {
     
        return INV_DT;
    }

    if (date->text == NULL) {
     
        return INV_DT;
    }


    //check if date is text and date is UTC : it is an error
    if (date->isText == true) {
        if (date->UTC == true) {
            return INV_DT;
        }
    }

    //If text is false or text is true but UTC is false
    //check if UTC is true that means text HAS to be false
    //ANYTHING GOES WRONG CHECK HERE FIRST (unecessary code)
    if (date->UTC == true) {
        if (date->isText == true) {
            return INV_DT;
        }
    }

    //if date is text then date and time fields should be an empty string else error
    if (date->isText == true) {

        //UTC has to be false
        if (date->UTC == true) {
            return INV_DT;
        }

        //If date is not an empty string its an error
        if (strcmp(date->date, "") != 0) {
            return INV_DT;
        }
        //If time is not an empty string its an error
        if (strcmp(date->time, "") != 0) {
            return INV_DT;
        }

        //If text is an empty string then its an error 
        if (strcmp(date->text, "") == 0) {
            return INV_DT;
        }

    }
    else { //if date is NOT text, its either (date) or (time) or (date and time)

        //if text is set as false, then text has to be an empty string else its Invalid
        if (strcmp(date->text, "") != 0) {
            return INV_DT;
        }

        //if time is an empty string then date can't be
        //date only
        if (strcmp(date->time, "") == 0) {
            if (strcmp(date->date, "") == 0) {
                return INV_DT;
            }
            //if time is not there but UTC is set to true then its invalid
            if (date->UTC == true) {
                return INV_DT;
            }

            //check if date has a valid format
            VCardErrorCode dateOnly = validateDateFormat(date->date);
            if (dateOnly == INV_DT) {
                return INV_DT;
            }
        }

        //if date is an empty string then time can't be
        //time only
        if (strcmp(date->date, "") == 0) {
            if (strcmp(date->time, "") == 0) {
                return INV_DT;
            }

            //Check if time has a valid format
            VCardErrorCode timeOnly = validateTimeFormat(date->time);
            if (timeOnly == INV_DT) {
                return INV_DT;
            }

        }

        //if date is NOT an empty string AND time is NOT and empty string
        //date and time
        if ((strcmp(date->date, "") != 0) && (strcmp(date->time, "") != 0)) {

            //check if date has a valid format
            VCardErrorCode dateOnly2 = validateDateFormat(date->date);
            if (dateOnly2 == INV_DT) {
                return INV_DT;
            }


            //Check if time has a valid format
            VCardErrorCode timeOnly2 = validateTimeFormat(date->time);
            if (timeOnly2 == INV_DT) {
                return INV_DT;
            }

        }
    }



    return OK;
}

/***************** Checks if the dates are the right length (Format)  *****************/
VCardErrorCode validateDateFormat (char * date) {

    if (date == NULL) {
        return INV_DT;
    }

    int length = strlen(date);

    //if the length of date is NOT any of the numbers from 4 to 8 (4,5,6,7,8) then it is an invalid format
    if ((length != 4) 
    && (length != 5)
    && (length != 6)
    && (length != 7)
    && (length != 8)) {
        return INV_DT;
    }

    return OK;

}

/***************** Checks if the time are the right length (Format)  *****************/
VCardErrorCode validateTimeFormat (char * time) {

    if (time == NULL) {
        return INV_DT;
    }

    int length = strlen(time);

    //if the length of time is not any of the numbers below (2,4,5,6,11) then it is invalid
    if ((length != 2)
    && (length != 4)
    && (length != 5)
    && (length != 6)
    && (length != 11)) {
        return INV_DT;
    }

    return OK;

}


/***************** Checks for dates inside optional list (invalid)  *****************/
VCardErrorCode checkDatesInOptionalList(Property * prop) {

    if (prop == NULL) {
        return INV_PROP; //E double check this error
    }

    char *copy = malloc(sizeof(char) * (strlen(prop->name) + 256));
    if (copy == NULL) {
        return OTHER_ERROR;
    }

    strcpy(copy, prop->name); //copied property name over so that it does not modify the card string

    //if bday or anniversary is in the optional list, then it is invalid date
    if ((caseInsensitiveCmp (copy, "bday") == 1)
    || (caseInsensitiveCmp (copy, "anniversary") == 1)) {
        free(copy);
        return INV_DT;
    }

    free(copy);
    return OK;

}


/***************** Checks for valid property names from Section 6.1 - 6.9.3  *****************/
VCardErrorCode checkValidPropNames(Property * prop) {

    if (prop == NULL) {
        return INV_PROP; //E double check this error
    }

    char *copy = malloc(sizeof(char) * (strlen(prop->name) + 256));
    if (copy == NULL) {
        return OTHER_ERROR;
    }

    strcpy(copy, prop->name); //copied property name over so that it does not modify the card string



    //if the property name is not any from the list then it is invalid so return
    //if function returns a -1 (string not same) for ALL the names from the list (&&) then its invalid
    if ((caseInsensitiveCmp (copy, "begin") == -1) 
    && (caseInsensitiveCmp (copy, "end") == -1) 
    && (caseInsensitiveCmp (copy, "source") == -1) 
    && (caseInsensitiveCmp (copy, "kind") == -1) 
    && (caseInsensitiveCmp (copy, "xml") == -1)
    && (caseInsensitiveCmp (copy, "fn") == -1)
    && (caseInsensitiveCmp (copy, "n") == -1)
    && (caseInsensitiveCmp (copy, "nickname") == -1)
    && (caseInsensitiveCmp (copy, "photo") == -1)
    && (caseInsensitiveCmp (copy, "bday") == -1) //double check this
    && (caseInsensitiveCmp (copy, "anniversary") == -1) //double check this
    && (caseInsensitiveCmp (copy, "gender") == -1)
    && (caseInsensitiveCmp (copy, "adr") == -1)
    && (caseInsensitiveCmp (copy, "tel") == -1)
    && (caseInsensitiveCmp (copy, "email") == -1) 
    && (caseInsensitiveCmp (copy, "impp") == -1)
    && (caseInsensitiveCmp (copy, "lang") == -1)
    && (caseInsensitiveCmp (copy, "tz") == -1)
    && (caseInsensitiveCmp (copy, "geo") == -1)
    && (caseInsensitiveCmp (copy, "title") == -1)
    && (caseInsensitiveCmp (copy, "role") == -1) 
    && (caseInsensitiveCmp (copy, "logo") == -1)
    && (caseInsensitiveCmp (copy, "org") == -1)
    && (caseInsensitiveCmp (copy, "member") == -1)
    && (caseInsensitiveCmp (copy, "related") == -1)
    && (caseInsensitiveCmp (copy, "categories") == -1) 
    && (caseInsensitiveCmp (copy, "note") == -1) 
    && (caseInsensitiveCmp (copy, "prodid") == -1) 
    && (caseInsensitiveCmp (copy, "rev") == -1) 
    && (caseInsensitiveCmp (copy, "sound") == -1) 
    && (caseInsensitiveCmp (copy, "uid") == -1) 
    && (caseInsensitiveCmp (copy, "clientpidmap") == -1) 
    && (caseInsensitiveCmp (copy, "url") == -1) 
    && (caseInsensitiveCmp (copy, "version") == -1) //double check this
    && (caseInsensitiveCmp (copy, "key") == -1) 
    && (caseInsensitiveCmp (copy, "fburl") == -1) 
    && (caseInsensitiveCmp (copy, "caladruri") == -1)
    && (caseInsensitiveCmp (copy, "caluri") == -1)) {


      

        free(copy);
        return INV_PROP;

    }



    free(copy);

    return OK;
}


/***************** Checks for Begin, Version and End in optional properties *****************/
VCardErrorCode checkBVEFun(Property * prop) {

    if (prop == NULL) {
        return INV_PROP; //E double check this error
    }

    char *copy = malloc(sizeof(char) * (strlen(prop->name) + 256));
    if (copy == NULL) {
        return OTHER_ERROR;
    }

    strcpy(copy, prop->name); //copied property name over so that it does not modify the card string

    if (excludeProp(copy) == 1) { //if a property is Begin Version or End
        free(copy);
        return INV_CARD;
    }

    free(copy);
    return OK;
}

/***************** Validates the FN property as per the specifications **************/
VCardErrorCode validateFNDocs(Property * fn) {

    if (fn == NULL) {
        return INV_PROP;
    }

    //Check if FN's name is actually FN 
    if (fn->name != NULL) {
        char *copy = malloc(sizeof(char) * (strlen(fn->name) + 256));
        strcpy(copy, fn->name);
        if (caseInsensitiveCmp (copy, "fn") != 1) { //If they are NOT the same
            free(copy);
            return INV_PROP;
        }
        free(copy);
    }
    else {
        return INV_PROP;
    }
    

    //Cardinality of Values
    if (fn->values != NULL) {
        
        int valLen = getLength(fn->values);
        if (valLen != 1) {
            return INV_PROP;
        }

        //Check if the values list contains any NULL items (ANYTHING GOES WRONG CHECK THIS)
        VCardErrorCode valueListError = validateValuesList(fn->values);
        if (valueListError == INV_PROP) {
            return INV_PROP;
        }
       
    }
    else {
        return INV_PROP;
    }
    

    //parameter list (may be empty, must never be NULL) Any thing goes wrong check here
    if (fn->parameters == NULL) {
        return INV_PROP;
    }

    if (getLength(fn->parameters) > 0) { //if the parameter list has items in it then validate the parameter struct
        VCardErrorCode paramError = validateParameterStructure(fn->parameters);
        if (paramError == INV_PROP) {
            return INV_PROP;
        }
    }


    return OK;
}

/***************** Validates the FN Structure **************/
VCardErrorCode validateFNStructure(Property * prop) {

    //name (must not be an empty string, must not be NULL)
    if (prop->name == NULL) {
     
        return INV_PROP;
    }

    if (strcmp(prop->name, "") == 0) {
      
        return INV_PROP;
    }


    //group (may be an empty string, must not be NULL)

    if (prop->group == NULL) {
     
        return INV_PROP;
    }

    //parameter list (may be empty, must never be NULL)
    if (prop->parameters == NULL) {
     
        return INV_PROP;
    }

    if (getLength(prop->parameters) > 0) { //if the parameter list has items in it then validate the parameter struct
        VCardErrorCode paramError = validateParameterStructure(prop->parameters);
        if (paramError == INV_PROP) {
         
            return INV_PROP;
        }
    }


    //values list (must have atleast one value in it, list must never be NULL)
    if (prop->values == NULL) {
      
        return INV_PROP;
    }

    if (getLength(prop->values) < 1) { //List must have atleast one value in it (FN should have only 1, that is being tested somewhere else)
      
        return INV_PROP;
    }

    //Check if the values list contains any NULL items (ANYTHING GOES WRONG CHECK THIS)
    VCardErrorCode valueListError = validateValuesList(prop->values);
    if (valueListError == INV_PROP) {
      
        return INV_PROP;
    }


    return OK;
}


/***************** Validates the optional properties list Structure *****************/
VCardErrorCode validateOptionalPropertiesStructure(List * list) {

    if (list == NULL) {
        return INV_PROP;
    }

    ListIterator iter = createIterator(list);

    Property* elem;
	while((elem = nextElement(&iter)) != NULL) {

        //Name
        if (elem->name == NULL) {
          
            return INV_PROP;
        }

        if (strcmp(elem->name, "") == 0) {
           
            return INV_PROP;
        }

        //Group
        if (elem->group == NULL) {
          
            return INV_PROP;
        }

        //Parameter List
        if (elem->parameters == NULL) { 
          
            return INV_PROP;
        }

        //check parameter struct
        if (getLength(elem->parameters) > 0) { //if the parameter list has items in it then validate the parameter struct
            VCardErrorCode paramError = validateParameterStructure(elem->parameters);
            if (paramError == INV_PROP) {
             
                return INV_PROP;
            }
        }
        

        //Value List
        if (elem->values == NULL) {
           
            return INV_PROP;
        }

        if (getLength(elem->values) < 1) { //List must have atleast one value in it
          
            return INV_PROP;
        }

        //Check if the values list contains any NULL items (ANYTHING GOES WRONG CHECK THIS)
        VCardErrorCode valueListError = validateValuesList(elem->values);
        if (valueListError == INV_PROP) {
          
            return INV_PROP;
        }
    }
    
    
    return OK;

}

/***************** Validates the parameter Structure *****************/
//Also checks if parameter name and value are not empty string
VCardErrorCode validateParameterStructure(List * list) {

    if (list == NULL) {
        return INV_PROP;
    }

    // int count = 0;
    ListIterator iter = createIterator(list);

    Parameter* elem;
	while((elem = nextElement(&iter)) != NULL) {

       

        if (elem == NULL) {
            return INV_PROP;
        }

        //Parameter Name
        if (elem->name == NULL) { //must not be NULL
            return INV_PROP;
        } 

        if (strcmp(elem->name, "") == 0) { //must not be an empty string
            return INV_PROP;
        }

        //Parameter Description
        if (elem->value == NULL) { //must not be NULL
            return INV_PROP;
        } 

        if (strcmp(elem->value, "") == 0) { //must not be an empty string
            return INV_PROP;
        }


    }

    
    return OK;

}





//Module 1

/***************** Converts a property to a valid content line *****************/
char* propertyToContentLine(void* prop) {

    char* returnString = malloc(sizeof(char) * 256); 


    if (!prop) {
        strcpy(returnString, "NULL");
        return returnString;
    }

    Property* property = (Property *)prop; //no need to free this in this function

    int len = 0;

    //Join Name and Group if any
    if ((property->group == NULL) || (property->name == NULL) || (strcmp(property->name, "") == 0)) {
        strcpy(returnString, "NULL");
        return returnString;        
    }
    else {
        if (strcmp(property->group, "") != 0) { //there is a group
            strcpy(returnString, property->group);
            strcat(returnString, ".");
            strcat(returnString, property->name);
        }
        else { //there is NOT a group
            strcpy(returnString, property->name);
        }
    }

    //add the list of parameters to the string
    if (property->parameters == NULL) {
        strcpy(returnString, "NULL");
        return returnString;
    }

    if (getLength(property->parameters) > 0) {

        ListIterator iter = createIterator(property->parameters);
        Property* elem;
        while((elem = nextElement(&iter)) != NULL){

            char* currDescr = parameterToContentLine(elem);
            if (strcmp(currDescr, "NULL") == 0) {
                free(currDescr);
                strcpy(returnString, "NULL");
                return returnString;
            }

            strcat(returnString, ";");
            int paramLen = strlen(currDescr);
            currDescr[paramLen - 1] = '\0';

            len = strlen(currDescr) + strlen(returnString) + 256;
            returnString = (char *)realloc(returnString, len);
            strcat(returnString, currDescr);
            
            free(currDescr);
        }
    }


    //add the list of values to the string
    if (property->values == NULL) {
        strcpy(returnString, "NULL");
        return returnString;
    }

    if (getLength(property->values) > 0) { //enters if the property values list is more than 0

        strcat(returnString, ":");
        char *valueString = calloc(256, sizeof(char));
        strcpy(valueString, "");

        ListIterator iter2 = createIterator(property->values);
        Property* elem2;
        while((elem2 = nextElement(&iter2)) != NULL) {

            char* string = valueToContentLine(elem2);
            if (strcmp(string, "NULL") == 0) {
                free(string);
                strcpy(returnString, "NULL");
                return returnString;
            }

            string[strlen(string)] = '\0';

            int lengthOfOneValue = strlen(string) + 256;

            valueString = (char *)realloc(valueString, lengthOfOneValue);

            strcat(valueString, string);

            free(string);

        }

        //If after the whole loop the value string (a concatenation of all the values in the list) is NOT ""
        if (strcmp(valueString, "") != 0) { //proceed as planned
            
            int valLen = strlen(valueString);
            valueString[valLen - 1] = '\r';
            valueString[valLen] = '\n';
            valueString[valLen + 1] = '\0';

            len = strlen(valueString) + strlen(returnString) + 256;
            returnString = (char *)realloc(returnString, len);
            strcat(returnString, valueString);
            free(valueString);
            
        }
        else { //after the whole loop the value string is "" 
        //it is inside here because the list said there is atleast one value in here
        //But after the loop its still an empty string 
        //that means there is 0 values in here (but the length says more than 0) or the first item is null in the list 

            //this flags it as an error
            free(valueString);
            strcpy(returnString, "NULL");
            return returnString;

        }

    }
    else { //if the property values list is not more than 0 that means no values: invalid
        strcpy(returnString, "NULL");
        return returnString;
    }

    return returnString;
}

/***************** Converts a parameter to a valid content line *****************/
char* parameterToContentLine(void* param) {

    Parameter* parameter = (Parameter *)param;

    if (param == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }

    if (parameter->name == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }

    if (strcmp(parameter->name, "") == 0) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }

    if (parameter->value == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString; 
    }

    if (strcmp(parameter->value, "") == 0) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }

    
    int len = strlen(parameter->name) + strlen(parameter->value) + 256;
    char * returnString = malloc(sizeof(char) * len);


    sprintf(returnString, "%s=%s;", parameter->name, parameter->value);



    return returnString;
}

/***************** Converts a value to a valid content line *****************/
char* valueToContentLine(void* val) {
    
    if (((char*)val) == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }
    else {

        char * returnString = malloc(sizeof(char) * (strlen((char *)val) + 256));

        strcpy(returnString, (char *)val);
        strcat(returnString, ";");

        return returnString;
    }


}

/***************** Converts a date to a valid content line *****************/
char* dateToContentLine(void* date) {

    if (date == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }
    if (((DateTime *)date)->date == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }
    if (((DateTime *)date)->time == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }
    if (((DateTime *)date)->text == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }

    //If all 3 are empty then its invalid
    if ((strcmp(((DateTime *)date)->text, "") == 0) 
    && (strcmp(((DateTime *)date)->time, "") == 0) 
    && (strcmp(((DateTime *)date)->date, "") == 0)) {
       char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString; 
    }

    int len = 0;
    char* returnString = malloc(sizeof(char) * 256);
    strcpy(returnString, "");


    //check if its text or not and form the date/time stamp
    if (((DateTime *)date)->isText == true) { //isText = true

        //if text is empty string and text = true then it is invalid
        if (strcmp(((DateTime *)date)->text, "") == 0) {
            strcpy(returnString, "NULL");
            return returnString;
        }

        //if text is true then date and time are not empty strings invalid
        if (strcmp(((DateTime *)date)->date, "") != 0) {
            strcpy(returnString, "NULL");
            return returnString;
        }

        if (strcmp(((DateTime *)date)->time, "") != 0) {
            strcpy(returnString, "NULL");
            return returnString;
        }

        char *construct = malloc(sizeof(char) * (strlen(((DateTime *)date)->text) + 256));
        sprintf(construct, ";VALUE=text:%s\r\n", ((DateTime *)date)->text);
        construct[strlen(construct)] = '\0';

        len = strlen(returnString) + strlen(construct) + 256;
        returnString = (char *)realloc(returnString, len);
        strcat(returnString, construct);

        free(construct);

    }
    else { //isText = false

        //if text = false and is NOT an empty string then it is invalid
        if (strcmp(((DateTime *)date)->text, "") != 0) {
            strcpy(returnString, "NULL");
            return returnString;
        }

        //check if it has a time or a date and form the date/time stamp
        if (strcmp(((DateTime *)date)->time, "") == 0) { //does not have time. only date

            char *constructStamp = malloc(sizeof(char) * (strlen(((DateTime *)date)->date) + 256));

            sprintf(constructStamp, ":%s\r\n", ((DateTime *)date)->date);
            constructStamp[strlen(constructStamp)] = '\0';

            len = strlen(returnString) + strlen(constructStamp) + 256;
            returnString = (char *)realloc(returnString, len);
            strcat(returnString, constructStamp);

            free(constructStamp);

        }
        else if (strcmp(((DateTime *)date)->date, "") == 0) { //does not have date, only time

            char *constructStamp = malloc(sizeof(char) * (strlen(((DateTime *)date)->time) + 256));

            if (((DateTime *)date)->UTC == true) {
                sprintf(constructStamp, ":T%sZ\r\n", ((DateTime *)date)->time);
                constructStamp[strlen(constructStamp)] = '\0';

                len = strlen(returnString) + strlen(constructStamp) + 256;
                returnString = (char *)realloc(returnString, len);
                strcat(returnString, constructStamp);

                free(constructStamp);
            }
            else {
                sprintf(constructStamp, ":T%s\r\n", ((DateTime *)date)->time);
                constructStamp[strlen(constructStamp)] = '\0';

                len = strlen(returnString) + strlen(constructStamp) + 256;
                returnString = (char *)realloc(returnString, len);
                strcat(returnString, constructStamp);

                free(constructStamp);
            }

        }
        else { //has both date and time

            char *constructStamp = malloc(sizeof(char) * (strlen(((DateTime *)date)->time) + strlen(((DateTime *)date)->date) + 256));
            if (((DateTime *)date)->UTC == true) {
                sprintf(constructStamp, ":%sT%sZ\r\n", ((DateTime *)date)->date, ((DateTime *)date)->time);
                constructStamp[strlen(constructStamp)] = '\0';

                len = strlen(returnString) + strlen(constructStamp) + 256;
                returnString = (char *)realloc(returnString, len);
                strcat(returnString, constructStamp);

                free(constructStamp);
            }
            else {
                sprintf(constructStamp, ":%sT%s\r\n", ((DateTime *)date)->date, ((DateTime *)date)->time);
                constructStamp[strlen(constructStamp)] = '\0';

                len = strlen(returnString) + strlen(constructStamp) + 256;
                returnString = (char *)realloc(returnString, len);
                strcat(returnString, constructStamp);

                free(constructStamp);
            }
        }
    }

    return returnString;
}


/***************** Checks the file name to see if the last 3 characters are vcf (case insensitive) *****************/
int checkFileNameWrite (const char* string) {

    if (string == NULL) {
        return -1;
    }

    int len = strlen(string);

    //.vcf  //.vcard
    if ((((string[len - 1] == 'f') || (string[len - 1] == 'F')) 
    && ((string[len - 2] == 'c') || (string[len - 2] == 'C'))
    && ((string[len - 3] == 'v') || (string[len - 3] == 'V'))
    && (string[len - 4] == '.')) 
    || (((string[len - 1] == 'd') || (string[len - 1] == 'D'))
    && ((string[len - 2] == 'r') || (string[len - 2] == 'R'))
    && ((string[len - 3] == 'a') || (string[len - 3] == 'A'))
    && ((string[len - 4] == 'c') || (string[len - 4] == 'C'))
    && ((string[len - 5] == 'v') || (string[len - 5] == 'V'))
    && (string[len - 6] == '.'))) {
        return 1;
    }
    else {
        return -1; //E error with not matching file extension
    }

}


