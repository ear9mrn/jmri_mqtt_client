/*
 *  © 2022 Pete Nevill
 *  All rights reserved.
 *
 *  This file is part of JMRI MQTT Accesssory Client
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */
 
#ifndef HTMLTEMP_h
#define HTMLTEMP_h

#include "config.h"
#include "headers.h"
#include "LittleFS.h"

class JMRI_HTML {

    public:
    
        static void       html_init();    
  
};

const char jscript[] PROGMEM = R"rawliteral(


//board types as per main code
const btype = [" ", "ESP8266", "PCA9685", "PCF857x"];

//events listner
if (!!window.EventSource) {
    var source = new EventSource('/events');
}

        
window.onload=function(){

    checkCookie();
    
    document.getElementById("jsonfileinput").addEventListener("change", function() {
          
          if (confirm("Existing configuration will be overwritten...\nAre you sure?") == true) {

              var content = "";
              var fileObj = document.getElementById("jsonfileinput").files[0];
              var fileSize = document.getElementById("jsonfileinput").files[0].size
              let fileread = new FileReader();
          
              fileread.onload = function(e) {
                  var content = e.target.result;  
                  //console.log(content.substr(2,8));

                  if (content.substr(2,8) == "mqtthost") {
             
                        var url = "restore"; 
                        var form = new FormData(); 
                        form.append("mf", fileObj); 
                 
                        xhr = new XMLHttpRequest();
                        xhr.open("post", url, true);
                        xhr.setRequestHeader("x-filesize", fileSize);
                        xhr.send(form);
                  
                  } else {
                        alert("Error reading file. This does not look like the correct format...");
                  }

              };
     
              fileread.readAsText(fileObj);
          }

          document.getElementById("jsonfileinput").value = "";
    });

    document.getElementById("firmwareupdate").addEventListener("change", function() {
          
          if (confirm("Existing firmware will be upgraded.\nAre you sure?") == true) {

              var content = "";
              var fileObj = document.getElementById("firmwareupdate").files[0];
              var fileSize = fileObj.size
              let fileread = new FileReader();
          
              fileread.onload = function(e) {
                  var content = e.target.result;  
                  //console.log(content.substr(2,8));      
                  var url = "/firmwareUpload"; 
                  var form = new FormData(); 
                  form.append("mf", fileObj); 
                 
                  xhr = new XMLHttpRequest();
                  xhr.open("post", url, true);
                  xhr.setRequestHeader("x-filesize", fileSize);
                  xhr.send(form);

              };
     
              fileread.readAsText(fileObj);
          }

          document.getElementById("firmwareupdate").value = "";
    });
    

}


//get cookie to show same tab as last selected
function checkCookie(){
  
      //console.log(document.cookie.indexOf('jmri_mqtt='));
      if ( document.cookie.indexOf('jmri_mqtt=') == -1 ) {
          //console.log("cookie does not exist" );
          document.cookie = "jmri_mqtt=0;";
      } 
}  


//listener for setting dot colour
source.addEventListener('setDot', function(e){ 
      //console.log('SetDot', e.data);
      var update = e.data.split(":");
      var id = document.getElementById(update[0]);
      
      id.setAttribute('data-state',update[1]);
      
      if ( update[1] == 'I' || update[1] == 'C' || update[1] == 'F' ){
            id.style.backgroundColor = "red";
      } else {
            id.style.backgroundColor = "green";
      }  

}, false);

//refresh page
source.addEventListener('refresh', function(e){ 
                  location.reload();
}, false);
                  
//function to update changes to MQTT topic
function updatemqtttopic(){

          var topic = document.getElementById("mqtttopic").value;

          if (topic.length <= 20) {
                var params = "mqtttopic=" + topic;
                //console.log(params);
                var xhr = new XMLHttpRequest();              
                xhr.open("POST", "/changemqtttopic", true);
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send(params);
          } else {
               alert("Topic string too long, max 20 characters..."); 
          }
}

//function to call to update sketch from github with latest version
function sketchUpdate(){

            if (confirm("Confirm you wish to update to the latest version?") == true) {
                  var params = "lver=" + document.getElementById('lver').textContent;
                  //console.log(params);
                  var xhr = new XMLHttpRequest();              
                  xhr.open("POST", "/urlfirmwareUpload", true);
                  xhr.send(params);
            } 
}
//function to call if eeprom reset button pressed
function eepromReset(){

            if (confirm("All configuration data will be erased (including WIFI settings).\nReconnection to WIFI will be required. \nAre you sure?") == true) {
                  var xhr = new XMLHttpRequest();              
                  xhr.open("GET", "/reset_eeprom", true);
                  xhr.send();
            } 
}
//function to call if rescan of i2c devices button click
function i2cScan(){

            if (confirm("Rescan for I2C devices.\nAre you sure?") == true) {
                  var xhr = new XMLHttpRequest();              
                  xhr.open("GET", "/i2cscan", true);
                  xhr.send();                
            }
}

//function to update changes to MQTT host
function updatemqtthost(){

          var ip = document.getElementById("mqttip").value
          
          if (/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/.test(ip))
          {
                //console.log("Ip addres ok");
                var params = "mqtthost=" + ip;
                //console.log(params);
                var xhr = new XMLHttpRequest();              
                xhr.open("POST", "/changemqttip", true);
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send(params);
            
                return (true)
          }
         
          alert("You have entered an invalid IP address!")     
          return (false)

}

//function to update changes to MQTT port
function updatemqttport(){
  
        var port = document.getElementById("mqttport").value;
      
        port  === parseInt(port, 10);
        if ( port >= 0 && port <= 9999) {
      
              var params = "mqttport=" + port;
              //console.log(params);
              var xhr = new XMLHttpRequest();              
              xhr.open("POST", "/changemqttport", true);
              xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
              xhr.send(params);
                 
        } else {
          
              alert("Please enter a valid port value...");
      
        }

}

//function to call if dot button click to change light or turnout
function dot_click(element) {

        var pin = element.getAttribute("name");
        //console.log(pin);
        var sysname = document.getElementById(pin).value;
        var state = element.dataset.state;
      
        //we flip the state here
        if (state == 'T'){
            state = 'C';
        } else if (state == 'C'){
            state = 'T';
        } else if (state == 'A'){
            state = 'I';
        } else if (state == 'I'){
            state = 'A';
        } else if (state == 'N'){
            state = 'F';
        } else if (state == 'F'){
            state = 'N';
        }
        
        var params = "sysname=" + sysname + "&state=" + state;
        //console.log(params);
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/dot_button_click", true);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.send(params);
}

//function called when PWM button is changed
function pwmClick(elem){
  
        //console.log(elem);
        var id = elem.getAttribute("id")
        var prop = id.split(":");
        var checked = document.getElementById(id).checked
      
        var params = "bname=" + prop[1] + "&name=" +  prop[2] + "&state=" + checked;
        //console.log(params);
      
        var xhr = new XMLHttpRequest();
        if (document.getElementById(id).checked) {
              document.getElementById("la:" + prop[1] + ":" + prop[2]).disabled = false;
              document.getElementById("ha:" + prop[1] + ":" + prop[2]).disabled = false;
        } else {
              document.getElementById("la:" + prop[1] + ":" + prop[2]).disabled = true;
              document.getElementById("ha:" + prop[1] + ":" + prop[2]).disabled = true;
        }
      
        xhr.open("POST", "/changechkb", true);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.send(params);
}

function descChange(elem){
        var id = elem.getAttribute("id")
        var prop = id.split(":"); 
        var desc = document.getElementById(id).value;  
        var params = "bname=" + prop[1] + "&pin=" +  prop[2] + "&desc=" + desc;
        //console.log(params); 
        
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/changedesc", true);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.send(params); 
}

function bdescChange(elem){
        var id = elem.getAttribute("id")
        var prop = id.split(":"); 
        var desc = document.getElementById(id).value;  
        var params = "bname=" + prop[1] + "&desc=" + desc;
        //console.log(params); 
        
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/changebdesc", true);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.send(params);
}
//function called if txt box (system name) is change/updated
function txtChange(elem){
        
        //console.log(elem);
        var id = elem.getAttribute("id")
        var prop = id.split(":");
        var sysname = document.getElementById(id).value;   
        sysname === parseInt(sysname, 10); 
        var theDot = document.getElementById(prop[1]+"dot"+prop[2])
        var theDesc = document.getElementById("desc:" + prop[1] + ":" + prop[2]) 
        
        if ( sysname >= 0 && sysname <= 65535) {
      
              theDot.className = "dot pointer";
              theDot.onclick = "dot_click(this)";
              var params = "bname=" + prop[1] + "&name=" +  prop[2] + "&sysname=" + sysname;
              theDot.setAttribute("onclick","dot_click(this)");
              theDesc.disabled = false;
              
              //console.log(params);
              var xhr = new XMLHttpRequest();
              xhr.open("POST", "/changename", true);
              xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
              xhr.send(params);              
              
        } else {
              document.getElementById(id).value = 0;
              theDesc.value = " ";
              theDesc.disabled = true;

              var params = "bname=" + prop[1] + "&pin=" +  prop[2] + "&desc=";
              xhr.open("POST", "/changedesc", true);
              xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
              xhr.send(params);
        
        } 
        
        if (document.getElementById(id).value == 0){
              theDot.className = "dot";
              theDot.setAttribute("onclick"," ");
              theDot.style.backgroundColor = "red"
              theDesc.disabled = true;
              theDesc.value = " ";

              var params = "bname=" + prop[1] + "&pin=" +  prop[2] + "&desc=";
              xhr.open("POST", "/changedesc", true);
              xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
              xhr.send(params);
        
        }

}

//function called if radio button is changed
function radioClick(elem){
  
        //console.log(elem);
        var id = elem.getAttribute("id")
        var prop = id.split(":");
        var mode = elem.getAttribute("value")
      
        var params = "bname=" + prop[1] + "&name=" +  prop[2] + "&mode=" + mode;
        //console.log(params);
      
        if (mode == 'L' || mode == 'S'){
          document.getElementById("pwm:" + prop[1] + ":" + prop[2]).disabled = true;
          document.getElementById("la:" + prop[1] + ":" + prop[2]).disabled = true;
          document.getElementById("ha:" + prop[1] + ":" + prop[2]).disabled = true;
          
        } else {
          document.getElementById("pwm:" + prop[1] + ":" + prop[2]).disabled = false;
          
          if (document.getElementById("pwm:" + prop[1] + ":" + prop[2]).checked) {
            document.getElementById("la:" + prop[1] + ":" + prop[2]).disabled = false;
            document.getElementById("ha:" + prop[1] + ":" + prop[2]).disabled = false;
            
          }
        }
      
        if ((mode == 'L' || mode == 'T') && document.getElementById("txt:" +prop[1]+":"+prop[2]).value != "0" ){
              document.getElementById(prop[1]+"dot"+prop[2]).setAttribute("onclick","dot_click(this)");
              document.getElementById(prop[1]+"dot"+prop[2]).className = "dot pointer";
        } else {
              document.getElementById(prop[1]+"dot"+prop[2]).setAttribute("onclick"," ");
              document.getElementById(prop[1]+"dot"+prop[2]).className = "dot";
        }
      
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/changeradio", true);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.send(params);
}

//function called if PWM angles are changed
function angleChange(elem){
  
        //console.log(elem);
        var id = elem.getAttribute("id");
        var prop = id.split(":");
        var val = document.getElementById(id).value;
        var lval = document.getElementById("la:"+prop[1]+":"+prop[2]).value;
        var hval = document.getElementById("ha:"+prop[1]+":"+prop[2]).value;
        var both = 0;
      
        if (parseInt(hval) < 1){
              document.getElementById("ha:"+prop[1]+":"+prop[2]).value = 1;
              hval = 1;
              both = 1;
        }
        
        if (parseInt(lval) > 179){
              document.getElementById("la:"+prop[1]+":"+prop[2]).value = 179;
              lval = 1;
              both = 1;
        }
      
        if (parseInt(hval) <= parseInt(lval)){
              if (prop[0] == "ha") {
                  document.getElementById("la:"+prop[1]+":"+prop[2]).value = parseInt(hval) - 1; 
              } else {
                  document.getElementById("ha:"+prop[1]+":"+prop[2]).value = parseInt(hval) + 1; 
              }
              both = 1;
        } 


        var xhr = new XMLHttpRequest();
        
        if (prop[0] == "la" || both == 1) {
              xhr.open("POST", "/anglechange", true);
              xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
              var params = "bname=" + prop[1] + "&name=" +  prop[2] + "&angle=" + lval + "&highlow=" + "la";
              //console.log(params);   
              xhr.send(params);
        }
      
        if (prop[0] == "ha" || both == 1 ) {
              xhr.open("POST", "/anglechange", true);
              xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
              var params = "bname=" + prop[1] + "&name=" +  prop[2] + "&angle=" + hval + "&highlow=" + "ha";
              //console.log(params);   
              xhr.send(params);
        }      
}

//get value of cookie to know which tab to show.
function getCookie(n) {
      let a = `; ${document.cookie}`.match(`;\\s*${n}=([^;]+)`);
      return a ? a[1] : '';
}

//function called when tab is selected
function openTable(evt, tabName) {
   
        var i, tabcontent, tablinks;
        tabcontent = document.getElementsByClassName("tabcontent");
        for (i = 0; i < tabcontent.length; i++) {
              //console.log(tabcontent[i].id);
              tabcontent[i].style.display = "none";
        }
    
        tablinks = document.getElementsByClassName("tablinks");
        for (i = 0; i < tablinks.length; i++) {
              tablinks[i].className = tablinks[i].className.replace(" active", "");
        }
    
        document.getElementById(tabName).style.display = "block";
        evt.currentTarget.className += " active";
    
        var tab = tabName.substring(3);
        document.cookie="jmri_mqtt=" + tab + ";"
        
}

//get json info from ESP8266 webserver
function getJson(){
        var xhr = new XMLHttpRequest();
        var data;
        var jsonResponse;
    
        xhr.open("GET", "/json", true);
        xhr.send();
     
        xhr.addEventListener("readystatechange", function() {
            if(this.readyState === 4) {
                  //console.log(this.responseText);
                  data= xhr.responseText;
                  jsonResponse = JSON.parse(data);
                  //console.log(jsonResponse);
                  buildTable(jsonResponse);
                  document.getElementById('ipadd').textContent = jsonResponse.clientip;
                  document.getElementById('cver').textContent = Number(jsonResponse.cver).toFixed(2);
                  if (parseFloat(jsonResponse.lver) == 99) {
                      document.getElementById('lver').title = 'Problem obtaining version details. Check internet connection!';
                      document.getElementById('lver').textContent = "Unknown!";
                  } else {                   
                      document.getElementById('lver').textContent = Number(jsonResponse.lver).toFixed(2);
                      if ( parseFloat(jsonResponse.lver) > parseFloat(jsonResponse.cver) ){
                            document.getElementById('href_lver').className = " ";
                            document.getElementById('href_lver').setAttribute('onclick', 'sketchUpdate()');
                            document.getElementById('lver').title = 'Download and install latest firmware from github.';  
                      }
                  }
                  document.getElementById('macadd').textContent = jsonResponse.clientmac;
                  document.getElementById("defaultOpen").click();
            }  
        });  
}

//build tabs and table using json info from webserver.
function buildTable(data){
  
        var tabs = document.getElementById('myTab');
        var mainTable = document.getElementById('mainTable');
        var headTable = document.getElementById('headTable');
        var pins = 9;
        var bdesc = " ";
      
        var seltab = getCookie('jmri_mqtt');
        if( seltab >= data.number-1) {
          setlab = data.number-1;
        }
        
        for (var j = 0; j < data.number; j++){  
          
              var defid = "";    
              if ( seltab == j ){
                  defid = "id='defaultOpen'";
              }
              if (data.bdesc[j] == " "){
                    bdesc = "Board " + j;
              } else {
                    bdesc = data.bdesc[j];
              }
          
              var tab = `<button class='tablinks' onclick="openTable(event, 'tab${j}')" ${defid} title="Click to show board parameters.">
              Addr:0x${data.addr[j].toString(16)}<br>${btype[data.type[j]]} <br>
              <input class="tabtxt" type='text' id='bdsec:${j}' size='7' maxlength="20" value='${bdesc}' onChange='bdescChange(this)' 
              title="Add name or description." ></button>`;
              tabs.innerHTML += tab;
        }
      
        var header = `<tr class='bg-info'> <th>MQTT Broker IP</th><th>MQTT Broker Port</th><th>MQTT Topic</th></tr>`;
        headTable.innerHTML += header; 
        
        header =`<tr id="head"><td>
           <input type='text' id='mqttip' size='14' value='${data.mqtthost}' title="Enter IP address of MQTT broker.&#13;&#10;Hit update button to save changes.">
           <input class='button_style' type="button" value="Update" name='mqttip' onclick="updatemqtthost()" title="Save change to MQTT host IP."></td>  
           <td><input type='text' id='mqttport' size='7' value='${data.mqttport}' title="Enter port of MQTT broker.&#13;&#10;Hit update button to save changes.">
           <input class='button_style' type="button" value="Update" name='mqttport' onclick="updatemqttport()" title="Save change to MQTT host port."></td>
           <td><input type='text' id='mqtttopic'  size='20' value='${data.mqtttopic}' title="Enter MQTT topic.&#13;&#10;JMRI default: /trains/track/# &#13;&#10;Hit update button to save changes.">
           <input class='button_style' type="button" value="Update" name='mqtttopic' onclick="updatemqtttopic()" title="Save change to MQTT Topic.&#13;&#10;JMRI default: /trains/track/#"></td></tr>`
        headTable.innerHTML += header;
      
          for (var j = 0; j < data.number; j++){ 
          
                var table  = `<tbody id='tab${j}' class='tabcontent'>`
            
                table += `<tr  class='bg-info'> <th>Pin<br>No.</th><th>State</th><th>System<br>Name</th>
                <th>Sensor</th><th>Light</th><th>Turnout</th><th>PWM</th><th>Low<br>Angle</th><th>High<br>Angle</th><th>Description</th></tr>`
            
                if (j>0) pins = 16;
                
                for (var i = 0; i < pins; i++){
                        var schecked = "";
                        var lchecked = "";
                        var tchecked = "";
                        var pchecked = "";
                        var angdis   = "";
                        var pwmdis   = "";
                        var trdis    = "";
                        var txtdis   = "";
                        var dot = "";
                        descdis  = "";
                        dotclass = "dot";
                        
                        if(data.mode[j][i] == "S") {
                            schecked = "checked";
                            angdis = "disabled";
                            pwmdis = "disabled";
                  
                        }
                        
                        if(data.mode[j][i] == "L") {
                            lchecked = "checked";
                            angdis = "disabled";
                            pwmdis = "disabled";
                        }
                        
                        if (j == 0 && (i == 0 || i == 3 || i == 4 || i == 8) ){
                            schecked = "disabled";
                        }
                        
                        if (data.mode[j][i] == "T") {
                            tchecked = "checked";
                            if (data.pwm[j][i] == 1 ) {
                                pchecked = "checked";
                                angdis = "enabled";
                            } else {
                                angdis = "disabled";
                            }
                        }
                        
                        if ( ( data.mode[j][i] == "T" || data.mode[j][i] == "L" ) && data.names[j][i] != '0' ) {
                               dotclass = "dot pointer";
                               dot = "onclick=\"dot_click(this);\"";
                        }
                        
                        if (data.state[j][i] == "C" || data.state[j][i] == "F" ||  data.state[j][i] == "I" || data.names[j][i] == '0'){
                              colour = "red";
                        } else {
                              colour = "green";
                        }

                        if (data.names[j][i] == '0'){
                             descdis  = "disabled";
                        }
                        
                        if(data.type[j] == 2){
                             schecked = "disabled";
                        }
                  


                          if (  (j == 0 && data.nPCFDev >= 1 && i == 1 ) ||
                                (j == 0 && data.nPCFDev >= 2 && i == 2 ) ||
                                (j == 0 && data.nPCFDev >= 3 && i == 5 ) ||
                                (j == 0 && data.nPCFDev >= 4 && i == 6 ) ||
                                (j == 0 && data.nPCFDev >= 5 && i == 7 ) ){
                                txtdis = "disabled";
                                schecked = "disabled";
                                lchecked = "disabled";
                                tchecked = "disabled";                         
                          }
                        
                         var row = `<tr id='tr:${j}:${i} ${trdis}'>
                          <td title="Pin number of attached board this device is attached to." >${i}</td>
                          <td><span style='background-color:${colour}' name='txt:${j}:${i}' id='${j}dot${i}' class='${dotclass}' 
                                    ${dot} data-state='${data.state[j][i]}' 
                                    title="Activate/status button.&#13;&#10;Click to change state of an output device (light/turnout)&#13;&#10;Green=(Active/On/Thrown), Red=(Inactive/Off/Closed)"></span></td>
                          <td><input type='text' id='txt:${j}:${i}' size='2' value='${data.names[j][i]}' onChange='txtChange(this)' ${txtdis} 
                                    title="Enter JMRI system name.&#13;&#10;Numeric value (excluding leading MT/MS/ML).&#13;&#10;A value of zero means unused."></td>
                          <td><input type='radio' id='S:${j}:${i}' name='${j}mod${i}' value='S' onchange='radioClick(this)' 
                                    title="Select if attached device is a Sensor.&#13;&#10;i.e. a digital input." ${schecked} ></td>
                          <td><input type='radio' id='L:${j}:${i}' name='${j}mod${i}' value='L' onchange='radioClick(this)'  
                                    title="Select if attached device is a light.&#13;&#10;i.e. digital output (high/low)." ${lchecked} ></td>
                          <td><input type='radio' id='T:${j}:${i}' name='${j}mod${i}' value='T' onchange='radioClick(this)'  
                                    title="Select if attached device is a turnout.&#13;&#10;Ensure PWM is checked if requried." ${tchecked} ></td>
                          <td><input type='checkbox' id='pwm:${j}:${i}' ${pchecked} ${pwmdis} onchange='pwmClick(this)' 
                                     title="Select if turnout driver requires a PWM signal.&#13;&#10;Unchecked means digital (high/low) will be sent."></td>
                          <td><input type='number' id='la:${j}:${i}' style="width: 40px" min=0 max=180 value=${data.lang[j][i]} ${angdis} onchange='angleChange(this)' style-"width: 3em" 
                                     title="Enter the low angle if a PWM turnout is attached.&#13;&#10;Minumum = 0 and must be less than high angle."></td>
                          <td><input type='number' id='ha:${j}:${i}' style="width: 40px" min=0 max=180 value=${data.hang[j][i]} ${angdis} onchange='angleChange(this)' style-"width: 3em" 
                                     title="Enter the high angle if a PWM turnout is attached.&#13;&#10;Maximum = 180 and must be greater than low angle."></td>
                          <td><input type='text' id='desc:${j}:${i}' size='8' value='${data.desc[j][i]}' onChange='descChange(this)'  
                                     maxlength="10" title="Brief description of attached device. &#13;&#10;Max 10 characters." ${descdis} ></td>
                          </tr>`
                        table += row;
                }
                table += `</tbody>`;
                mainTable.innerHTML += table;
           }
      document.getElementById("defaultOpen").click();
}

//call function to get json data and display tables.
getJson();
)rawliteral";

//index.html template save to PROGMEM
const char indexhtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="en">
<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<script type="text/javascript" src="/jmri_js.js"></script>
<link rel="stylesheet" type="text/css" href="/style.css">
</head>
<body>
    <a href="https://www.jmri.org/"> 
    <img src="https://www.jmri.org/images/logo-jmri.gif" alt="JMRI"></a>
    <a style="font-size:10px"; href="https://www.jmri.org/copyright.shtml"> Copyright &copy; 1997 - 2021 JMRI Community. JMRI&#174;</a><br>
    <h2>JMRI Accessory MQTT Client Configuration.</h2>
    <p>Current Version: <span id=cver></span>&nbsp;&nbsp;&nbsp;Latest Version: <a id="href_lver" href="#" onclick="javascript: void(0);" class="disabled"><span id="lver" title="No update available."></span></a>
    <p>Client IP Address: <span id=ipadd></span>&nbsp;&nbsp;&nbsp;Client MAC Address: <span id=macadd></span></p>
    
    <input class='button_style' type="button" value="Hardware Reset" name='reset' onclick="eepromReset()" title="Reset configuration to default values. This includes WIFI settings..."></td>
    <input class='button_style' type="button" value="Board Rescan" name='rescan' onclick="i2cScan()" title="Rescan I2C for attached boards." ></td>
    <button class='button_style' type="button" onclick="location.href='/json_exp'" title="Create a backup of the current configuration." >Create backup</button> </td>  
    
    <button class='button_style' onclick="document.getElementById('jsonfileinput').click();" title="Restore configuration from file.&#13;&#10;Current configuration will be overwritten.&#13;&#10;Existing WIFI settings will not be effected.">Restore backup</button>
    <input type="file" accept="application/json" id="jsonfileinput" style="display: none" />
    
    <button class='button_style' onclick="document.getElementById('firmwareupdate').click();" title="Upgrade firmware from locally stored file.">Firmware Upgrade</button>
    <input type="file" accept=".bin" id="firmwareupdate" style="display: none" />  
    
    <button class='button_style' type="button" onclick="window.open('/logging.html','_blank')" title="Open message log tab." ">View Messages</button><br>
     
    <table id="headTable" class="table table-striped">
    </table>
    
    <div id="myTab" class="tab">
    </div>
    
    <table id="mainTable" class="table table-striped">
    </table>
    
</body>
</html>)rawliteral";


//style.css template save to PROGMEM
const char stylecss[] PROGMEM = R"rawliteral(
 
tr,
td,
th {
  border-color: inherit;
  border-style: solid;
  border-width: 0;
  text-align: center;
}
th{ 
    color:#fff;
  }

td,th{   
    padding: 1px,1px;
    }

      img {width: 80px;height: auto;}
      .hover {position: relative;top: 50px;left: 50px;}
      .dot {height:15px;width:15px;border-radius:7px;display:inline-block;}
      .dot.pointer {cursor: pointer;}
      input[type="range"] {width:50px;}
      div.a{display: none;}
      div.b{}
      body {font-family: Arial;}


a.disabled {
  pointer-events: none;
  cursor: default;
  text-decoration: none; 
  color: #000000;
}

/*input { display: none }*/
.tabtxt{

      text-align: center;
      background-color: inherit;
      //background-color: #ccc;
      border-top-style: hidden;
      border-right-style: hidden;
      border-left-style: hidden;
      border-left-style: hidden;
      border-bottom-style: hidden;
      font-size: 11px;
}
.tabtxt:focus{

      background-color: inherit;
      //background-color: #ccc;
      outline: none;

}
.tabtxt:hover {

      background-color: inherit;
      //background-color: #ccc;
      border-top-style: hidden;
      border-right-style: hidden;
      border-left-style: hidden;
      border-bottom-style: hidden;

}

.tab.active {
      background-color: #ccc;
      outline: none;
}


/* Style the tab */
.tab {
      display: inline-block;
      border: 1px solid #ccc;
      background-color: #f1f1f1;
}
/* Style the buttons inside the tab */
.tab button {
      background-color: inherit;
      float: left;
      border: none;
      outline: none;
      cursor: pointer;
      padding: 3px 3px;
      transition: 0.3s;
      font-size: 11px;
}
/* Change background color of buttons on hover */
.tab button:hover {
      background-color: #ddd;
}
/* Create an active/current tablink class */
.tab button.active {
      background-color: #ccc;
}

.button_style {
      font-family inherit;
      background-color: #0dcaf0 ; 
      border: none;
      color: white;
      padding: 5px 5px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      //font-size: 16px;
      border-radius: 6px;
}
.button_style:hover {
       background-color: #0abfe4; 
}

.br {
   display: block;
   margin: 10px 0;
}

.button_style:active {
  
  background: #e5e5e5;
  -webkit-box-shadow: inset 0px 0px 5px #c1c1c1;
     -moz-box-shadow: inset 0px 0px 5px #c1c1c1;
          box-shadow: inset 0px 0px 5px #c1c1c1;
   outline: none;
}
.table {
      --bs-table-bg: transparent;
      --bs-table-accent-bg: transparent;
      --bs-table-striped-color: #212529;
      --bs-table-striped-bg: rgba(0, 0, 0, 0.05);
      --bs-table-active-color: #212529;
      --bs-table-active-bg: rgba(0, 0, 0, 0.1);
      --bs-table-hover-color: #212529;
      --bs-table-hover-bg: rgba(0, 0, 0, 0.075);
      margin-bottom: 1rem;
      color: #212529;
      vertical-align: top;
      border-color: #dee2e6;
}
.table > :not(caption) > * > * {
      padding: 0.5rem 0.5rem;
      background-color: var(--bs-table-bg);
      border-bottom-width: 1px;
      box-shadow: inset 0 0 0 9999px var(--bs-table-accent-bg);
}
.table > tbody {
      vertical-align: inherit;
}
.table > thead {
      vertical-align: bottom;
}
.table > :not(:last-child) > :last-child > * {
      border-bottom-color: currentColor;
}
.caption-top {
      caption-side: top;
}
.table-sm > :not(caption) > * > * {
      padding: 0.25rem 0.25rem;
}
.table-bordered > :not(caption) > * {
      border-width: 1px 0;
}
.table-bordered > :not(caption) > * > * {
      border-width: 0 1px;
}
.table-borderless > :not(caption) > * > * {
      border-bottom-width: 0;
}
.table-striped > tbody > tr:nth-of-type(odd) {
      --bs-table-accent-bg: var(--bs-table-striped-bg);
      color: var(--bs-table-striped-color);
}
.table-active {
      --bs-table-accent-bg: var(--bs-table-active-bg);
      color: var(--bs-table-active-color);
}
.table-hover > tbody > tr:hover {
      --bs-table-accent-bg: var(--bs-table-hover-bg);
      color: var(--bs-table-hover-color);
}
.table-primary {
      --bs-table-bg: #cfe2ff;
      --bs-table-striped-bg: #c5d7f2;
      --bs-table-striped-color: #000;
      --bs-table-active-bg: #bacbe6;
      --bs-table-active-color: #000;
      --bs-table-hover-bg: #bfd1ec;
      --bs-table-hover-color: #000;
      color: #000;
      border-color: #bacbe6;
}
.bg-info {
      background-color: #0dcaf0 !important;
}
.table-striped > tbody > tr:nth-of-type(odd) {
      --bs-table-accent-bg: var(--bs-table-striped-bg);
      color: var(--bs-table-striped-color);
}

)rawliteral";

const char logbox[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang="en">
<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link rel="stylesheet" type="text/css" href="/style.css">

</head>
<body>
<a href="https://www.jmri.org/"> 
<img src="https://www.jmri.org/images/logo-jmri.gif" alt="JMRI"></a>
<a style="font-size:10px"; href="https://www.jmri.org/copyright.shtml"> Copyright &copy; 1997 - 2021 JMRI Community. JMRI&#174;</a><br>
<h2>JRMI Accessory MQTT Client Configuration message logger.</h2>

 Logging Level: <input type='radio' id='log:0' name='rlog' value='0' onchange='radioClick(this)' 
                        title="All logging disabled."><label>No Logging</label>
                <input type='radio' id='log:1' name='rlog' value='1' onchange='radioClick(this)'  
                        title="Basic logging, info only." checked ><label>Info Only</label>
                <input type='radio' id='log:2' name='rlog' value='2' onchange='radioClick(this)'  
                        title="Verbose logging."><label>Verbose</label><br>
 
<textarea id="logbox" rows=20 cols=90 title="JMRI logging viewer"></textarea>
<script type="text/javascript">

      if (!!window.EventSource) {
            var source = new EventSource('/events');
      }

//function called if radio button is changed
function radioClick(elem){
  
        //console.log(elem);
        var id = elem.getAttribute("id")
        var rval = id.split(":");
      
        var params = "lmode=" + rval[1];
        //console.log(params);
            
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/changeLogradio", true);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.send(params);
}


      var txt = document.getElementById("logbox");     

      function scrollLogToBottom() {
          logTa = document.getElementById("logbox");
          logTa.scrollTop = logTa.scrollHeight;
      }
      
      const zeroPad = (num, places) => String(num).padStart(places, '0')
     
      source.addEventListener('updatelog', function(e){ 
          //console.log('updatelog', e.data); 
          
          var today = new Date();
          var time = today.getHours() + ":" + today.getMinutes() + ":" + zeroPad(today.getSeconds(),2) + "." + zeroPad(today.getMilliseconds(),3) + " -> ";
          var update = e.data;
        
          var txt = document.getElementById("logbox");
          txt.value += time + update + "\n";
          scrollLogToBottom();
  
      }, false);

      var req = new XMLHttpRequest();
      req.open('GET', document.location, false);
      req.send(null);
      var loglvl = req.getResponseHeader ("Log-Level");
      document.getElementById('log:' + loglvl).checked = true;
      //console.log(loglvl);

</script>
</body>
</html>
)rawliteral";

#endif
