#include <cpptest.h>
#include <iostream>
#include <fstream>
#include "test_agenttiny.h"
#include "test_object_connectivity_moni.h"
#include "test_object_device.h"
#include "test_object_firmware.h"
#include "test_object_app.h"
#include "test_connection.h"
#include "test_atiny_rpt.h"


int main(){
  Test::Suite ts;
  ts.add(std::auto_ptr<Test::Suite>(new TestAgenttiny));
  ts.add(std::auto_ptr<Test::Suite>(new TestConnection));
  ts.add(std::auto_ptr<Test::Suite>(new TestObjectConnectivityMoni));
  ts.add(std::auto_ptr<Test::Suite>(new TestObjectDevice));
  ts.add(std::auto_ptr<Test::Suite>(new TestObjectFirmware));
  ts.add(std::auto_ptr<Test::Suite>(new TestObjectApp));
  ts.add(std::auto_ptr<Test::Suite>(new TestAtinyRpt));

  std::ofstream html;
  html.open("Result.htm");
  
  Test::HtmlOutput output;
  ts.run(output);
  output.generate(html);
  html.close();
}
