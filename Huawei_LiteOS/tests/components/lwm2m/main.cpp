#include <cpptest.h>
#include <iostream>
#include <fstream>
#include "test_agenttiny.h"
//#include "test_connection.h"


int main(){
  Test::Suite ts;
  ts.add(std::auto_ptr<Test::Suite>(new TestAgenttiny));

  std::ofstream html;
  html.open("Result.htm");
  
  Test::HtmlOutput output;
  ts.run(output);
  output.generate(html);
  html.close();
}
