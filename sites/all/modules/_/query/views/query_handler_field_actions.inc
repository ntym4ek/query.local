<?php

/**
 * Description of what my handler does.
 */
class query_handler_field_actions extends views_handler_field
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
    // вывести формы выбора дат для каждой установки
    $query_id = $values->id;
    $month_start = $values->field_field_month[0]["raw"]["value"];

    // новая корректировка только при утверждённой основной и отсутствии созданных позже
    $efquery = new EntityFieldQuery();
    $has_correction = $efquery
      ->entityCondition('entity_type', 'query')
      ->propertyCondition('created', $values->eck_query_created, '>')
      ->fieldCondition('field_month', 'value', $month_start)
      ->fieldCondition('field_nomenklatura', 'target_id', $values->field_field_nomenklatura[0]["raw"]["target_id"])
      ->count()
      ->execute();
    if (!$has_correction) {
      // изменения за сутки до начала производства запретить
      if (!query_check_query_produce_started($query_id)) {
        // для клиента
        if (!user_has_role(ROLE_MANAGER) && !user_has_role(ROLE_ADMIN)) {
          if ($values->field_field_query_status_client[0]["raw"]["value"] == 'processing' && $values->field_field_query_type[0]["raw"]["value"] != 'main') {
            $output .= '<a class="btn btn-success btn-xs" href="/query/' . $query_id . '/approve">согласовать</a>';
          }
          if ($values->field_field_query_type[0]["raw"]["value"] == 'correction'
            || ($values->field_field_query_type[0]["raw"]["value"] == 'main' && $values->field_field_query_status_client[0]["raw"]["value"] == 'approved')) {
            $output .= '<a class="btn btn-danger btn-xs" href="/admin/structure/entity-type/query/query/add?type=cancel&query_id=' . $query_id . '">отменить производство</a>';
          }
          // для менеджера
        } else {
          if ($values->field_field_query_status_manager[0]["raw"]["value"] != 'approved') {
            $output .= '<a class="btn btn-success btn-xs" href="/query/' . $query_id . '/approve">утвердить</a>';
            $output .= '<a class="btn btn-default btn-xs" href="/admin/structure/entity-type/query/query/' . $query_id . '/edit">редактировать</a>';
          }
          if ($values->field_field_query_type[0]["raw"]["value"] == 'correction'
            || ($values->field_field_query_type[0]["raw"]["value"] == 'main' && $values->field_field_query_status_client[0]["raw"]["value"] == 'approved')) {
            $output .= '<a class="btn btn-danger btn-xs" href="/admin/structure/entity-type/query/query/add?type=cancel&query_id=' . $query_id . '">отменить производство</a>';
          }
        }

        // для обоих
        if ($values->field_field_query_type[0]["raw"]["value"] == 'correction'
          || ($values->field_field_query_type[0]["raw"]["value"] == 'cancel' && ($values->field_field_query_status_client[0]["raw"]["value"] != 'approved' || $values->field_field_query_status_manager[0]["raw"]["value"] != 'approved'))
          || ($values->field_field_query_type[0]["raw"]["value"] == 'main' && $values->field_field_query_status_client[0]["raw"]["value"] == 'approved')) {
          $output .= '<a class="btn btn-info btn-xs" href="/admin/structure/entity-type/query/query/add?type=correction&query_id=' . $query_id . '">создать корректировку</a>';
        }
      }
    }

    return $output;
  }
}
