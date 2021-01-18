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
    // вывести выбранные даты для каждой установки
    $output = '';
    $query_id = $values->id;

    $query_wr = entity_metadata_wrapper('query', $query_id);
    $month_start = $query_wr->field_month->value();

    // вывести комментарий, если заявка от клиента
    // вывести установки, если есть
    $produce_units = query_get_produce_units_from_query($query_wr->value());
    foreach ($produce_units as $pu_tid => $produce_unit) {
      $load = query_get_produce_unit_load($pu_tid, $month_start, $query_wr->created->value(), $query_wr->value());

      $output .=
        '<div class="produce-unit">' .
        '<div class="produce-unit-name">' .
        '<label class="views-label" for="edit-name--2">Установка</label>' .
        '<h3>' . $produce_unit['name'] . '</h3>' .
        '</div>' .
        '<div class="produce-unit-dates">' .
        '<label class="views-label">Даты</label>' .
        '<div class="form-checkboxes">';
      for ($i = 1; $i <= date("t", $month_start); $i++) {
        $day_start = $month_start + ($i - 1) * 60 * 60 * 24;

        $classes = empty($load[$day_start]['classes']) ? [] : $load[$day_start]['classes'];

        $tooltip = empty($load[$day_start]['output']) || empty($load[$day_start]['is_busy_same_company']) ? '' : ' data-toggle="tooltip" data-placement="top" title="' . $load[$day_start]['output'] . '"';

        $output .= '<span class="c-box ' . implode(' ', $classes) . '"' . $tooltip . '>' . $i . '</span>';
      }
      $output .=
        '</div>' .
        '<div class="produce-unit-legenda"><span></span>- установка свободна <span class="loaded"></span>- установка занята <span class="company-loaded"></span>- другая продукция компании <span class="selected"></span>- выбранные даты</div>' .
        '</div>' .
        '</div>';
    }

    if (!empty($values->field_field_comment[0]["raw"]["value"])) {
      $output .=
        '<div class="query-comment">' .
        '<div class="alert alert-warning" role="alert">' .
        $values->field_field_comment[0]["raw"]["value"] .
        '</div>' .
        '</div>';
    }

    return $output;
  }
}
