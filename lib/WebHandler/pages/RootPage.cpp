#include "Pages.h"
#include <Util.hpp>

const char ROOT_STYLE[] PROGMEM =
    "<style>\n"
    "pre { font-size: 0.8em; }\n"
    "</style>\n";

const char LAST10_SCRIPT[] PROGMEM =
    "<script>\n"
    "function loadLast10(){\n"
    "var e = document.getElementById(\"last10\");\n"

    "var t = document.getElementById(\"sgid0\");\n"
    "var p = document.getElementById(\"sgid1\");\n"
    "var h = document.getElementById(\"sgid2\");\n"

    "fetch(\"/last10\").then((resp) => resp.json()).then(function(data){\n"
    "var text=\"\";\n"
    "var i;\n"
    "for ( i = 0; i < data.last10.length; i++) {\n"
    "text += data.last10[i] + \"\\n\";\n"
    "}\n"
    "e.textContent=text;\n"
    "});\n"

    "fetch('/sensor').then(resp => resp.json()).then(function (o) {\n"
    "t.value = o.temperature_C.toFixed(1) + '°C',\n"
    "h.value = o.humidity.toFixed(1) + '%',\n"
    "p.value = o.pressure_hPa.toFixed(1) + 'hPa' });\n"

    "}\n"
    "setInterval(loadLast10,10000);\n"
    "document.onload=loadLast10();\n"
    "</script>\n";

const char SENSOR_HTML[] PROGMEM =
    "<div class='pure-control-group'>\n"
    "<label for='sgid0'>Temperature</label>\n"
    "<input id='sgid0' type='text' maxlength='64' readonly>\n"
    "</div>\n"

    "<div class='pure-control-group'>\n"
    "<label for='sgid1'>Pressure</label>\n"
    "<input id='sgid1' type='text' maxlength='64' readonly>\n"
    "</div>\n"

    "<div class='pure-control-group'>\n"
    "<label for='sgid2'>Humidity</label>\n"
    "<input id='sgid2' type='text' maxlength='64' readonly>\n"
    "</div>\n";

void handleRootPage()
{
  sendHeader(appcfg.ota_hostname, false, ROOT_STYLE);
  sendPrint("<form class='pure-form pure-form-aligned'>");
  sendLegend("Sensor");
  sendPrint(SENSOR_HTML);
  sendLegend("Last 10 LOG Messages");
  sendPrint("<pre><div id='last10'></div></pre></form>\n");
  sendPrint(LAST10_SCRIPT);
  sendFooter();
}
