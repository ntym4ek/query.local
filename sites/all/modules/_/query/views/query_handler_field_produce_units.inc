<?php

/**
 * Description of what my handler does.
 */
class query_handler_field_produce_units extends views_handler_field
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

    // todo вывести формы выбора дат для каждой установки
    // todo в зависимости от продукта и загруженности установки
    $query_id = $values->id;
    $product_tid = $values->field_field_product[0]["raw"]["tid"];
    $produce_units = query_get_produce_units_by_product($product_tid);
    $month = $values->field_field_month[0]["raw"]['value'];

    foreach($produce_units as $produce_unit) {
      $form_id = query_produce_unit_select_dates_form_id($query_id, $produce_unit['tid']);
      $produce_unit_form = drupal_get_form($form_id, $query_id, $product_tid, $produce_unit, $month);
      $output .= drupal_render($produce_unit_form);
    }

    return $output;
  }
}
