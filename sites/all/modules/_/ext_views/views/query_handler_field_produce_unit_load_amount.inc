<?php

/**
 * Description of what my handler does.
 */
class query_handler_field_produce_unit_load_amount extends views_handler_field
{
  function query()
  {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * Renders the field handler.
   */
  function render($values)
  {
    // период отчёта
    $month_min = empty($this->view->exposed_raw_input["month"]["min"]) ? $this->view->query->where[1]["conditions"][1]["value"][":field_data_field_month_field_month_value"] : $this->view->exposed_raw_input["month"]["min"];
    $month_max = empty($this->view->exposed_raw_input["month"]["max"]) ? $this->view->query->where[1]["conditions"][1]["value"][":field_data_field_month_field_month_value1"] : $this->view->exposed_raw_input["month"]["max"];

    list($year_begin, $month_begin) = explode('-', $month_min);
    list($year_end, $month_end) = explode('-', $month_max);
    $begin_month_start = gmmktime(0,0,0, $month_begin, 1, $year_begin);
    $end_month_start = gmmktime(0,0,0, $month_end, 1, $year_end);
    $current_month_start = $begin_month_start;
    $today_start = gmmktime(0, 0, 0, date('n'), date('j'), date('Y'));
    $stop = false;
    $output = 0;
    do {
//      $load_arr = production_produce_unit_get_load($values->taxonomy_term_data_field_data_field_produce_unit_tid, $current_month_start);
      $load_arr = production_produce_unit_get_load($current_month_start, $values->taxonomy_term_data_field_data_field_produce_unit_tid);

      foreach($load_arr as $day_start => $load) {
        if (($load['nomenklatura']['tid'] == $values->tid) && $day_start > $today_start) {
          $output += $load['output'];
        }
      }

      if ($current_month_start == $end_month_start) $stop = true;
      else $current_month_start = gmmktime(0,0,0, date('n', $current_month_start)+1, 1, date('Y', $current_month_start));
    } while (!$stop);

    return number_format($output, 0, '', ' ');
  }
}
