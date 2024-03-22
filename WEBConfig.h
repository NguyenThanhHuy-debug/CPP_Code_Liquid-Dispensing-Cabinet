//=========Chứa mã HTLM Website==//
#ifndef _HTML_H_
#define _HTML_H_
#include <Arduino.h>

const char MainpageHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta name="viewport" content="width=device-width,user-scalable=0" charset="UTF-8">		
    <script async src="code.js"></script>    
    <link rel="stylesheet" href="style.css">
		<title>IoTVision</title>
	</head>
	<body>
    <div style="text-align: center">
      <header>
                        <img src="data:image/png;base64,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" 
                        alt="Logo">
					</header>
    </div>
    <table style="width: 100%">
        <tr>
            <th colspan="2" style="text-align: center; padding: 5px; border-bottom: 0.5px solid #c8c8c8">Thiết lập WiFi</th>
        </tr>
        <tr>
            <td style="text-align: right; padding: 5px; width: 35%">Tên WiFi:</td>
            <td><input id="tbxTenWiFi"/></td>
        </tr>
        <tr>
            <td style="text-align: right; padding: 5px">Mật khẩu:</td>
            <td><input id="tbxMatKhauWiFi"/></td>
        </tr>
    </table>
    <div style="text-align: right; padding-right: 10px">        
        <button class="NutNhan" id="btnQuetTimWiFi" onclick="QuetTimWiFi()">QUÉT WiFi</button>
        <button class="NutNhan" id="btnLuuWiFiVaoEEPROM" onclick="LuuWiFiVaoEEPROM()">LƯU</button>
    </div>     
    <div id="ThongBao"></div>
    <section id="DanhSachWiFi"></section>
	</body>
<html>
)rawliteral";


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================================== Begin: STYLE CSS ===============================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
const char style_css[] PROGMEM = R"rawliteral(
body {
  background-color: #FFF;
  border: 0;
  margin: 0;
  font: 1em tahoma, arial, sans-serif;
}  
header {
  border-bottom: 1px solid #888;
  padding: 10px;
}
section {
  padding-top: 5px;
  border-bottom: 1px solid #888;
  border-top: 1px solid #888;
}
#ThongBao {
  border-top: 1px solid #888;
  text-align: left; 
  padding: 5px 0 5px 15px;
  font: 0.9em tahoma, arial, sans-serif;
  font-style: italic;
}
h1 {
color: #F07057;
}      
.NutNhan {
  height: 30px; 
  width: 130px; 
  margin: 10px 0;
  background-color: #F07057;
  border-radius: 3px;
  border: 1px solid #EEE;
}
.ape {
  margin-left: 15px;
  padding: 3px 0px;
  font: 0.9em tahoma, arial, sans-serif;
}
.ape:hover {
  cursor: pointer;
}
.w0 {
  background: url('data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+Cjxzdmcgd2lkdGg9IjI0IiBoZWlnaHQ9IjI0IiB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+CjxwYXRoIGQ9Ik0xLDlMMywxMUM3Ljk3LDYuMDMgMTYuMDMsNi4wMyAyMSwxMUwyMyw5QzE2LjkzLDIuOTMgNy4wOCwyLjkzIDEsOVoiIHN0eWxlPSJmaWxsOiBibGFjazsiLz4KPHBhdGggZD0iTTUsMTNMNywxNUM5Ljc2LDEyLjI0IDE0LjI0LDEyLjI0IDE3LDE1TDE5LDEzQzE1LjE0LDkuMTQgOC44Nyw5LjE0IDUsMTNaIiBzdHlsZT0iZmlsbDogYmxhY2s7Ii8+CjxwYXRoIGQ9Ik05LDE3TDEyLDIwTDE1LDE3QzEzLjM1LDE1LjM0IDEwLjY2LDE1LjM0IDksMTdaIiBzdHlsZT0iZmlsbDogYmxhY2s7Ii8+Cjwvc3ZnPgo=') no-repeat right top;
  height: 24px;
  margin-right: 20px;
}
.w1 {
  background:  url('data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+Cjxzdmcgd2lkdGg9IjI0IiBoZWlnaHQ9IjI0IiB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+CjxwYXRoIGQ9Ik0xLDlMMywxMUM3Ljk3LDYuMDMgMTYuMDMsNi4wMyAyMSwxMUwyMyw5QzE2LjkzLDIuOTMgNy4wOCwyLjkzIDEsOVoiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNNSwxM0w3LDE1QzkuNzYsMTIuMjQgMTQuMjQsMTIuMjQgMTcsMTVMMTksMTNDMTUuMTQsOS4xNCA4Ljg3LDkuMTQgNSwxM1oiIHN0eWxlPSJmaWxsOiBibGFjazsiLz4KPHBhdGggZD0iTTksMTdMMTIsMjBMMTUsMTdDMTMuMzUsMTUuMzQgMTAuNjYsMTUuMzQgOSwxN1oiIHN0eWxlPSJmaWxsOiBibGFjazsiLz4KPC9zdmc+Cg==') no-repeat right top;
  height: 24px;
  margin-right: 20px;
}
.w2 {
  background:  url('data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+Cjxzdmcgd2lkdGg9IjI0IiBoZWlnaHQ9IjI0IiB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+CjxwYXRoIGQ9Ik0xLDlMMywxMUM3Ljk3LDYuMDMgMTYuMDMsNi4wMyAyMSwxMUwyMyw5QzE2LjkzLDIuOTMgNy4wOCwyLjkzIDEsOVoiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNNSwxM0w3LDE1QzkuNzYsMTIuMjQgMTQuMjQsMTIuMjQgMTcsMTVMMTksMTNDMTUuMTQsOS4xNCA4Ljg3LDkuMTQgNSwxM1oiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNOSwxN0wxMiwyMEwxNSwxN0MxMy4zNSwxNS4zNCAxMC42NiwxNS4zNCA5LDE3WiIgc3R5bGU9ImZpbGw6IGJsYWNrOyIvPgo8L3N2Zz4K') no-repeat right top;
  height: 24px;
  margin-right: 20px;
}
.w3 {
  background:  url('data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhRE9DVFlQRSBzdmcgUFVCTElDICItLy9XM0MvL0RURCBTVkcgMS4xLy9FTiIgImh0dHA6Ly93d3cudzMub3JnL0dyYXBoaWNzL1NWRy8xLjEvRFREL3N2ZzExLmR0ZCI+Cjxzdmcgd2lkdGg9IjI0IiBoZWlnaHQ9IjI0IiB2ZXJzaW9uPSIxLjEiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyIgeG1sOnNwYWNlPSJwcmVzZXJ2ZSI+CjxwYXRoIGQ9Ik0xLDlMMywxMUM3Ljk3LDYuMDMgMTYuMDMsNi4wMyAyMSwxMUwyMyw5QzE2LjkzLDIuOTMgNy4wOCwyLjkzIDEsOVoiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNNSwxM0w3LDE1QzkuNzYsMTIuMjQgMTQuMjQsMTIuMjQgMTcsMTVMMTksMTNDMTUuMTQsOS4xNCA4Ljg3LDkuMTQgNSwxM1oiIHN0eWxlPSJmaWxsOiBncmF5OyIvPgo8cGF0aCBkPSJNOSwxN0wxMiwyMEwxNSwxN0MxMy4zNSwxNS4zNCAxMC42NiwxNS4zNCA5LDE3WiIgc3R5bGU9ImZpbGw6IGdyYXk7Ii8+Cjwvc3ZnPgo=') no-repeat right top;
  height: 24px;
  margin-right: 20px;
}
)rawliteral";
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================================== Begin: STYLE CSS ===============================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================================== Begin: CODE JAVASCRIPT =========================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
const char code_js[] PROGMEM = R"rawliteral(

const gel = (e) => document.getElementById(e);

function create_obj() {
    td = navigator.appName;
    if(td == "Microsoft Internet Explorer") {
        obj = new ActiveXObject("Microsoft.XMLHTTP");
    }
    else {
        obj = new XMLHttpRequest();
    }
    
    return obj;
}
var xhttp = create_obj();
//-----------------------------------------------------------------------------------------------------
async function LuuWiFiVaoEEPROM() {
    var TenWiFi = document.getElementById("tbxTenWiFi").value;
    var MatKhauWiFi = document.getElementById("tbxMatKhauWiFi").value;
    xhttp.open("GET","/LuuWiFiVaoEEPROM?TenWiFi=" + TenWiFi + "&MatKhauWiFi=" + MatKhauWiFi, true);
    xhttp.onreadystatechange = KetQuaLuuWiFi;//nhận reponse 
    xhttp.send();
}
//--------------------------------Kiểm tra response -------------------------------------------------//
function KetQuaLuuWiFi(){
  if(xhttp.readyState == 4 && xhttp.status == 200){        
      //---------- Update data sử dụng javascript ----------//
      var res = xhttp.responseText;    
      gel("ThongBao").innerHTML = res;     
  }
}
//-----------------------------------------------------------------------------------------------------
async function QuetTimWiFi() {
    gel("ThongBao").innerHTML = "Đang quét tìm WiFi (15-30s)...";
    xhttp.open("GET","/QuetTimWiFi", true);
    xhttp.onreadystatechange = KetQuaQuetTimWiFi;//nhận reponse 
    xhttp.send();
}
//--------------------------------Kiểm tra response -------------------------------------------------//
function KetQuaQuetTimWiFi(){
  if(xhttp.readyState == 4 && xhttp.status == 200){        
      //---------- Update data sử dụng javascript ----------//
      var res = xhttp.responseText;    
      gel("DanhSachWiFi").innerHTML = res;     
  }
}
//-----------------------------------------------------------------------------------------------------
gel("DanhSachWiFi").addEventListener(
  "click",
  (e) => {
    gel("tbxTenWiFi").value = e.target.innerText;
  },
  false
);
//-----------------------------------------------------------------------------------------------------
)rawliteral";
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================================== End: CODE JAVASCRIPT ===========================================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

#endif