// Put all onload AJAX calls here, and event listeners
$(document).ready(function () {

    /* Clear all the input fields upon reload of the page */

    //Edit Property Value
    document.getElementById("inputPropNumber").value = "";
    document.getElementById("inputPropVal").value = "";

    //Add Property
    document.getElementById("inputPropName").value = "";
    document.getElementById("inputPropGroup").value = "";
    document.getElementById("inputPropValue").value = "";

    //Create Vcard
    document.getElementById("inputFNValue").value = "";
    document.getElementById("inputFilename").value = "";



    /* Add scrolling to file log table once its more than 5 rows */
    var rowCount = $('#fileLogTableBody tr').length;
    if (rowCount > 5) {
        console.log(rowCount);

        $('#fileLogDiv').addClass('add-scroll');
        console.log("Added scrolling: " + rowCount + " rows");

    }

    /* Add scrolling to card view table once its more than 5 rows (should it be 10?)*/
    var rowCountCV = $('#CardViewBody tr').length;

    if (rowCountCV > 10) {
        console.log(rowCountCV);

        $('#CardViewDiv').addClass('add-scroll');

        console.log("Added scrolling: " + rowCountCV + " rows");
    }

    /* popup for parameters */
    $('[data-toggle="popover"]').popover();


    /************************ Uploads File Checks ************************/
    //Check if all the files in the uploads is valid and delete the invalid ones
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/deleteInvalidFiles',
        data: {
        },
        success: function (returnData) {

            if (returnData.deleted.length > 0) {
                var i = 0;
                var listOfDeletedFiles = "";
                for (i = 0; i < returnData.deleted.length; i++) {
                    listOfDeletedFiles = listOfDeletedFiles + "- " + returnData.deleted[i] + "\n";
                }

                console.log("Some files were invalid, they will now be deleted");
                console.log(returnData.deleted);
                alert("The following files are invalid and will not be uploaded:\n" + listOfDeletedFiles);
            }
        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });

    /**********************  Populate File Log Chart ***********************/
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/uploadsFile',
        data: {

        },
        success: function (returnData) {

            var table = document.getElementById("fileLogTable").getElementsByTagName('tbody')[0];

            if (returnData.uploadFolderLength === 0) {
                var titleOfFileLog = document.getElementById("filelogTitle");
                titleOfFileLog.innerHTML = "File Log (No Files)";
                document.getElementById("fileLogDiv").style.display = "none";

            }
            else {
                var titleOfFileLog = document.getElementById("filelogTitle");
                titleOfFileLog.innerHTML = "File Log";
                document.getElementById("fileLogDiv").style.display = "block";

                var i = 0;
                for (i = 0; i < returnData.uploadFolderLength; i++) {
                    table.insertRow().innerHTML =
                        "<td>" + "<a href='" + returnData.allFileNames[i] + "' download>" + returnData.allFileNames[i] + "</a>" + "</td>" +
                        "<td>" + returnData.table[i].FN + "</td>" +
                        "<td>" + returnData.table[i].OP + "</td>";

                }
            }

            /* Add scrolling to file log table once its more than 5 rows */
            var rowCount = $('#fileLogTableBody tr').length;
            if (rowCount > 5) {
                console.log(rowCount);

                $('#fileLogDiv').addClass('add-scroll');
            }

            console.log("Successfully populated File Log Chart.");
        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });


    /***********************  Card View Panel ***********************/

    //Drop Down Files
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/cardViewDropDownFiles',
        data: {
        },
        success: function (returnData) {

            var dropdown = document.getElementById("dropDownlistFileNames");

            if (returnData.fileNames.length <= 0) {
                var titleForCardView = document.getElementById("cardViewTitle");
                titleForCardView.innerHTML = "Card View(No Files)";

                dropdown.style.display = "none";

                document.getElementById("CardViewDiv").style.display = "none";

            }
            else {

                dropdown.style.display = "block";
                document.getElementById("CardViewDiv").style.display = "block";

                var i = 0;
                for (i = 0; i < returnData.fileNames.length; i++) {

                    var option = document.createElement("option");

                    option.text = returnData.fileNames[i];
                    option.value = (i + 1);
                    dropdown.add(option);
                }
            }

            console.log("Successfully created drop down options");

        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });

    //Get which drop down option was clicked on
    var fileDropDown = document.getElementById("dropDownlistFileNames");

    fileDropDown.addEventListener("change", function () {
        var text = $("#dropDownlistFileNames option:selected").text();
        var value = $("#dropDownlistFileNames").val();


        if (value != 0) {
            //Make AJAX Request to get the card information
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/popCardView',
                data: {
                    fileName: text
                },
                success: function (returnData) {

                    //Clear the tbody
                    $("#CardViewBody").empty();

                    var table = document.getElementById("CardViewTable").getElementsByTagName('tbody')[0];

                    for (i = 0; i < returnData.cardObj.length; i++) {

                        if (returnData.cardObj[i].name == "BDAY") { //BDAY

                            var newStr = dateInfoForCardView(returnData.cardObj, returnData.paramLen, i)
                            table.insertRow().innerHTML = newStr;
                        }
                        else if (returnData.cardObj[i].name == "ANNIVERSARY") { //Anniversay
                            var newStrAnn = dateInfoForCardView(returnData.cardObj, returnData.paramLen, i)
                            table.insertRow().innerHTML = newStrAnn;

                        }
                        else if (returnData.paramLen[i] > 0) { //If there is 1 or more parameters

                            //Column 1 (Property Number) Column 2 (Property Name)
                            var infoString = "<td>" + (i + 1) + "</td>" +
                                "<td>" + returnData.cardObj[i].name + "</td>";


                            //Column 3 (Property Values)
                            var newVal = cleanUpCommasInValues(returnData.cardObj[i].values);


                            infoString = infoString + "<td>" + newVal + "</td>";


                            //Column 4 (Property Parameters and Show Parameters)
                            infoString = infoString + "<td>" + "<button type=\"button\" class=\"btn btn-outline-dark\" data-toggle=\"popover\" title=\"Parameters\"";

                            // String togeather parameter names = parameters value
                            //If one of the values contains a double quote then remove and replace it with a single quote
                            var paramString = "";
                            var j = 0;
                            for (j = 0; j < returnData.param[i].params.length; j = j + 2) {

                                if (returnData.param[i].params[j + 1][0] == '\"') {

                                    var newParamEdit = returnData.param[i].params[j + 1];

                                    newParamEdit = newParamEdit.substring(1);
                                    newParamEdit = "'" + newParamEdit;
                                    newParamEdit = newParamEdit.slice(0, -1);
                                    newParamEdit = newParamEdit + "'";

                                    paramString = paramString + returnData.param[i].params[j] + " = " + newParamEdit + '\n';
                                }
                                else {
                                    paramString = paramString + returnData.param[i].params[j] + " = " + returnData.param[i].params[j + 1] + '\n';
                                }
                            }

                            //Finish the parameter button and popover
                            infoString = infoString + "data-content=\"" + paramString + "\">" + returnData.paramLen[i] + "</button>" + "</td>";

                            //Add this row to the table
                            table.insertRow().innerHTML = infoString;
                        }
                        else { //if there is 0 parameters

                            //Column 1 (Property Number)
                            //Column 2 (Property Name)
                            var infoString = "<td>" + (i + 1) + "</td>" +
                                "<td>" + returnData.cardObj[i].name + "</td>";

                            //Column 3 (Property Values)
                            //Get rid of empty string commas from the values
                            var newVal = cleanUpCommasInValues(returnData.cardObj[i].values);


                            infoString = infoString + "<td>" + newVal + "</td>";



                            //Column 4 (Property Parameters and Show Parameters)
                            //Parameters are 0 and the button is disabled
                            infoString = infoString +
                                "<td>" + "<button type=\"button\" class=\"btn btn-outline-dark\" disabled>" + returnData.paramLen[i] + "</button>" + "</td>";

                            //Insert Row into the table
                            table.insertRow().innerHTML = infoString;
                        }



                    }

                    /* Add scrolling to card view table once its more than 5 rows (should it be 10?)*/
                    var rowCountCV = $('#CardViewBody tr').length;

                    if (rowCountCV > 10) {

                        $('#CardViewDiv').addClass('add-scroll');
                    }

                    /* popup for parameters */
                    $('[data-toggle="popover"]').popover();


                    console.log("Successfully populated card view chart based on the option from the drop down");
                },
                fail: function (error) {
                    // Non-200 return, do something with error
                    console.log(error);
                }
            });
        }

        /*********************** Edit Property Value ***********************/

        var editPropVal = document.getElementById("editPropertyValue");

        var errorMessage = document.getElementById("errorForEditPropVal");

        //Show the Edit Property Value Form
        editPropVal.style.display = "block";
        console.log("Showing edit property form");

        //Once the submit button is clicked start an ajax call
        var submitButton = document.getElementById("editPropValCardViewButton");
        submitButton.onclick = function (e) {

            var propertyTableNumber = document.getElementById("inputPropNumber").value;
            var newpropertyValue = document.getElementById("inputPropVal").value;

            //Can't have empty values as inputs
            //Need to check if the number is one of the editable ones
            //Need to check if the number is in the valid range (1-length)
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/popCardView',
                data: {
                    fileName: text
                },
                success: function (returnData) {

                    console.log("#1");
                    console.log(errorMessage.style.display);

                    if ((propertyTableNumber == "") && (newpropertyValue == "")) {

                        errorMessage.style.display = "block";
                        errorMessage.innerHTML = "Property Number and New Property Value can't be empty, please try again.";
                        console.log("Property Number and New Property Value can't be empty, please try again.");
                    }
                    else if (propertyTableNumber == "") {

                        errorMessage.style.display = "block";
                        errorMessage.innerHTML = "Property Number can't be empty, please try again.";
                        console.log("Property Number can't be empty, please try again.");

                    }
                    else if (newpropertyValue == "") {

                        errorMessage.style.display = "block";
                        errorMessage.innerHTML = "New Property Value can't be empty, please try again.";
                        console.log("New Property Value can't be empty, please try again.");

                    }

                    //All the variables you need for the error checks
                    var lengthOfFileCard = returnData.cardObj.length;
                    var arrayReferenceNum = propertyTableNumber - 1;
                    var propToChange = returnData.cardObj[arrayReferenceNum];



                    //Check if the number is from the valid range
                    if ((propertyTableNumber != "") && (newpropertyValue != "")) {

                        if (isNumeric(propertyTableNumber) == false) {
                            errorMessage.style.display = "block";
                            errorMessage.innerHTML = "Property Number (" + propertyTableNumber + ") is invalid, please enter a valid number.";
                            console.log("Property Number is invalid");
                        }
                        else if ((propertyTableNumber > lengthOfFileCard) || (propertyTableNumber < 1)) {

                            errorMessage.style.display = "block";
                            errorMessage.innerHTML = "Property Number (" + propertyTableNumber + ") is invalid, please try again.";
                            console.log("Property Number is invalid");
                        }
                        else if ((propToChange.name == "BDAY") || (propToChange.name == "ANNIVERSARY")) {

                            //Check if the property is a date (not allowing edit of dates yet)
                            errorMessage.style.display = "block";
                            errorMessage.innerHTML = "Property Number (" + propertyTableNumber + ") refers to a date, please try again.";
                            console.log("Property number refers to a date.");
                        }
                        else if (propToChange.values.length > 1) {

                            //Check if the length of the value list is not more than 1 (compound values not allowed)
                            errorMessage.style.display = "block";
                            errorMessage.innerHTML = "Property Number (" + propertyTableNumber + ") refers to a property which has compound values, please try again with a single value.";
                            console.log("Property Number refers to to a property with compound values, only single values allowed");
                        }
                        else if (newpropertyValue.indexOf(";") != -1) {

                            //Do not allow users to input compound values
                            errorMessage.style.display = "block";
                            errorMessage.innerHTML = "Property Value is a compound value, please try again with a single value.";
                            console.log("Property Value is a compound value, please try again with a single value.");
                        }
                        else {
                            errorMessage.style.display = "none";

                            //Call Ajax and send this information to the C library
                            $.ajax({
                                type: 'get',
                                dataType: 'json',
                                url: '/editPropertyValue',
                                data: {
                                    tableNumber: propertyTableNumber,
                                    valueChange: newpropertyValue,
                                    fileName: "uploads/" + text
                                },
                                success: function (returnData) {

                                    if (returnData.returnResult == 0) {
                                        console.log("Successfully Updated Property Value");
                                        alert("Successfully Updated Property Value!");

                                        //Refresh the page
                                        location.reload();
                                        return false;
                                    }
                                    else {
                                        console.log("Error, could not update property value");
                                        alert("Error, could not update property value");

                                    }


                                },
                                fail: function (error) {
                                    // Non-200 return, do something with error
                                    console.log(error);
                                }
                            });
                        }

                    }


                },
                fail: function (error) {
                    // Non-200 return, do something with error
                    console.log(error);
                }
            });
        }

        /************************ Add Property ************************/

        //Show the form
        var addProp = document.getElementById("addProperty");
        addProp.style.display = "block";
        console.log("Showing add property form");

        var errorMessageAddProp = document.getElementById("errorForAddProp");
        errorMessageAddProp.style.display = "none";

        var addPropButton = document.getElementById("addPropCardViewButton");
        addPropButton.onclick = function (e) {

            var propName = document.getElementById("inputPropName").value;
            var propGroup = document.getElementById("inputPropGroup").value;
            var propValue = document.getElementById("inputPropValue").value;


            // Prop Name must not be empty, Value can't be empty
            if (propName == "") {
                errorMessageAddProp.style.display = "block";
                errorMessageAddProp.innerHTML = "Property Name can't be empty. Please input a property name.";
                console.log("Property Name can't be empty. Please input a property name.");
            }
            else if (propValue == "") {
                errorMessageAddProp.style.display = "block";
                errorMessageAddProp.innerHTML = "Property Value must have atleast 1 item in it.";
                console.log("Property Value must have atleast 1 item in it.");
            }
            else if (propValue.indexOf(";") != -1) {
                errorMessageAddProp.style.display = "block";
                errorMessageAddProp.innerHTML = "Property Value must not be compound. Please enter a single value.";
                console.log("Property Value must not be compound.");
            }
            else {
                errorMessageAddProp.style.display = "none";

                // Make all the property items into a JSON object
                var JSONPropObj = { group: propGroup, name: propName, values: [propValue] };

                //Stringify the JSON object
                var JSONString = JSON.stringify(JSONPropObj);

                //Pass it into an AJAX call
                $.ajax({
                    type: 'get',
                    dataType: 'json',
                    url: '/addProperty',
                    data: {
                        fileName: "uploads/" + text,
                        property: JSONString
                    },
                    success: function (returnData) {

                        if (returnData.returnResult === 0) {
                            console.log("Successfully Added New Property!");
                            alert("Successfully Added New Property!");

                            //Refresh the page
                            location.reload();
                            return false;
                        }
                        else {

                            console.log("Error, could not add new property");
                            alert("Error, could not add new property");
                        }

                    },
                    fail: function (error) {
                        // Non-200 return, do something with error
                        console.log(error);
                    }
                });
            }

        }

    });


    /************************ Create A New Vcard ************************/
    var errorCreateCard = document.getElementById("errorMessageForCreateCard");
    errorCreateCard.style.display = "none";

    var submitButtonCreateCard = document.getElementById("submitCreateCard");

    submitButtonCreateCard.onclick = function (e) {

        var FNValue = document.getElementById("inputFNValue").value;
        var file = document.getElementById("inputFilename").value;


        //Check to see if the file name already exists (If it already exists it refreshes the page)
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/cardViewDropDownFiles',
            data: {
            },
            success: function (returnData) {

                var returnVar = 1;

                var i = 0;
                for (i = 0; i < returnData.fileNames.length; i++) {

                    if (returnData.fileNames[i] === file) {
                        returnVar = -1; //file already exists
                    }
                }

                //Check if the file name is not empty //Check if the values box is not empty
                //Check if the file name has the correct extension
                var checkFile = validateFileName(file);
                if ((file == "") && (FNValue == "")) {
                    errorCreateCard.style.display = "block";
                    errorCreateCard.innerHTML = "File Name and FN value can't be empty, please fill those fields.";
                    console.log("File Name and FN value can't be empty, please fill those fields.");
                }
                else if (file == "") {
                    errorCreateCard.style.display = "block";
                    errorCreateCard.innerHTML = "File Name can't be empty, please enter a file name.";
                    console.log("File Name can't be empty, please enter a file name.");
                }
                else if (FNValue == "") {
                    errorCreateCard.style.display = "block";
                    errorCreateCard.innerHTML = "FN value can't be empty, please enter a value.";
                    console.log("FN value can't be empty, please enter a value.");
                }
                else if (checkFile == -1) {
                    errorCreateCard.style.display = "block";
                    errorCreateCard.innerHTML = "Incorrect File Extension. Please enter a file name with either a .vcard or .vcf extension";
                    console.log("Incorrect File Extension.");
                }
                else if (returnVar == -1) {
                    errorCreateCard.style.display = "block";
                    errorCreateCard.innerHTML = "The file you are trying to create already exists. Please try a different name.";
                    console.log("The file you are trying to create already exists.");
                }
                else if (FNValue.indexOf(";") != -1) {

                    //Do not allow users to input compound values
                    errorCreateCard.style.display = "block";
                    errorCreateCard.innerHTML = "FN Value can't be a compound value, please enter a single value.";
                    console.log("FN Value can't be a compound value, please enter a single value.");
                }
                else {
                    errorCreateCard.style.display = "none";

                    //Create a JSON object like the one in JSON to card with all the information
                    var JSONCardObj = { FN: FNValue };

                    //Stringify the JSON object
                    var JSONCardString = JSON.stringify(JSONCardObj);
                    console.log(JSONCardString);

                    //Create an AJAX call 
                    //Send in the filename and the JSON object
                    $.ajax({
                        type: 'get',
                        dataType: 'json',
                        url: '/createVCard',
                        data: {
                            card: JSONCardString,
                            fileName: file
                        },
                        success: function (returnData) {

                            if (returnData.returnResult == 0) {

                                console.log("Successfully Created a new VCard File!");
                                alert("Successfully Created a new VCard File!");

                                //Refresh the page
                                location.reload();
                                return false;

                            }
                            else {
                                console.log("Error, could not create new Vcard File");
                                alert("Error, could not create new VCard File.");
                            }



                        },
                        fail: function (error) {
                            // Non-200 return, do something with error
                            console.log(error);
                        }
                    });
                }

            },
            fail: function (error) {
                // Non-200 return, do something with error
                console.log(error);
            }
        });
    }


    /************************ Upload a VCard File ************************/
    //The button that submits the upload request
    var buttonForUpload = document.getElementById("uploadButton");

    //When the button is clicked
    buttonForUpload.onclick = function (e) {

        //The button that uploads the file from the browser
        var fileButton = document.getElementById("uploadFile");
        var fileNameFromUpload = fileButton.files[0].name;

        //The entire form
        var uploadForm = document.getElementById("uploadForm");
        var uploadButtonError = document.getElementById("uploadErrorMessage");


        //Stop Submit until all the error checks are done
        e.preventDefault();

        //Carry out Error Checks
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/cardViewDropDownFiles',
            data: {
            },
            success: function (returnData) {

                // Check if the file has the right extension
                var checkUploadedFileName = validateFileName(fileNameFromUpload);


                //Check if none of the file names are repeated
                var returnVarForUpload = 1;
                var i = 0;
                for (i = 0; i < returnData.fileNames.length; i++) {

                    if (returnData.fileNames[i] === fileNameFromUpload) {
                        returnVarForUpload = -1; //file already exists
                    }
                }

                //Do all the checks and show error message to user, else submit 
                if (checkUploadedFileName === -1) {
                    console.log("File can't be uploaded due to incorrect file extension. Please Try Again.");
                    uploadButtonError.innerHTML = "File can't be uploaded due to incorrect file extension. Please Try Again.";

                }
                else if (returnVarForUpload == -1) {
                    console.log("File Name already exists, please upload another file.");
                    uploadButtonError.innerHTML = "File Name already exists, please upload another file.";
                }
                else {
                    //Submit once it is a unique and valid file
                    console.log("File Submitted!");
                    uploadForm.submit();
                }
            },
            fail: function (error) {
                // Non-200 return, do something with error
                console.log(error);
            }
        });
    }


    /* Validates if the filename has the correct extension */
    function validateFileName(string) {

        if (string == null) {
            return -1;
        }

        var len = string.length;

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
            return -1; //File extension not valid
        }
    }

    //Checks to see if it is a number or not
    function isNumeric(num) {
        return !isNaN(num)
    }



    /* Compound Values have multiple ,,, so we want to get rid of the extras*/
    function cleanUpCommasInValues(valArray) {

        var str = "";

        var i = 0;
        for (i = 0; i < valArray.length; i++) {

            if (valArray[i] != "") {
                str = str + valArray[i] + ", ";
            }
        }

        str = str.slice(0, -2);

        return str;
    }

    /* The Function Creates a table row for dates */
    function dateInfoForCardView(cardObj, paramLen, i) {
        var str = "<td>" + (i + 1) + "</td>" +
            "<td>" + cardObj[i].name + "</td>";

        if (cardObj[i].isText == true) {
            str = str + "<td>" + cardObj[i].text + "</td>";
        }
        else {

            if (cardObj[i].date != "") { //There is a date
                str = str + "<td>" + " Date: " + cardObj[i].date;
                if (cardObj[i].time != "") { //check if there is time or not

                    if (cardObj[i].isUTC == true) {
                        str = str + "<br>" + "Time: " + cardObj[i].time + "(UTC)" + "</td>";
                    }
                    else {
                        str = str + "<br>" + "Time: " + cardObj[i].time + "</td>";
                    }

                }
                else {
                    str = str + "</td>";
                }
            }
            else { //there is not a date (MUST be time)

                if (cardObj[i].time != "") {
                    if (cardObj[i].isUTC == true) {
                        str = str + "<td>" + " Time: " + cardObj[i].time + "(UTC)" + "</td>";
                    }
                    else {
                        str = str + "<td>" + " Time: " + cardObj[i].time + "</td>";
                    }

                }
            }

        }

        if (paramLen[i] > 0) {
            str = str + "<td>" + "<button type=\"button\" class=\"btn btn-outline-dark\" data-toggle=\"popover\" title=\"Parameters\" data-content=\"VALUE=text\">" + paramLen[i] + "</button>" + "</td>";
        }
        else {
            str = str + "<td>" + "<button type=\"button\" class=\"btn btn-outline-dark\" disabled>" + paramLen[i] + "</button>" + "</td>";

        }

        return str;
    }

});

/* November 24 */