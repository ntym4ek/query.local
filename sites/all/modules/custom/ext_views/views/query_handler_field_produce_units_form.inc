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
      $notif_arr[$notif_wr->field_qn_produce_unit->getIdentifier()] = $notif_wr->field_qn_notification->value();
    }

    // вывести комментарий, если заявка от клиента
    // вывести установки, если есть
    if ($query_load = drupal_json_decode($query_wr->field_load_json->value())) {
      $output .=
        '<div class="produce-units">' .
          '<div class="produce-units-list">';

      foreach ($query_load as $pu_tid => $query_pu_load) {
        $produce_unit = production_produce_unit_get_load($month_start, $pu_tid, $query_wr->getIdentifier());
        $notification = isset($notif_arr[$pu_tid]) ? $notif_arr[$pu_tid] : null;
        $output .= theme('produce_unit_load', ['produce_unit' => $produce_unit[$pu_tid], 'month_start' => $month_start,  'notification' => $notification,  'show_amount' => false, 'show_nomenklatura' => false]);
      }

      $output .=
          '</div>' .
          '<div class="produce-unit-legenda">' .
            '<div><span></span>- установка свободна</div>' .
            '<div><span class="loaded"></span>- ' . (user_has_role(ROLE_MANAGER_PRODUCTION) ? 'тех. работы' : 'установка занята') . '</div>' .
            '<div><span class="company-loaded"></span>- другая продукция' . (user_has_role(ROLE_MANAGER_PRODUCTION) ? '' : ' компании') . '</div>' .
            '<div><span class="selected"></span>- даты производства</div></div>' .
        '</div>';
    }

    if (!empty($values->field_field_comment[0]["raw"]["value"])) {
      $output .=
        '<div class="query-comment" data-query-id="' . $query_id . '">' .
          '<div class="alert alert-warning" role="alert">' .
            $values->field_field_comment[0]["raw"]["value"] .
          '</div>' .
        '</div>';
    }

    return $output;
  }
}
