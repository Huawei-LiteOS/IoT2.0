#ifndef _TEST_ATINY_RPT_H_
#define _TEST_ATINY_RPT_H_

typedef struct _atiny_rpt_list_t
{
    atiny_dl_list node;
    lwm2m_uri_t uri;
    atiny_dl_list rpt_list;
    int rpt_node_cnt;
}atiny_rpt_list_t;

typedef struct
{ 
    atiny_dl_list list;
    data_report_t data;
}atiny_rpt_node_t;


class TestAtinyRpt:public Test::Suite {
 protected:
  void tear_down();
  void setup();

 public:
  void test_atiny_init_rpt();
  void test_atiny_add_rpt_uri();
  void test_atiny_rm_rpt_uri();
  void test_atiny_queue_rpt_data();
  void test_atiny_dequeue_rpt_data();
  void test_atiny_clear_rpt_data();

  TestAtinyRpt();
};









#endif 

