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
    $notif_arr = [];
    foreach($query_wr->field_query_notifications->getIterator() as $notif_wr) {
      $notif_arr[$notif_wr->field_qn_produce_unit->tid->value()] = $notif_wr->field_qn_notification->value();
    }

    // вывести комментарий, если заявка от клиента
    // вывести установки, если есть
    if ($query_load = drupal_json_decode($query_wr->field_load_json->value())) {
//    if ($produce_units = production_produce_units_get_from_query1($query_wr->value())) {
      $output .=
        '<div class="produce-units">' .
          '<div class="produce-units-list">';

      foreach ($query_load as $pu_tid => $query_pu_load) {
        //$load = production_produce_unit_get_load($pu_tid, $month_start, $query_wr->created->value(), $query_wr->value());
        $pu_wr = entity_metadata_wrapper('taxonomy_term', $pu_tid);
        $load = production_produce_unit_get_load1($pu_tid, $month_start, $query_wr->created->value(), $query_wr->getIdentifier());

        $output .=
          '<div class="produce-unit">' .
          '<div class="produce-unit-name">' .
          '<label class="views-label" for="edit-name--2">Установка</label>' .
          '<h3>' . $pu_wr->label() . '</h3>' .
          (empty($notif_arr[$pu_tid]) ? '' : '<div class="notification" data-toggle="popover" data-trigger="hover" title="Внимание!" data-content="' . $notif_arr[$pu_tid] . '"></div>') .
          '</div>' .
          '<div class="produce-unit-dates">' .
          '<label class="views-label">Даты</label>' .
          '<div class="form-checkboxes">';
        for ($i = 1; $i <= date("t", $month_start); $i++) {
          $day_start = $month_start + ($i - 1) * 60 * 60 * 24;

          $classes = empty($load[$day_start]['classes']) ? [] : $load[$day_start]['classes'];

          $tooltip = empty($load[$day_start]['output']) || empty($load[$day_start]['is_current_load']) ? '' : ' data-toggle="tooltip" data-placement="top" title="' . $load[$day_start]['output'] . '"';

          $output .= '<span class="c-box ' . implode(' ', $classes) . '"' . $tooltip . '>' . $i . '</span>';
        }
        $output .=
          '</div>' .
          '</div>' .
          '</div>';
      }

      $output .=
        '</div>' .
        '<div class="produce-unit-legenda"><div><span></span>- установка свободна</div><div><span class="loaded"></span>- установка занята</div><div><span class="company-loaded"></span>- другая продукция компании</div><div><span class="selected"></span>- выбранные даты</div></div>' .
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
