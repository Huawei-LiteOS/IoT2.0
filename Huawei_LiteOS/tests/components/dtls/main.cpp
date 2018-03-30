#include<cpptest.h>
#include<iostream>
#include<fstream>
#include "test_dtls.h"

int main()
{
  Test::Suite ts;
  ts.add(std::auto_ptr<Test::Suite>(new TestDtls));

  std::ofstream html;
  html.open("Result.htm");
  
  Test::HtmlOutput output;
  //Test::TextOutput to(Test::TextOutput::Verbose);
  //ts.run(to);
  ts.run(output);
  output.generate(html);
  html.close();
}


