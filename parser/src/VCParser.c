/****************************************/
/*
Name: Nareshri Babu
ID: 0956951 
Course: CIS 2750
*/
/****************************************/


#include "VCParser.h"
#include "LinkedListAPI.h"
#include "VCHelpers.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// ************* Card parser functions - MUST be implemented ***************
VCardErrorCode createCard(char* fileName, Card** newCardObject) {

    /**variable declarations**/

    //Variables for substringSearch function
    int indexSemi = 0;
    int indexColon = 0;
    char stringForContained[2000] = "";

    //variables for parsing in
    char *strBuffer = calloc(2000, sizeof(char)); //free it everywhere where u return and at the end
    char propSubString[1000] = ""; //property substring
    char bufferToCheckPropNames[1000] = "";
    char secondSubString[1000] = ""; //IF YOU CHANGE THE NUMBER ON THIS THEN MAKE SURE TO CHANGE IT IN MEMSET

    int lengthOfLine = 0;
    int numFileLines = 0; //how many total lines in the file
    int count = 0; //counts the current line in the file

    //These will keep track of how many of these are in the card if there is more than 1 then it is invalid card
    int beginCount = 0;
    int versionCount = 0;
    int fnCount = 0;
    int endCount = 0;

    //Check if filename is NULL or null character or empty string, if so then return inv_file
    if ((fileName == NULL) || (strcmp(fileName,"\0") == 0) || (strcmp(fileName, "") == 0)) {

        free(strBuffer);
		return INV_FILE;
	}

    //CHECK IF THE NEW VCARD OBJECT IS NULL OR NOT, IF IT IS NULL THEN RETURN 
    if (newCardObject == NULL) {
        
        free(strBuffer);

        return INV_CARD; //E
    } 

    //CHECK THAT IT IS A VALID VCARD FILE (see todo)
    int filenameReturn = checkFileName (fileName);
    if (filenameReturn == -1) {
        free(strBuffer);
        return INV_FILE;
    }


    
    //Check how many lines are in the entire file (for BEGIN, VERSION, and END)
    FILE *file = NULL;
    file = fopen(fileName, "r");

    //file doesn't exist return inv_file
	if (file == NULL) { 
        free(strBuffer);
		return INV_FILE;
	}

    while (fgets(strBuffer, 2000, file) != NULL) {

        numFileLines++;

    }

    fclose(file);


    //Open file
	FILE *fp = NULL;
	fp = fopen(fileName, "r");

	//file doesn't exist return inv_file
	if (fp == NULL) { 
        free(strBuffer);
		return INV_FILE;
	}

    //make a new Vcard object
    VCardErrorCode e = newVCard(newCardObject);  //E have an error check here too
    if (e != 0) {
        free(strBuffer);
        return OTHER_ERROR;
    }

    //start going through the file line by line
    while (fgets(strBuffer, 2000, fp) != NULL) {

        count++;

        lengthOfLine = strlen(strBuffer);

        //CHECK IF EACH LIST IS TERMINATED BY A \R\N
        if ((strBuffer[lengthOfLine - 2] != '\r') || (strBuffer[lengthOfLine - 1] != '\n')) { //line's last to chars are \r\n
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            
            return INV_CARD;
        }


        //Remove the \r\n from the end of the line
        char *ridOfLineEndings = calloc((lengthOfLine + 256), sizeof(char));
        if (ridOfLineEndings == NULL) {
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            return OTHER_ERROR;
        }

        strncpy(ridOfLineEndings, strBuffer, (lengthOfLine - 2)); //!! If anythign goes wrong with folding lines get rid of this
        
        strcpy(strBuffer, ridOfLineEndings);

        free(ridOfLineEndings);



        //Check for BEGIN, VERSION and END
        //Check for FN
        if (count == 1) { //BEGIN

            char *checkBVFE = malloc(sizeof(char) * (strlen(strBuffer) + 256));
            if (checkBVFE == NULL) {
                deleteCard(*newCardObject); 
                (*newCardObject) = NULL;
                fclose(fp);
                free(strBuffer);
                return OTHER_ERROR;
            }

            strcpy(checkBVFE, strBuffer);

            int n = substringSearch(checkBVFE, "begin:vcard");

            free(checkBVFE);

            if (n == -1) { //DOES NOT have begin

                deleteCard(*newCardObject); 
                (*newCardObject) = NULL;
                fclose(fp);
                free(strBuffer);
                
                return INV_CARD;
            }            
            
        }
        if (count == 2) { //VERSION

            char *checkBVFE = malloc(sizeof(char) * (strlen(strBuffer) + 256));
            if (checkBVFE == NULL) {
                deleteCard(*newCardObject); 
                (*newCardObject) = NULL;
                fclose(fp);
                free(strBuffer);
                return OTHER_ERROR;
            }

            strcpy(checkBVFE, strBuffer);

            int n = substringSearch(checkBVFE, "version:4.0");

            free(checkBVFE);

            if (n == -1) { //DOES NOT have version:4.0

                deleteCard(*newCardObject); 
                (*newCardObject) = NULL;
                fclose(fp);
                free(strBuffer);
                
                return INV_CARD;
            }

        }
        // if (count == 3) { //FN //ERROR HERE!!!! CHANGED THIS SO THAT FN DOES NOT HAVE TO BE ON LINE 3

        //     char *checkBVFE = malloc(sizeof(char) * (strlen(strBuffer) + 256));
        //     if (checkBVFE == NULL) {
        //         deleteCard(*newCardObject); 
        //         (*newCardObject) = NULL;
        //         fclose(fp);
        //         free(strBuffer);
        //         return OTHER_ERROR;
        //     }

        //     strcpy(checkBVFE, strBuffer);

        //     int n = substringSearch(checkBVFE, "fn");

        //     free(checkBVFE);

        //     if (n == -1) { //DOES NOT have fn
        //         deleteCard(*newCardObject); 
        //         (*newCardObject) = NULL;
        //         fclose(fp);
        //         free(strBuffer);
        //         
        //         return INV_CARD;
        //     }

        // }
        if (count == numFileLines) { //END

            char *checkBVFE = malloc(sizeof(char) * (strlen(strBuffer) + 256));
            if (checkBVFE == NULL) {
                deleteCard(*newCardObject); 
                (*newCardObject) = NULL;
                fclose(fp);
                free(strBuffer);
                return OTHER_ERROR;
            }

            strcpy(checkBVFE, strBuffer);

            int n = substringSearch(checkBVFE, "end:vcard");

            free(checkBVFE);

            if (n == -1) { //DOES NOT have end
                deleteCard(*newCardObject); 
                (*newCardObject) = NULL;
                fclose(fp);
                free(strBuffer);
              
                return INV_CARD;
            }

        }

        /******* Check if there are more than one BEGIN *******/
        char *checkTagsB = malloc(sizeof(char) * (strlen(strBuffer) + 256));
        if (checkTagsB == NULL) {
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            return OTHER_ERROR;
        }

        strcpy(checkTagsB, strBuffer);

        int b = substringSearch(checkTagsB, "begin");

        free(checkTagsB);

        if (b != -1) { //HAS a begin
            beginCount++;
        }

        if (beginCount > 1) { //if the number of begins is greater than 1
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
          
            return INV_CARD;
        }


        /******* Check if there are more than one VERSION *******/
        char *checkTagsV = malloc(sizeof(char) * (strlen(strBuffer) + 256));
        if (checkTagsV == NULL) {
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            return OTHER_ERROR;
        }


        strcpy(checkTagsV, strBuffer);
        int v = substringSearch(checkTagsV, "version:");
        free(checkTagsV);

        //if ((v != -1) && ((strBuffer[0] == 'V') || (strBuffer[0] == 'v'))) { //problem where if the content had version, didnt do this cause if it has groups then the first letter wont be v
        if (v != -1) { //HAS VERSION
            versionCount++;
        }

        if (versionCount > 1) {
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
           
            return INV_CARD;
        }

        /******* Check if there are more than one END *******/
        char *checkTagsE = malloc(sizeof(char) * (strlen(strBuffer) + 256));
        if (checkTagsE == NULL) {
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            return OTHER_ERROR;
        }


        strcpy(checkTagsE, strBuffer);
        int er = substringSearch(checkTagsE, "end:vcard");
        free(checkTagsE);

        if (er != -1) { //HAS VERSION
            endCount++;
        }

        if (endCount > 1) {
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            
            return INV_CARD;
        }   


        //NC Maybe put this above checking for \r\n cause folded line might not have /r/n might just have \n
        //send each line to the checkFoldedLine function (this unfolds the line if its folded, else leaves it the same)
        int foldedLineReturn = checkFoldedLine(fp, &strBuffer, &numFileLines); //changed sent in copy instead of strBuffer
        if (foldedLineReturn == -1) {
            deleteCard(*newCardObject); 
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            return INV_PROP; //E problem with folded line, it might not contain another character after a space, or it doesnt have a /r/n
        }



        //grab the index of the first ; and :
        strcpy(stringForContained, strBuffer); //copy the strbuffer into another string so that the string does not get modified
        indexSemi = substringSearch (stringForContained, ";"); //do the ; first cause every line must have a : value
        indexColon = substringSearch (stringForContained, ":");

        if (indexColon == -1) { //if the string does not contain : then its invalid
            //free all the memory
            deleteCard(*newCardObject);
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            return INV_PROP;
        }

        
        if ((indexSemi < indexColon) && (indexSemi != -1)) { //if ; is before : and exists in the string
            //fill in the first half (after groups) not including the ;
            memset(propSubString, '\0', 1000);
            strncpy(propSubString, strBuffer, (indexSemi)); 

            //fill in the second half of the substring
            memset(secondSubString, '\0', 1000);
            strncpy(secondSubString, strBuffer + indexSemi, (strlen(strBuffer) - indexSemi));

        }
        else { //if : is before ;
            //fill in the first half (after groups) not including the :
            memset(propSubString, '\0', 1000);
            strncpy(propSubString, strBuffer, (indexColon));
            

            //fill in the second half of the substring
            memset(secondSubString, '\0', 1000);
            strncpy(secondSubString, strBuffer + indexColon, (strlen(strBuffer) - indexColon));

        }

    

        //if . is in propSubString then that means it has a group (because propSubString just has the string uptil the first ; or :)
        
        //create new group variable
        char group[1000] = "";

        char *r = strstr(propSubString, ".");
        if (r != NULL) {
            int indexPeriod = (int)(r - propSubString);

            //clear buffer and copy the string before the ; and put it inside the list
            memset(group, '\0', 1000); 
            strncpy(group, propSubString, indexPeriod);

            char *holder = malloc(sizeof(char) * (strlen(propSubString) + 256));
            if (holder == NULL) {
                deleteCard(*newCardObject);
                (*newCardObject) = NULL;
                fclose(fp);
                free(strBuffer);
                return OTHER_ERROR;
            }

            char *pointer = NULL;

            strcpy(holder, propSubString);

            pointer = holder + indexPeriod + 1;

            strcpy(propSubString, pointer);

            free(holder);

        }
        else {
            strcpy(group, "");
        }


        //check if prop name is invalid
        if (strcmp(propSubString, "") == 0) {
            deleteCard(*newCardObject);
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            return INV_PROP;
        }

        //check if the second half of the substring is empty/invalid (only : or ; or empty)
        if ((strcmp(secondSubString, ":") == 0) || (strcmp(secondSubString, ";") == 0) || (strcmp(secondSubString, "") == 0)) {
            deleteCard(*newCardObject);
            (*newCardObject) = NULL;
            fclose(fp);
            free(strBuffer);
            return INV_PROP;
        }


        //copy to a buffer so the actual substring does not get changed
        memset(bufferToCheckPropNames, '\0', 1000);
        strcpy(bufferToCheckPropNames, propSubString);


        //property is FN
        if(checkFN (bufferToCheckPropNames) == 1) {

            fnCount++; //Increment the number of FNs in the card

            if (fnCount > 1) { //if there is more than one FN put it in optional properties 
                //optional properties
                int optionalReturnVal = createOptionalProperties (newCardObject, propSubString, secondSubString, group);
                if (optionalReturnVal == -1) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return INV_PROP; //E
                }
                else if (optionalReturnVal == 6) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return OTHER_ERROR; //E
                }
            }
            else { //if there is less than or equal to one FN
                //call the createFN function and send in the second half of the substring
                int FNReturnVal = createFN (newCardObject, propSubString, secondSubString, group);
                if (FNReturnVal == -1) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return INV_PROP; //E
                }
                else if (FNReturnVal == 6) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return OTHER_ERROR;
                }
            }





        }
        else { //Optional Properties

            //Get the Dates
            int checkDateReturn = checkDates (bufferToCheckPropNames);
            if (checkDateReturn != -1) { //if this is a date (bday or anniversary)

                if (checkDateReturn == 6) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return OTHER_ERROR;
                }
                else if (checkDateReturn == 0) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return OTHER_ERROR;
                }
                
                DateTime *dateReturn = createDate (newCardObject, propSubString, secondSubString);
                if (dateReturn == NULL) {

                    deleteDate(dateReturn);
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return INV_PROP; 
                }


                //if there is another BDAY then put it in the optional list
                if ((strcmp(propSubString, "BDAY") == 0) || (strcmp(propSubString, "bday") == 0) || (strcmp(propSubString, "Bday") == 0)) { //!! What if its BdAy?
                    if ((*newCardObject)->birthday == NULL) { //if nothing was in bday put the date in bday
                        (*newCardObject)->birthday = dateReturn;
                    }
                    else { //there is already a bday put in optional list and delete date
                        deleteDate(dateReturn);
                        int optionalReturnVal1 = createOptionalProperties (newCardObject, propSubString, secondSubString, group);
                        if (optionalReturnVal1 == -1) {
                            deleteCard(*newCardObject);
                            (*newCardObject) = NULL;
                            fclose(fp);
                            free(strBuffer);
                            return INV_PROP; //E
                        }
                        else if (optionalReturnVal1 == 6) {
                            deleteCard(*newCardObject);
                            (*newCardObject) = NULL;
                            fclose(fp);
                            free(strBuffer);
                            return OTHER_ERROR;
                        }
                    }
                    
                }
                else if ((strcmp(propSubString, "ANNIVERSARY") == 0) || (strcmp(propSubString, "anniversary") == 0) || (strcmp(propSubString, "Anniversary") == 0)) {
                    if ((*newCardObject)->anniversary == NULL) { //if nothing was in anniversary put the date in bday
                        (*newCardObject)->anniversary = dateReturn;
                    }
                    else {
                        deleteDate(dateReturn);
                        int optionalReturnVal2 = createOptionalProperties (newCardObject, propSubString, secondSubString, group);
                        if (optionalReturnVal2 == -1) {
                            deleteCard(*newCardObject);
                            (*newCardObject) = NULL;
                            fclose(fp);
                            free(strBuffer);
                            return INV_PROP; //E
                        }
                        else if (optionalReturnVal2 == 6) {
                            deleteCard(*newCardObject);
                            (*newCardObject) = NULL;
                            fclose(fp);
                            free(strBuffer);
                            return OTHER_ERROR; //E
                        }
                    }
                }
                else { //Not a date
                    deleteDate(dateReturn);
                }

            }
            else if (excludeProp (bufferToCheckPropNames) != 1) { //all the optional properties except BEGIN, VERSION and END (and BDAY and ANNIVERSARY) not Begin, version and end
                
                int excludeReturn = excludeProp (bufferToCheckPropNames); //!! If something goes wrong delete this
                if (excludeReturn == 0) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return OTHER_ERROR;
                }
                else if (excludeReturn == 6) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return OTHER_ERROR;
                }

                //optional properties
                int optionalReturnVal3 = createOptionalProperties (newCardObject, propSubString, secondSubString, group);
                if (optionalReturnVal3 == -1) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return INV_PROP; //E
                }
                else if (optionalReturnVal3 == 6) {
                    deleteCard(*newCardObject);
                    (*newCardObject) = NULL;
                    fclose(fp);
                    free(strBuffer);
                    return OTHER_ERROR; //E
                }

            }
            
        } 

        strBuffer = (char *)realloc(strBuffer, 2000); //if it has bee under alloced
        if (strBuffer == NULL) {
            fclose(fp);
            free(strBuffer);
            deleteCard(*newCardObject);
            (*newCardObject) = NULL;
            return OTHER_ERROR;
        }
 
    }

    if (count == 0) { //if it skipped the whole loop (e.g. empty file)
        fclose(fp);
        free(strBuffer);
        deleteCard(*newCardObject);
        (*newCardObject) = NULL;
        return INV_CARD;
    }
	
    //Check that there was an FN in the entire card
    if (fnCount < 1) { //If number of fns are less than 1 (0) then it is invalid
        fclose(fp);
        free(strBuffer);
        deleteCard(*newCardObject);
        (*newCardObject) = NULL;

        
        return INV_CARD;        
    }


    //close file once done
    fclose(fp);

    free(strBuffer);

    return OK;

}




//Prof's Function
void deleteCard(Card* obj) {

    if (!obj) { 
        return;
    }

    //frees the property FN
    deleteProperty(((Property *)((obj)->fn)));

    //frees and deletes the optional properties list
    freeList(((obj)->optionalProperties));

    //frees the DateTime birthday (after checking its NULL)
    if (((DateTime *)((obj)->birthday)) != NULL) {
        deleteDate(((DateTime *)((obj)->birthday)));
    }
    

    //frees the DateTime anniversary (after checking its NULL)
    if (((DateTime *)((obj)->anniversary)) != NULL) {
        deleteDate(((DateTime *)((obj)->anniversary)));
    }

    //frees the Card *obj, does not free the Card **obj
    free(obj);


    return;
}

//Prof's Function
char* cardToString(const Card* obj) {

    char* returnString = malloc(sizeof(char) * 256);

    if (!obj) { 
        strcpy(returnString, "NULL");
        return returnString; //E 
    }


    int newLen = 0;
	strcpy(returnString, "");

    //FN Property

    char *string = propertyToString((*obj).fn);
    newLen = strlen(returnString)+256+strlen(string);
	returnString = (char*)realloc(returnString, newLen);
	strcat(returnString, string);
    free(string);

    //List Of Optional Properties
    if ((*obj).optionalProperties != NULL) {
        char *string2 = toString((*obj).optionalProperties);
        newLen = strlen(returnString)+256+strlen(string2);
        returnString = (char*)realloc(returnString, newLen);
        strcat(returnString, string2);
        free(string2);
    }

    //Date: Birthday
    if ((*obj).birthday != NULL) {

        char *string3 = dateToString((((*obj).birthday)));
        newLen = strlen(returnString)+256+strlen(string3);
        returnString = (char*)realloc(returnString, newLen);
        strcat(returnString, string3);
        free(string3);
        
    }
    

    //Date: Anniversary 
    if ((*obj).anniversary != NULL) {

        char *string4 = dateToString((((*obj).anniversary)));
        newLen = strlen(returnString)+256+strlen(string4);
        returnString = (char*)realloc(returnString, newLen);
        strcat(returnString, string4);
        free(string4);


    }

    return returnString;

}

//Prof's Function
char* errorToString(VCardErrorCode err) {

    
    char* returnString = malloc(sizeof(char) * 256);

    if (err == OK) {
        strcpy(returnString, "OK");
    }
    else if (err == 1) {
        strcpy(returnString, "invalid file");
    }
    else if (err == 2) {
        strcpy(returnString, "invalid card");
    }
    else if (err == 3) {
        strcpy(returnString, "invalid property"); 
    }
    else if (err == 4) {
        strcpy(returnString, "invalid datetime"); 
    }
    else if (err == 5) {
        strcpy(returnString, "write error"); 
    }
    else if (err == 6) {
        strcpy(returnString, "other error"); 
    }
    else {
        strcpy(returnString, "Invalid error code");
    }


    return returnString;
}
// *************************************************************************

// ************* List helper functions - MUST be implemented *************** 

/***************************************************************/
void deleteProperty(void* toBeDeleted) {

    if (toBeDeleted == NULL) { 
        return;
    }

    free(((Property *)toBeDeleted)->name);
    //((Property *)toBeDeleted)->name = NULL;

    free(((Property *) toBeDeleted)->group);
    //((Property *) toBeDeleted)->group = NULL;

    freeList(((Property *) toBeDeleted)->parameters);
    freeList(((Property *) toBeDeleted)->values);

    free((Property *)toBeDeleted);
    //toBeDeleted = NULL;

    return;
}

int compareProperties(const void* first,const void* second) {

    // if ((first == NULL) || (second == NULL)) {
    //     return -1;
    // }

    return 0;
}

char* propertyToString(void* prop) {

    char* returnString = malloc(sizeof(char) * 256); //realloc


    if (!prop) {
        strcpy(returnString, "NULL");
        return returnString;
    }

    Property* property = (Property *)prop;
    

    char* random2 = malloc(sizeof(char) * 256);
    if (random2 == NULL) {
        strcpy(returnString, "NULL");
        return returnString;
    }

    char* random3 = malloc(sizeof(char) * 256);
    if (random3 == NULL) {
        strcpy(returnString, "NULL");
        return returnString;
    }

    int len = 0;

    //add name and gorup of the property to the string
    sprintf(returnString, "\n*****************************\n\nProperty Name: %s\nProperty Group: %s\n\nValues:\n", property->name, property->group);


    //add the list of values to the string
    char *string = toString(property->values);
    len = strlen(string) + strlen(returnString) + 256;
    returnString = (char *)realloc(returnString, len);
    strcat(returnString, string);
    free(string);

    sprintf(random2, "\nLength of Value List: %d\n", getLength(property->values));
    len = strlen(random2) + strlen(returnString) + 256;
    returnString = (char *)realloc(returnString, len);
    strcat(returnString, random2);
    free(random2);

    //add the list of parameters to the string
    strcat(returnString, "\nParameters:\n");
    char *string2 = toString(property->parameters);
    len = strlen(string2) + strlen(returnString) + 256;
    returnString = (char *)realloc(returnString, len);
    strcat(returnString, string2);
    free(string2);

    sprintf(random3, "\nLength of Param List: %d\n", getLength(property->parameters));
    len = strlen(random3) + strlen(returnString) + 256;
    returnString = (char *)realloc(returnString, len);
    strcat(returnString, random3);
    free(random3);

    strcat(returnString, "\n*****************************\n");

    return returnString;
}


/***************************************************************/
void deleteParameter(void* toBeDeleted) {

    if (toBeDeleted == NULL) { 
        return;
    }

    free(((Parameter *)toBeDeleted)->name);
    //((Parameter *)toBeDeleted)->name = NULL;

    free(((Parameter *)toBeDeleted)->value);
    //((Parameter *)toBeDeleted)->value = NULL;

    free((Parameter *)toBeDeleted);
    //toBeDeleted = NULL;

    return;
}

int compareParameters(const void* first,const void* second) {

    // if ((first == NULL) || (second == NULL)) {
    //     return -1;
    // }

    return 0;
}


char* parameterToString(void* param) {

    if (param == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }

    Parameter* parameter = (Parameter *)param;
    int len = strlen(parameter->name) + strlen(parameter->value) + 256;
    char * returnString = malloc(sizeof(char) * len);

    sprintf(returnString, "Parameter Name: %s\nParameter Value: %s\n", parameter->name, parameter->value);



    return returnString;
}


/***************************************************************/
void deleteValue(void* toBeDeleted) {

    if (((char *)toBeDeleted) == NULL) {
        return;
    }

    free((char *)toBeDeleted); 

    //toBeDeleted = NULL;

    return;
}

int compareValues(const void* first,const void* second) {

    // if ((first == NULL) || (second == NULL)) {
    //     return -1;
    // }

    return 0;
}

char* valueToString(void* val) {
    
    if (((char*)val) == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }
    else {

        char * returnString = malloc(sizeof(char) * (strlen((char *)val) + 256));

        strcpy(returnString, (char *)val);
        return returnString;
    }


}

/***************************************************************/
void deleteDate(void* toBeDeleted) {

    if (toBeDeleted == NULL) { 
        return;
    }


    free(((DateTime *)toBeDeleted)->date);
    //((DateTime *)toBeDeleted)->date = NULL;

    free(((DateTime *)toBeDeleted)->time);
    //((DateTime *)toBeDeleted)->time = NULL;

    free(((DateTime *)toBeDeleted)->text);
    //((DateTime *)toBeDeleted)->text = NULL;

    free((DateTime *)toBeDeleted);
    //toBeDeleted = NULL;

    return;
}

int compareDates(const void* first,const void* second) {

    // if ((first == NULL) || (second == NULL)) {
    //     return -1;
    // }

    return 0;
}

char* dateToString(void* date) {

    if (date == NULL) {
        char *nullString = malloc(sizeof(char) * 20);
        strcpy(nullString, "NULL");
        return nullString;
    }

    char* returnString = malloc(sizeof(char) * 256); //realloc

    int len = 0;
    //indicates whether this is UTC time
	//bool	UTC;  

    sprintf(returnString, "\n*****************************\nDATE:\n");

    if (((DateTime *)date)->UTC == true) {
        strcat(returnString, "It is UTC time\n");
    }
    else {
        strcat(returnString, "It is NOT UTC time\n");
    }

	//Indicates whether the date is a text value, e.g. "circa 1800"
    if (((DateTime *)date)->isText == true) {
        strcat(returnString, "It is text\n");
    }
    else {
        strcat(returnString, "It is NOT text\n");
    }

	//YYYYMMDD.
	//Must be an empty string if DateTime is text, or if the date portion of the date-and-or-time is unspecificed
    len = strlen(returnString) + strlen(((DateTime *)date)->date) + 256;
    returnString = (char *)realloc(returnString, len);
    strcat(returnString, "\nDate: ");
    strcat(returnString, ((DateTime *)date)->date);

	//HHMMSS.
	//Must be an empty string if DateTime is text, or if the time portion of the date-and-or-time is unspecificed
    len = strlen(returnString) + strlen(((DateTime *)date)->time) + 256;
    returnString = (char *)realloc(returnString, len);
    strcat(returnString, "\nTime: ");
    strcat(returnString, ((DateTime *)date)->time);
	
	//Text value for the DateTime. Must be an empty string if DateTime is not text
    len = strlen(returnString) + strlen(((DateTime *)date)->text) + 256;
    strcat(returnString, "\nText: ");
    strcat(returnString, ((DateTime *)date)->text);
    strcat(returnString, "\n*****************************\n");


    return returnString;
}

/****************************************************************************/
/******************************* Assignment 2 *******************************/
/****************************************************************************/

VCardErrorCode writeCard(const char* fileName, const Card* obj) {
    //Do we have to check if the obj is parsed OK or we can assume that it parsed ok
    //Do we have to check if FN is NULL or not?


    //Check if filename is NULL or null character or empty string, if so then return inv_file
    if ((fileName == NULL) || (strcmp(fileName,"\0") == 0) || (strcmp(fileName, "") == 0)) {

		return WRITE_ERROR; 
	}

    //CHECK IF THE NEW VCARD OBJECT IS NULL OR NOT, IF IT IS NULL THEN RETURN 
    if (obj == NULL) {
        
        return WRITE_ERROR; 
    } 

    //CHECK THAT IT IS A VALID VCARD FILE (see todo)
    int filenameReturn = checkFileNameWrite (fileName);
    if (filenameReturn == -1) {
        return WRITE_ERROR; 
    }

    //Open the file //!!What is the location of the file
    FILE *file = NULL;
    file = fopen(fileName, "w");

    //file doesn't exist return inv_file
	if (file == NULL) {
		return WRITE_ERROR;
	}    

    //Variables
    char *strBuffer = calloc(2000, sizeof(char));
    int length = 0;

    //Add Begin to the file
    strcpy(strBuffer, "BEGIN:VCARD");
    length = 0;
    length = strlen(strBuffer);

    strBuffer[length] = '\r';
    strBuffer[length + 1] = '\n';
    strBuffer[length + 2] = '\0';


    fwrite(strBuffer , 1, (length + 2), file);

    //Add version to the file
    strcpy(strBuffer, "VERSION:4.0");
    length = 0;
    length = strlen(strBuffer);
    strBuffer[length] = '\r';
    strBuffer[length + 1] = '\n';
    strBuffer[length + 2] = '\0';

    fwrite(strBuffer, 1, (length + 2), file);


    //Fill in FN
    if (obj->fn != NULL) {
        char *string = propertyToContentLine(obj->fn);
        if (strcmp(string, "NULL") == 0) {
            fclose(file);
            free(strBuffer);
            free(string);
            return WRITE_ERROR;
        }
        fwrite(string, 1, (strlen(string)), file);
        free(string);
    }
    else {
        fclose(file);
        free(strBuffer);
        return WRITE_ERROR;
    }
    


    //Fill in the dates
    //Birthday
    if (obj->birthday != NULL) {

        char *str = malloc(sizeof(char) * 256);
        strcpy(str, "BDAY");
        char *string2 = dateToContentLine(obj->birthday);
        if (strcmp(string2, "NULL") == 0) {
            free(str);
            free(string2);
            fclose(file);
            free(strBuffer);
            return WRITE_ERROR;
        }
        str = (char *)realloc(str, (strlen(string2) + 256));
        strcat(str, string2);

        fwrite(str, 1, (strlen(str)), file);

        free(str);
        free(string2);

    }
    
    //Anniversary
    if (obj->anniversary != NULL) {

        char *strAnn = malloc(sizeof(char) * 256);
        strcpy(strAnn, "ANNIVERSARY");
        char *string3 = dateToContentLine(obj->anniversary);
        if (strcmp(string3, "NULL") == 0) {
            free(strAnn);
            free(string3);
            fclose(file);
            free(strBuffer);
            return WRITE_ERROR;
        }
        strAnn = (char *)realloc(strAnn, (strlen(string3) + 256));
        strcat(strAnn, string3);

        fwrite(strAnn, 1, (strlen(strAnn)), file);

        free(strAnn);
        free(string3);

    }


    //Fill in optional list
    if (obj->optionalProperties == NULL) {
        fclose(file);
        free(strBuffer);
        return WRITE_ERROR;
    }


    ListIterator iter = createIterator(obj->optionalProperties);
    Property* elem;
	while((elem = nextElement(&iter)) != NULL){

		char* currDescr = propertyToContentLine(elem);
        if (strcmp(currDescr, "NULL") == 0) {
            fclose(file);
            free(currDescr);
            free(strBuffer);
            return WRITE_ERROR;
        }

        fwrite(currDescr, 1, (strlen(currDescr)), file);
		
		free(currDescr);
	}


    //Add end to the file
    strcpy(strBuffer, "END:VCARD");
    length = 0;
    length = strlen(strBuffer);
    strBuffer[length] = '\r';
    strBuffer[length + 1] = '\n';
    strBuffer[length + 2] = '\0';

    fwrite(strBuffer , 1, (length + 2), file);


    free(strBuffer);

    fclose(file);


    return OK;

}



VCardErrorCode validateCard(const Card* obj) {
    //NOTE: if you send something in insert back which is NULL then it wont actually insert it


    /***** Card Struct Validation *****/
    if (obj == NULL) { //If card obj is NULL

        return INV_CARD;
    }

    //Card Structure Verification

        //FN
        if (obj->fn == NULL) {
    
            return INV_CARD;
        }

        VCardErrorCode errorFN = validateFNStructure(obj->fn);
        if (errorFN == INV_PROP) {
        
            return INV_PROP;
        }
        else if (errorFN == INV_CARD) {
            return INV_CARD;
        }
        else if (errorFN == INV_DT) {
            return INV_DT;
        }


        //Optional properties list
        if (obj->optionalProperties == NULL) {
        
            return INV_CARD;
        }

        VCardErrorCode error = validateOptionalPropertiesStructure(obj->optionalProperties);
        if (error == INV_PROP) {
        
            return INV_PROP;
        }
        else if (error == INV_DT) {
            return INV_DT;
        }
        else if (error == INV_CARD) {
            return INV_CARD;
        }
        

        //Dates (might check it later)
        
    


    /***** Validating Card Struct against vCard Specification *****/

        //FN
        VCardErrorCode errorFNSpecs = validateFNDocs(obj->fn); 
        if (errorFNSpecs == INV_PROP) {

            return INV_PROP;
        }


        //Optional Properties

        VCardErrorCode errorOP = validateOptionalPropertiesListDocs(obj->optionalProperties);
        if (errorOP == INV_PROP) {
        
            return INV_PROP;
        }
        else if (errorOP == INV_CARD) {
        
            return INV_CARD;
        }
        else if (errorOP == INV_DT) {
        
            return INV_DT;
        }
        else if (errorOP == OTHER_ERROR) {
            
            return OTHER_ERROR;
        }
            


        //Dates: Verifying structure and from the specifications
        //Dates: BDAY 
        if (obj->birthday != NULL) {
            VCardErrorCode errorDate = validateDates(obj->birthday);
            if (errorDate == INV_DT) {
                
                return INV_DT;
            }
        }

        //Dates: Anniversary
        if (obj->anniversary != NULL) {
            VCardErrorCode errorDateA = validateDates(obj->anniversary);
            if (errorDateA == INV_DT) {
                
                return INV_DT;
            }  
        }
        



    return OK;
}

//Mod 3 Functions
char* strListToJSON(const List* strList) {

    char *returnString = malloc(sizeof(char) * 256);
    if (returnString == NULL) {
        return NULL; 
    }
    strcpy(returnString, "");

    //If list is NULL then return NULL
    if (strList == NULL) {
        free(returnString);
        return NULL;
    }

    //if list is less than 1 (empty) then return []
    if (getLength((List *)strList) < 1) {
        strcpy(returnString, "[]");
        return returnString;
    }
    
    //list has atleast 1 item in it
    strcpy(returnString, "[");
    ListIterator iter = createIterator((List *)strList);
    char* elem;
	while((elem = nextElement(&iter)) != NULL) {
      
        
        int newLen = strlen(returnString)+strlen(elem) + 256;
		returnString = (char*)realloc(returnString, newLen);

        strcat(returnString, "\"");
        strcat(returnString, elem);
        strcat(returnString, "\"");

        strcat(returnString, ",");

    }


    //remove the last comma
    returnString[strlen(returnString) - 1] = '\0';

    strcat(returnString, "]");

    return returnString;
}



List* JSONtoStrList(const char* str) {

    if (str == NULL) {
        return NULL;
    }

    if (strcmp(str, "") == 0) {
        return NULL;
    }

    //initialize new list
    void (*deleteVal)(void *) = &deleteValue;
    int (*cmpVal)(const void*, const void*) = &compareValues;
    char* (*ValToStr)(void*) = &valueToString;

    List *newList = initializeList(ValToStr, deleteVal, cmpVal);


    //Get rid of [ and ] from the string
    char *copy = malloc(sizeof(char) * (strlen(str) + 256));
    char *stringPointer = copy; //this is the pointer to the start of the original string
    
    strcpy(copy, (str + 1));
    int len = strlen(copy);
    copy[len - 1] = '\0';


    //separate each value by the ""
    int lenTray = 0;
    lenTray = (strlen(str)) + 256;
    char tray[lenTray];

    //finds the first " and calculates the index of that 
    char* end = strstr(copy, "\"");

    if(end != NULL) { //contains a " 
        
        while (end != NULL) {
            
    
            //calculate ending index of " (don't need to calculate )
            end = strstr((copy + 1), "\""); //end = strstr((copy + index + 1), "\"");
            if (end == NULL) {
                break;
            }
            int index2 = (int)(end - copy);
        
          


            //remove the quotation marks
            memset(tray, '\0', lenTray);
            strncpy(tray, (copy + 1), (index2 - 1));
            tray[strlen(tray)] = '\0';

            

            //Malloc a new string, copy string without "" and send it to the new list
            char *sendVal = malloc(sizeof(char) * (strlen(copy) + 256));
            if (sendVal == NULL) {
                copy = stringPointer;
                free(copy);
                freeList(newList);
                return NULL;
            }

            if (strcmp(tray, "") == 0) {
                
                strcpy(sendVal, "");
            }
            else {
                strcpy(sendVal, tray);
            }
            
            insertBack(newList, sendVal);



            //iterate copy so that it now excludes the word sent into the list and the ,
            int lenOfCopy = strlen(copy);

            if (lenOfCopy != index2 + 1) {
                copy = copy + index2 + 2;
            }
            else {
                break;
            }
 
        }

    }
    else { //does not contain a " so no values in list?
        copy = stringPointer;
        free(copy);
        freeList(newList);
        return NULL;
    }

    copy = stringPointer;
    free(copy);
    
    return newList;
}

char* propToJSON(const Property* prop) {

    char *returnString = malloc(sizeof(char) * 256);
    if (returnString == NULL) {
        return NULL; 
    }
    strcpy(returnString, "");

    if (prop == NULL) {
        return returnString;
    }

    strcpy(returnString, "{");

    //group
    if (prop->group != NULL) {
        int newLen = strlen(returnString)+strlen(prop->group) + 256;
        returnString = (char*)realloc(returnString, newLen);

        strcat(returnString, "\"");
        strcat(returnString, "group");
        strcat(returnString, "\"");

        strcat(returnString, ":");

        //if group is an empty string
        if (strcmp(prop->group, "") == 0) {
            strcat(returnString, "\"");
            strcat(returnString, "\"");
        }
        else { //group has an actual string
            strcat(returnString, "\"");
            strcat(returnString, prop->group);
            strcat(returnString, "\"");
        }
    }
    else { //group is NULL
        strcpy(returnString, "");
        return returnString;
    }

    strcat(returnString, ",");

    /*********** name **************/
    if (prop->name == NULL) {
        strcpy(returnString, "");
        return returnString;
    }
    if (strcmp(prop->name, "") == 0) {
        strcpy(returnString, "");
        return returnString;
    }

    int newLen2 = strlen(returnString)+strlen(prop->name) + 256;
    returnString = (char*)realloc(returnString, newLen2);

    strcat(returnString, "\"");
    strcat(returnString, "name");
    strcat(returnString, "\"");

    strcat(returnString, ":");

    strcat(returnString, "\"");
    strcat(returnString, prop->name);
    strcat(returnString, "\"");


    strcat(returnString, ",");


    //values List

    //if the list is null
    if (prop->values == NULL) {
        strcpy(returnString, "");
        return returnString;
    }

    //if the length is less than one, empty list
    if (getLength(prop->values) < 1) {
        strcpy(returnString, "");
        return returnString;
    }

    //get the values list string from strListToJSON function
    char *valuesString = strListToJSON(prop->values);

    int newLen3 = strlen(returnString)+strlen(valuesString) + 256;
    returnString = (char*)realloc(returnString, newLen3);

    strcat(returnString, "\"");
    strcat(returnString, "values");
    strcat(returnString, "\"");

    strcat(returnString, ":");

    strcat(returnString, valuesString);

    free(valuesString);

    strcat(returnString, "}");


    return returnString;
}

Property* JSONtoProp(const char* str) {

    if (str == NULL) {
        return NULL;
    }

    if (strcmp(str, "") == 0) {
        return NULL;
    }

    //Initialize property
    Property* prop = newProperty();

    //remove the {}
    char *copy = malloc(sizeof(char) * (strlen(str) + 256));
    if (copy == NULL) {
        deleteProperty(prop);
        return NULL;
    }
    char *stringPointer = copy; //this is the pointer to the start of the original string
    
    strcpy(copy, (str + 1));
    int len = strlen(copy);
    copy[len - 1] = '\0';    




    //create a tray that is going to be used to pass in strings to the prop struct
    int lenTray = 0;
    lenTray = (strlen(str)) + 256;
    char tray[lenTray];


    //finds the first , and calculates the index of that 
    char* end = strstr(copy, ",");

    if(end != NULL) { //contains a ,
        
        while (end != NULL) {

            //find the index of [
            char *squareBracket = strstr(copy, "[");
            if (squareBracket == NULL) {
                deleteProperty(prop);
       
                copy = stringPointer;
                free(copy);
                return NULL; //double check this
            }
            int squareIndex = (int)(squareBracket - copy);
            
            //Find the index of the next ,
            char* end = strstr(copy, ",");
            if (end == NULL) {
                break;
            }
            int index = (int)(end - copy);


           

            if (squareIndex > index) { //if the comma is not inside the [ then extract string

                //clear tray and grab the string from the start to ,
                memset(tray, '\0', lenTray);
                strncpy(tray, (copy), (index));
                tray[strlen(tray)] = '\0';

                
              

                //name //values //group
                int returnNumber = fillInProp(prop, tray);
                if (returnNumber == -1) {
                    deleteProperty(prop);
                 
                    copy = stringPointer;
                    free(copy);
                    return NULL; //something is null or mistake in the JSON string
                }


                copy = copy + index + 1;
            }
            else {
                int returnNumber = fillInProp(prop, copy);
                if (returnNumber == -1) {
                    deleteProperty(prop);
                
                    copy = stringPointer;
                    free(copy);
                    return NULL; //something is null or mistake in the JSON string
                }
               
                break;
            }
            
           
            
        }

    }
    else { //does not contain a ,
      
        deleteProperty(prop);
        copy = stringPointer;
        free(copy);
        return NULL;
    }



    //parameters list must always be empty but not NULL
    if (prop->parameters == NULL) {
        deleteProperty(prop);
        copy = stringPointer;
        free(copy);
        return NULL; 
    }

    //If it is not multiple values then it won't trigger the else and will quit the while loop once it doesn't find any more ,
    //Values list is empty
    if (getLength(prop->values) == 0) {
        int returnNumber = fillInProp(prop, copy);
        if (returnNumber == -1) {
            deleteProperty(prop);
            copy = stringPointer;
            free(copy);
            return NULL; //something is null or mistake in the JSON string
        }

    }
    

    copy = stringPointer;
    free(copy);

    return prop;
}

char* dtToJSON(const DateTime* prop) {

    char *returnString = malloc(sizeof(char) * 256);
    if (returnString == NULL) {
        return NULL;
    }
    strcpy(returnString, "");

    if (prop == NULL) {
        return returnString;
    }

    if (prop->date == NULL) {
        return returnString;
    }

    if (prop->time == NULL) {
        return returnString;
    }

    if (prop->text == NULL) {
        return returnString;
    }



    strcpy(returnString, "{");


    /************ isText **************/
    strcat(returnString, "\"");
    strcat(returnString, "isText");
    strcat(returnString, "\"");

    strcat(returnString, ":");


    if (prop->isText == true) {
        strcat(returnString, "true");  
    }
    else {
        strcat(returnString, "false");
    }


    strcat(returnString, ",");

    /************ DATE **************/
    int newLen = strlen(returnString)+strlen(prop->date) + 256;
    returnString = (char*)realloc(returnString, newLen);

    strcat(returnString, "\"");
    strcat(returnString, "date");
    strcat(returnString, "\"");

    strcat(returnString, ":");

    //if date is an empty string
    if (strcmp(prop->date, "") == 0) {
        strcat(returnString, "\"");
        strcat(returnString, "\"");
    }
    else {
        strcat(returnString, "\"");
        strcat(returnString, prop->date);
        strcat(returnString, "\"");
    }

    strcat(returnString, ",");

    /************ TIME **************/
    int newLen1 = strlen(returnString)+strlen(prop->time) + 256;
    returnString = (char*)realloc(returnString, newLen1);

    strcat(returnString, "\"");
    strcat(returnString, "time");
    strcat(returnString, "\"");

    strcat(returnString, ":");

    //if time is an empty string
    if (strcmp(prop->time, "") == 0) {
        strcat(returnString, "\"");
        strcat(returnString, "\"");
    }
    else {
        strcat(returnString, "\"");
        strcat(returnString, prop->time);
        strcat(returnString, "\"");
    }


    strcat(returnString, ",");

    /************ TEXT **************/
    int newLen2 = strlen(returnString)+strlen(prop->text) + 256;
    returnString = (char*)realloc(returnString, newLen2);

    strcat(returnString, "\"");
    strcat(returnString, "text");
    strcat(returnString, "\"");

    strcat(returnString, ":");

    //if text is an empty string
    if (strcmp(prop->text, "") == 0) {
        strcat(returnString, "\"");
        strcat(returnString, "\"");
    }
    else {
        strcat(returnString, "\"");
        strcat(returnString, prop->text);
        strcat(returnString, "\"");
    }

    strcat(returnString, ",");

    /************ isUTC **************/
    strcat(returnString, "\"");
    strcat(returnString, "isUTC");
    strcat(returnString, "\"");

    strcat(returnString, ":");

    if (prop->UTC == true) {
        strcat(returnString, "true");  
    }
    else {
        strcat(returnString, "false");
    }


    strcat(returnString, "}");



    return returnString;
}




DateTime* JSONtoDT(const char* str) {
//check if there is a comma in the JSON string in the text field if the code still works

    if (str == NULL) {
        return NULL;
    }

    if (strcmp(str, "") == 0) {
        return NULL;
    }

    //Initialize a new DateTime struct
    DateTime* date = newDate();
    if (date == NULL) {
        return NULL;
    }
    if (date->date == NULL) {
        deleteDate(date);
        return NULL;
    }
    if (date->time == NULL) {
        deleteDate(date);
        return NULL;
    }
    if (date->text == NULL) {
        deleteDate(date);
        return NULL;
    }

    //remove the {}
    char *copy = malloc(sizeof(char) * (strlen(str) + 256));
    if (copy == NULL) {
        deleteDate(date);
        return NULL;
    }
    char *stringPointer = copy; //this is the pointer to the start of the original string
    
    strcpy(copy, (str + 1));
    int len = strlen(copy);
    copy[len - 1] = '\0';    

    //create a tray that is going to be used to pass in strings to the prop struct
    int lenTray = 0;
    lenTray = (strlen(str)) + 256;
    char tray[lenTray];


    //finds the first , and calculates the index of that 
    char* end = strstr(copy, ",");

    if(end != NULL) { //contains a ,
        
        while (end != NULL) {

            //Find the index of ,
            end = strstr(copy, ",");
            if (end == NULL) {
                break;
            }
            int index = (int)(end - copy);

            if (copy[index + 1] == '"') { //if one character ahead there is a " only then take the substring

                //clear the tray and grab the string from the start to ,
                memset(tray, '\0', lenTray);
                strncpy(tray, (copy), (index));
                tray[strlen(tray)] = '\0';


                int dateResult = fillInDate(date, tray);
                if (dateResult == -1) {
                    copy = stringPointer;
                    free(copy);
                    deleteDate(date);
                }



                copy = copy + index + 1;

            }
            else { //there is a comma but it is not followed by a "

                //find the next comma
                end = strstr((copy + index + 1), ",");
                if (end == NULL) {
                    break;
                }
                
                int index = (int)(end - copy);
                

                //clear the tray and grab the string from the start to ,
                memset(tray, '\0', lenTray);
                strncpy(tray, (copy), (index));
                tray[strlen(tray)] = '\0';

                

                int dateResult = fillInDate(date, tray);
                if (dateResult == -1) {
                    copy = stringPointer;
                    free(copy);
                    deleteDate(date);
                }

                copy = copy + index + 1;

            }

        }

        //clear the tray and grab the string from the start to ,
        memset(tray, '\0', lenTray);
        strncpy(tray, (copy), (strlen(copy)));
        tray[strlen(tray)] = '\0';


        int dateResult = fillInDate(date, tray);
        if (dateResult == -1) {
            copy = stringPointer;
            free(copy);
            deleteDate(date);
        }

    }

    copy = stringPointer;
    free(copy);

    return date;
}

Card* JSONtoCard(const char* str) {

    if (str == NULL) {
        return NULL;
    }

    if (strcmp(str, "") == 0) {
        return NULL;
    }

    Card *obj = NULL;
    VCardErrorCode e = newVCard(&obj);
    if (e != OK) {
        return NULL;
    }

    //remove the {}
    char *copy = malloc(sizeof(char) * (strlen(str) + 256));
    if (copy == NULL) {
        deleteCard(obj);
        return NULL;
    }
    char *stringPointer = copy; //this is the pointer to the start of the original string
    
    strcpy(copy, (str + 1));
    int len = strlen(copy);
    copy[len - 1] = '\0';    



    //Separate both values from the :

    char* end = strstr(copy, ":");
    if (end != NULL) {

        int index = (int)(end - copy);

        int length = 0;
        length = (strlen(copy)) + 256;
        
        //first substring
        char first[length];
        memset(first, '\0', length);
        strncpy(first, (copy), (index));
        first[strlen(first)] = '\0';

        //copy the first substring so that it does not get changed in the caseInsensitiveCmp
        char firstCopy[length];
        strcpy(firstCopy, first);

       

        //second substring
        char second[length];
        memset(second, '\0', length);
        strncpy(second, (copy + index + 1), (length - index));
        second[strlen(second)] = '\0';

        

        if (caseInsensitiveCmp (firstCopy, "\"fn\"") == 1) { //if first IS FN (lower case cause of case insensitivity)

            //remove "" from first (name)
            char newFirst[length];
            memset(newFirst, '\0', length);
            strncpy(newFirst, (first + 1), (strlen(first) - 2));
         

            //add that to FN's name
            strcpy(obj->fn->name, newFirst);


            //remove "" from second (Value) (IF ANYTHING GOES WRONG CHECK THIS)
            char newSecond[length];
            memset(newSecond, '\0', length);
            strncpy(newSecond, (second + 1), (strlen(second) - 2));
           

            //need to malloc the value that is being sent in
            char * val = malloc(sizeof(char) * (strlen(newSecond) + 256));
            if (val == NULL) {
                copy = stringPointer;
                free(copy); 
                deleteCard(obj);
                obj = NULL;
                return NULL;
            }

            strcpy(val, newSecond);

            //add that to FN's values list
            insertBack(obj->fn->values, val);


            //Set group to an empty string
            strcpy(obj->fn->group, "");

        }
        else { //its not FN then its an invalid string
            copy = stringPointer;
            free(copy); 
            deleteCard(obj);
            obj = NULL;
            return NULL;

        }


    }
    else { // : does not exist invalid string
        copy = stringPointer;
        free(copy); 
        deleteCard(obj);
        obj = NULL;
        return NULL;
    }


    copy = stringPointer;
    free(copy);

    return obj;
}

void addProperty(Card* card, const Property* toBeAdded) {

    if (card == NULL) {
        return;
    }

    if (toBeAdded == NULL) {
        return;
    }

    if (card->optionalProperties == NULL) {
        return;
    }
    else {
        insertBack(card->optionalProperties, (Property *)toBeAdded);
    }


    return;
}

/****************************************************************************/
/******************************* Assignment 3 *******************************/
/****************************************************************************/

/* Gets the FN Value*/
char * getFNValue (char * fileName) {

    if (fileName == NULL) {
        return NULL;
    }

    //Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return NULL;
	}

    Card *newCardObject;

    VCardErrorCode createError = createCard(fileName, &newCardObject);
    if (createError != OK) {
        return NULL;
    }

    //Validate Card?
    VCardErrorCode validateError = validateCard(newCardObject);
    if (validateError != OK) {
        deleteCard(newCardObject);
        return NULL;
    }

    char *newStr = toString(newCardObject->fn->values);

    char *returnString = malloc(sizeof(char) * (strlen(newStr) + 256));
    strcpy(returnString, newStr);

    deleteCard(newCardObject);
    free(newStr);

    return returnString;

}

/* Gets the length of the optional properties list*/
int lengthOfOPList (char *fileName) {

    if (fileName == NULL) {
        return -1;
    }

    //Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return -1;
	}
	
	Card *newCardObject = NULL;

    VCardErrorCode createError = createCard(fileName, &newCardObject);
    if (createError != OK) {
        return -1;
    }

    //Validate Card
    VCardErrorCode validateError = validateCard(newCardObject);
     if (validateError != OK) {
		 deleteCard(newCardObject);
         return -1;
     }
     
     int len = getLength(newCardObject->optionalProperties);
     
     deleteCard(newCardObject);
     
     return len;
	
}

/* For CardView: Upon getting a fileName return back a JSON description of the whole card */
char * fileToJSON (char *fileName) {

    if (fileName == NULL) {
        return NULL;
    }

    //Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return NULL;
	}

    //create a card
    Card *obj = NULL;
    VCardErrorCode createError = createCard(fileName, &obj);
    if (createError != OK) {
        return NULL;
    }

    //verify the card struct
    VCardErrorCode validateError = validateCard(obj);
    if (validateError != OK) {
        deleteCard(obj);
        return NULL;
    }

    char *returnString = malloc(sizeof(char) * 100);
    int newLen = 0;

    //Add a [ to show start of a list of JSON objects
    strcpy(returnString, "[");
    
    //Send the FN Property to propToJSON and start a string
    char *string = propToJSON(obj->fn);
    newLen = strlen(returnString)+256+strlen(string);
	returnString = (char*)realloc(returnString, newLen);
	strcat(returnString, string);
    free(string);

    strcat(returnString, ",");

    //If not NULL send the dates to dtToJSON and concatenate it to the previous string
    if (obj->birthday != NULL) {
        char *dateStr = dtToJSON(obj->birthday);

        //remove the last } to add one more field
        dateStr[strlen(dateStr) - 1] = '\0';
        strcat(dateStr, ",\"name\":\"BDAY\"}");

        newLen = strlen(returnString)+256+strlen(dateStr);
        returnString = (char*)realloc(returnString, newLen);
        strcat(returnString, dateStr);
        free(dateStr);
        strcat(returnString, ",");
    }

    if (obj->anniversary != NULL) {
        char *dateStrAnn = dtToJSON(obj->anniversary);

        //remove the last } to add one more field
        dateStrAnn[strlen(dateStrAnn) - 1] = '\0';
        strcat(dateStrAnn, ",\"name\":\"ANNIVERSARY\"}");


        newLen = strlen(returnString)+256+strlen(dateStrAnn);
        returnString = (char*)realloc(returnString, newLen);
        strcat(returnString, dateStrAnn);
        free(dateStrAnn);
        strcat(returnString, ",");

    }

    //In a for loop send all the optional properties and concatenate it to the previous string
    ListIterator iter = createIterator(obj->optionalProperties);
    Property* elem;
	while((elem = nextElement(&iter)) != NULL){

        char *str = propToJSON(elem);
        int len = strlen(returnString)+256+strlen(str);
        returnString = (char*)realloc(returnString, len);
        strcat(returnString, str);
        free(str);
        strcat(returnString, ",");
    }

    //remove last ,
    returnString[strlen(returnString) - 1] = '\0';

    //Add a ] to show end of a list of JSON objects
    strcat(returnString, "]");

    deleteCard(obj);
    obj = NULL;
    
    //Return the string
    return returnString;
}


/* Gets the length of the parameters list for all properties inside the card*/
char* lengthOfParamList (char *fileName) {

    if (fileName == NULL) {
        return NULL;
    }


    //Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return NULL;
	}

	Card *newCardObject = NULL;

    VCardErrorCode createError = createCard(fileName, &newCardObject);
    if (createError != OK) {
        return NULL;
    }

    //Validate Card?
    VCardErrorCode validateError = validateCard(newCardObject);
     if (validateError != OK) {
		 deleteCard(newCardObject);
         return NULL;
     }
     
    char *returnString = malloc(sizeof(char) * 100);
    int newLen = 0;

    //Add a [ to show start of a list of JSON objects
    strcpy(returnString, "[");

    //Create a JSON string of numbers that corespond to the number of params 

    //FN
    int lenParam = getLength(newCardObject->fn->parameters);
    char *str = malloc(sizeof(char) * 100);
    sprintf(str, "\"%d\",", lenParam);
    newLen = strlen(returnString)+256+strlen(str);
    returnString = (char*)realloc(returnString, newLen);
    strcat(returnString, str);
    free(str);

    //Bday (if text is true then there is 1 param)
    if (newCardObject->birthday != NULL) {
        char *strBday = malloc(sizeof(char) * 100);

        if (newCardObject->birthday->isText == true) {
            sprintf(strBday, "\"1\",");
        }
        else {
            sprintf(strBday, "\"0\",");
        }
        
        newLen = strlen(returnString)+256+strlen(strBday);
        returnString = (char*)realloc(returnString, newLen);
        strcat(returnString, strBday);
        free(strBday);
    }

    //Anniversary (if text is true then there is 1 param)
    if (newCardObject->anniversary != NULL) {
        char *strAnn = malloc(sizeof(char) * 100);

        if (newCardObject->anniversary->isText == true) {
            sprintf(strAnn, "\"1\",");
        }
        else {
            sprintf(strAnn, "\"0\",");
        }

        newLen = strlen(returnString)+256+strlen(strAnn);
        returnString = (char*)realloc(returnString, newLen);
        strcat(returnString, strAnn);
        free(strAnn);
    }

    ListIterator iter = createIterator(newCardObject->optionalProperties);
    Property* elem;
	while((elem = nextElement(&iter)) != NULL){
        int lenOP = getLength(elem->parameters);
        char *strOP = malloc(sizeof(char) * 100);
        sprintf(strOP, "\"%d\",", lenOP);
        int length = strlen(returnString)+256+strlen(strOP);
        returnString = (char*)realloc(returnString, length);
        strcat(returnString, strOP);
        free(strOP);
    }


    //remove last ,
    returnString[strlen(returnString) - 1] = '\0';

    //Add a ] to show end of a list of JSON objects
    strcat(returnString, "]");
     
    deleteCard(newCardObject);
     
    return returnString;
}

//Takes in a filename and sends back a JSON object string of all the parameters, if it is 0 params its an empty object
char * fileToParamToJSON (char *fileName) {

    if (fileName == NULL) {
        return NULL;
    }

    //Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return NULL;
	}

	Card *newCardObject = NULL;

    VCardErrorCode createError = createCard(fileName, &newCardObject);
    if (createError != OK) {
        return NULL;
    }

    //Validate Card?
    VCardErrorCode validateError = validateCard(newCardObject);
     if (validateError != OK) {
		 deleteCard(newCardObject);
         return NULL;
     }
     
    char *returnString = malloc(sizeof(char) * 100);
    int newLen = 0;

    
    //Add a [ to show start of a list of JSON objects
    strcpy(returnString, "[");

    //FN
    char *functionString = paramToJSON(newCardObject->fn);

    newLen = strlen(returnString)+256+strlen(functionString);
    returnString = (char*)realloc(returnString, newLen);

    strcat(returnString, functionString);
    strcat(returnString, ",");

    free(functionString);

    //BDAY

    if (newCardObject->birthday != NULL) {
        if (newCardObject->birthday->isText == true) {

            char *dateStrB = malloc(sizeof(char) * 256);
            //{"params":["TYPE", "work", "PREF", "1"]}
            sprintf(dateStrB, "{\"params\":[\"VALUE\", \"text\"]},");

            newLen = strlen(returnString)+256+strlen(dateStrB);
            returnString = (char*)realloc(returnString, newLen);
            strcat(returnString, dateStrB);

            free(dateStrB);

        }
        else {
            char *dateStrB = malloc(sizeof(char) * 256);
            sprintf(dateStrB, "{\"params\":[]},");

            newLen = strlen(returnString)+256+strlen(dateStrB);
            returnString = (char*)realloc(returnString, newLen);
            strcat(returnString, dateStrB);

            free(dateStrB);
        }
    }

    //ANNIVERSARY
    if (newCardObject->anniversary != NULL) {
        if (newCardObject->anniversary->isText == true) {

            char *dateStrA = malloc(sizeof(char) * 256);
            
            sprintf(dateStrA, "{\"params\":[\"VALUE\", \"text\"]},");

            newLen = strlen(returnString)+256+strlen(dateStrA);
            returnString = (char*)realloc(returnString, newLen);
            strcat(returnString, dateStrA);

            free(dateStrA);

        }
        else {
            char *dateStrA = malloc(sizeof(char) * 256);
            sprintf(dateStrA, "{\"params\":[]},");

            newLen = strlen(returnString)+256+strlen(dateStrA);
            returnString = (char*)realloc(returnString, newLen);
            strcat(returnString, dateStrA);

            free(dateStrA);
        }
        
    }

    //OPTIONAL PROPERTIES
    ListIterator iter = createIterator(newCardObject->optionalProperties);
    Property* elem;
	while((elem = nextElement(&iter)) != NULL){

        char *str = paramToJSON(elem);

        newLen = strlen(returnString)+256+strlen(str);
        returnString = (char*)realloc(returnString, newLen);

        strcat(returnString, str);
        strcat(returnString, ",");

        free(str);

    }

    //remove the ,
    returnString[strlen(returnString) - 1] = '\0';


    //Add a [ to show end of a list of JSON objects
    strcat(returnString, "]");

    deleteCard(newCardObject);

    return returnString;

}

//Given a property it finds all the parameters in an array
char * paramToJSON (const Property* prop) {

    //{"params":["TYPE", "work", "PREF", "1"]}

    if (prop == NULL) {
        return NULL;
    }


    char *returnString = malloc(sizeof(char) * 100);
    int counter = 0;

    //Add a [ to show start of a list of JSON objects
    strcpy(returnString, "{");

    strcat(returnString, "\"params\":[");

    ListIterator iter = createIterator(prop->parameters);
    Parameter* elem;
	while((elem = nextElement(&iter)) != NULL){

        int length = strlen(returnString) + 256 + strlen(elem->name) + strlen(elem->value);
        returnString = (char*)realloc(returnString, length);

        char *newStr = malloc(sizeof(char) * length);
        
        char *valCopy = escapeQuotes(elem->value);

        sprintf(newStr, "\"%s\", \"%s\"", elem->name, valCopy);
        strcat(returnString, newStr);
        strcat(returnString, ",");

        free(newStr);
        free(valCopy);

        counter++;
    }


    //Remove the last ,
    if (counter > 0) {
        returnString[strlen(returnString) - 1] = '\0'; 
    }
    
    strcat(returnString, "]");
    strcat(returnString, "}");

    return returnString;

}

char * escapeQuotes(char *string) {

    if (string == NULL) {
        return NULL;
    }

    char *returnString = malloc(sizeof(char) * (strlen(string) + 256));

    if ((string[0] == '"') && (string[strlen(string) - 1] == '"')) {

        sprintf(returnString, "\\%s", string);
        int length = strlen(returnString);
        returnString[length - 1] = '\\';
        returnString[length] = '"';
        returnString[length + 1] = '\0';      

    }
    else {
        strcpy(returnString, string);
    }

    return returnString;
}

/* Edit a single Value */
int editSingleValue(char *fileName, int itemNum, char *newValue) {
	
	if (fileName == NULL) {
		//return INV_FILE;
        return -1;
	}
	if (itemNum == 0) {
		//return OTHER_ERROR;
        return -1;
	}
	if (newValue == NULL) {
		//return INV_FILE;
        return -1;
	}
	
	//Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return -1;
	}

	Card *newCardObject = NULL;

	//Create the Card
    VCardErrorCode createError = createCard(fileName, &newCardObject);
    if (createError != OK) {
        //return INV_CARD;
        return -1;
    }
    
    
    VCardErrorCode validateError = validateCard(newCardObject);
     if (validateError != OK) {
		 deleteCard(newCardObject);
		 newCardObject = NULL;
         //return INV_CARD;
         return -1;
     }
     
     
    
    //Find which element needs to be replaced
    int count = 0;
    int element = itemNum;
    if (element < 0) {
		deleteCard(newCardObject);
		newCardObject = NULL;
		//return OTHER_ERROR;
        return -1;
	}
	
	//Check FN
	count++;
	if (element == count) {
		int length = strlen(newValue) + 256;
		newCardObject->fn->values->head->data = (char*)realloc(newCardObject->fn->values->head->data, length);
		strcpy(newCardObject->fn->values->head->data, newValue);
	}
	
	
	//Check Dates (Are not letting them change dates so just increase count)
	if (newCardObject->birthday != NULL) {
		count++;
	}
	
	if (newCardObject->anniversary != NULL) {
		count++;
	}
	
	//Check Optional Properties
	ListIterator iter = createIterator(newCardObject->optionalProperties);
    Property* elem;
	while((elem = nextElement(&iter)) != NULL){
		count++;
		if (element == count) {
			int newLength = strlen(newValue) + 256;
			elem->values->head->data = (char*)realloc(elem->values->head->data, newLength);
			strcpy(elem->values->head->data, newValue);
		}
	}
	
	//Validate Card Again
	VCardErrorCode newValidateError = validateCard(newCardObject);
     if (newValidateError != OK) {
		 deleteCard(newCardObject);
		 newCardObject = NULL;
         //return INV_CARD;
         return -1;
     }
     else {
		 //Write Card
		 VCardErrorCode writeError = writeCard(fileName, newCardObject);
		 if (writeError != OK) {
			 deleteCard(newCardObject);
			 newCardObject = NULL;
			 //return WRITE_ERROR;
             return -1;
		 }
	 }
    

	deleteCard(newCardObject);
	newCardObject = NULL;

	
	return 0;
}

/* Adds a new property to the file (modifies the file) */
int addPropertyToCard (char* fileName, char*JSONstr) {
	
	
	//Check if filename is not null
	if (fileName == NULL) {
		return -1;
	}
	
	//Check if JSON string is not null
	if (JSONstr == NULL) {
		return -1;
	}
	
	//Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return -1;
	}
	
	//create a card from file name
	Card *newCardObject = NULL;
    VCardErrorCode createError = createCard(fileName, &newCardObject);
    if (createError != OK) {
        return -1;
    }
    
    //Validate the card
    VCardErrorCode validateError = validateCard(newCardObject);
     if (validateError != OK) {
		 deleteCard(newCardObject);
		 newCardObject = NULL;
         return -1;
     }
	
	
	//convert the JSON string into a property
	Property* newProp = JSONtoProp(JSONstr);
	if (newProp == NULL) {
		deleteCard(newCardObject);
		newCardObject = NULL;
        return -1;	
	}
	
	//Add property to card
	addProperty(newCardObject, newProp); 
	
	//validate card again
	VCardErrorCode validateError2 = validateCard(newCardObject);
	if (validateError2 != OK) {
		 deleteCard(newCardObject);
		 newCardObject = NULL;
         return -1;
     }
     else {
		 //Write Card
		 VCardErrorCode writeError = writeCard(fileName, newCardObject);
		 if (writeError != OK) {
			 deleteCard(newCardObject);
			 newCardObject = NULL;
			 return -1;
		 }
	 }
    

	deleteCard(newCardObject);
	newCardObject = NULL;
	
	return 0;
	
	
}

/* Creates a new vcard file (simple) */
int createVCardFile (char *fileName, char *JSONstr) {
	
	//Check if filename is not null
	if (fileName == NULL) {
		
		return -1;
	}
	
	//Check if JSON string is not null
	if (JSONstr == NULL) {
		
		return -1;
	}
	
	//Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return -1;
	}

	//Create a new Card object from the JSON string
	Card* newCardObject = JSONtoCard(JSONstr);
	if (newCardObject == NULL) {
		
		return -1;
	}
	
	//validate card
	VCardErrorCode validateError = validateCard(newCardObject);
	if (validateError != OK) {
		 deleteCard(newCardObject);
		 newCardObject = NULL;
		
         return -1;
     }
     else {
		 //Write Card
		 VCardErrorCode writeError = writeCard(fileName, newCardObject);
		 if (writeError != OK) {
			 deleteCard(newCardObject);
			 newCardObject = NULL;
			
			 return -1;
		 }
	 }
    

	deleteCard(newCardObject);
	newCardObject = NULL;

	return 0;
}

/* Validates the card and sends back an integer value for ok or not ok*/
int validateCardReturnInt (char *fileName) {

    //Check if filename is not null
	if (fileName == NULL) {
		return -1;
	}

    //Check if file name is valid extension
	int fileError = checkFileName (fileName);
	if (fileError == -1) {
		return -1;  
	}

    //create a card from file name
	Card *newCardObject = NULL;
    VCardErrorCode createError = createCard(fileName, &newCardObject);
    if (createError != OK) {
        return -1;
    }
    
    //validate card
	VCardErrorCode validateError = validateCard(newCardObject);
	if (validateError != OK) {
		deleteCard(newCardObject);
		newCardObject = NULL;
        return -1;
    }


    deleteCard(newCardObject);
	newCardObject = NULL;

    return 0;

}

/* November 24 */
