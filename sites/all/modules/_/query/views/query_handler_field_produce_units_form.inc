<?php

/**
 * Description of what my handler does.
 */
class query_handler_field_produce_units_form extends views_handler_field
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
    // todo убрать формы, вывести статику
    // вывести формы выбора дат для каждой установки
    $query_id = $values->id;

    $form_id = query_produce_unit_select_dates_form_id($query_id);
    $produce_unit_form = drupal_get_form($form_id, $query_id);

    return drupal_render($produce_unit_form);
  }
}
