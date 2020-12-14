'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app = express();
const path = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname + '/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/', function (req, res) {
  res.sendFile(path.join(__dirname + '/public/index.html'));
});

// Send Style, do not change
app.get('/style.css', function (req, res) {
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname + '/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js', function (req, res) {
  fs.readFile(path.join(__dirname + '/public/index.js'), 'utf8', function (err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, { compact: true, controlFlowFlattening: true });
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function (req, res) {
  if (!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function (err) {
    if (err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function (req, res) {
  fs.stat('uploads/' + req.params.name, function (err, stat) {
    if (err == null) {
      res.sendFile(path.join(__dirname + '/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: ' + err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Get All the files from the uploads folder
app.get('/uploadsFile', function (req, res) {

  let ffi = require("ffi-napi");
  let sharedLib = ffi.Library('./parser/bin/libvcparser', {
    'getFNValue': ['string', ['string']],
    'lengthOfOPList': ['int', ['string']]
  });

  var fs = require('fs');
  var files = fs.readdirSync('uploads');

  var i = 0;
  var tableObject = [];
  var fileLength = files.length;

  for (i = 0; i < fileLength; i++) {

    var str = "uploads/" + files[i];
    var name = sharedLib.getFNValue(str);
    var lengthOP = sharedLib.lengthOfOPList(str);

    if ((name != null) && (lengthOP != -1)) {

      name = name.replace("\n", "");
      var obj = {
        FN: name,
        OP: lengthOP
      }

      tableObject.push(obj);

    }
    else {

      //When you splice it you remove a filename, so the length of the file should decrease
      //BUT you need to reset i by 1 otherwise you will skip one file
      files.splice(i, 1);
      fileLength = fileLength - 1;
      i = i - 1;
    }
  }

  res.send({
    table: tableObject,
    uploadFolderLength: fileLength,
    allFileNames: files
  });
});


/* Card View Panel */

//Drop Down
app.get('/cardViewDropDownFiles', function (req, res) {

  //Gets the function from the library 
  let ffi = require("ffi-napi");
  let sharedLib = ffi.Library('./parser/bin/libvcparser', {
    'validateCardReturnInt': ['int', ['string']]
  });

  //Get all the files in the uploads folder
  var fs = require('fs');
  var files = fs.readdirSync('uploads');

  var fileLength = files.length;
  var returnNumber = 0;
  var i = 0;
  for (i = 0; i < fileLength; i++) {

    var fullFileName = "uploads/" + files[i];
    returnNumber = sharedLib.validateCardReturnInt(fullFileName);

    if (returnNumber === -1) {
      files.splice(i, 1);
      fileLength = fileLength - 1;
      i = i - 1;
    }
  }

  res.send({
    fileNames: files
  });


});


/* Populates the Card View (Sends back a JSON object) */
app.get('/popCardView', function (req, res) {

  let ffi = require("ffi-napi");
  let sharedLib = ffi.Library('./parser/bin/libvcparser', {
    'fileToJSON': ['string', ['string']],
    'lengthOfParamList': ['string', ['string']],
    'fileToParamToJSON': ['string', ['string']]
  });

  var str = "uploads/" + req.query.fileName;
  var cardStr = sharedLib.fileToJSON(str);
  var paramString = sharedLib.fileToParamToJSON(str);
  var lengthArray = sharedLib.lengthOfParamList(str);


  var card = JSON.parse(cardStr);
  var JSONParamString = JSON.parse(paramString);
  var lengthsOfParams = JSON.parse(lengthArray);


  res.send({
    cardObj: card,
    paramLen: lengthsOfParams,
    param: JSONParamString
  });

});


/* Edits a property value using the user input */
app.get('/editPropertyValue', function (req, res) {

  let ffi = require("ffi-napi");
  let sharedLib = ffi.Library('./parser/bin/libvcparser', {
    'editSingleValue': ['int', ['string', 'int', 'string']],
  });

  var returnVar = sharedLib.editSingleValue(req.query.fileName, req.query.tableNumber, req.query.valueChange);

  res.send({
    returnResult: returnVar
  });

});


/* Adds a property to a card (Reads in the file, validate file, convert string to a property, add property, validate, write to file)*/
app.get('/addProperty', function (req, res) {

  let ffi = require("ffi-napi");
  let sharedLib = ffi.Library('./parser/bin/libvcparser', {
    'addPropertyToCard': ['int', ['string', 'string']],
  });

  var returnVar = sharedLib.addPropertyToCard(req.query.fileName, req.query.property);

  res.send({
    returnResult: returnVar
  });

});

/* Creates a new vcard file */
app.get('/createVCard', function (req, res) {

  let ffi = require("ffi-napi");
  let sharedLib = ffi.Library('./parser/bin/libvcparser', {
    'createVCardFile': ['int', ['string', 'string']],
  });

  var str = "uploads/" + req.query.fileName;

  var returnVar = sharedLib.createVCardFile(str, req.query.card);

  res.send({
    returnResult: returnVar
  });

});


/* Validates the File */
app.get('/validateFile', function (req, res) {

  let ffi = require("ffi-napi");
  let sharedLib = ffi.Library('./parser/bin/libvcparser', {
    'validateCardReturnInt': ['int', ['string']]
  });

  var returnVar = sharedLib.validateCardReturnInt(req.query.fileName);

  console.log(req.query.fileName);
  console.log(returnVar);
  res.send({
    returnResult: returnVar
  });

});


app.get('/deleteInvalidFiles', function (req, res) {

  //Gets the function from the library 
  let ffi = require("ffi-napi");
  let sharedLib = ffi.Library('./parser/bin/libvcparser', {
    'validateCardReturnInt': ['int', ['string']]
  });

  //Get all the files in the uploads folder
  var fs = require('fs');
  var files = fs.readdirSync('uploads');

  var fileLength = files.length;
  var deletedObject = [];
  var returnNumber = 0;
  var i = 0;
  for (i = 0; i < fileLength; i++) {


    var fullFileName = "uploads/" + files[i];
    returnNumber = sharedLib.validateCardReturnInt(fullFileName);

    if (returnNumber === -1) {

      //Delete the invalid file
      fs.unlink(fullFileName, (err => {
        if (err) console.log(err);
      }));


      //Add it to the object that stores deleted filenames
      deletedObject.push(files[i]);
    }
  }

  res.send({
    deleted: deletedObject
  });


});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

/* November 24 */
