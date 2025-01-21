<?php

/**
 * Description of what my handler does.
 */
class warehouse_handler_field_status extends views_handler_field
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
    $output = '';

    $wwork_time = $values->field_field_wwork_time[0]["raw"]["value"];
    $day_start = gmmktime(0, 0, 0, gmdate('n', $wwork_time), gmdate('j', $wwork_time), gmdate('Y', $wwork_time));
    $year = date('Y', $wwork_time);
    $month = date('m', $wwork_time);
    $day = date('d', $wwork_time);
    $offset = ($wwork_time - $day_start)/60;
    $offset += 180; // корректировка из-за решения в date_field_prepare_view (warehouse.module)

    $shedule = warehouse_get_day_schedule($month, $day, $year);
    if (isset($shedule[$offset]['load']->status)) {
      $output = warehouse_get_text_for_order_status($shedule[$offset]['load']->status);
    }

    return $output;
  }
}
