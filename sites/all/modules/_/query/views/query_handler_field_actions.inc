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

    if ($values->field_field_query_type[0]["raw"]["value"] == 'correction') {
      if (!user_has_role(ROLE_ADMIN) && !user_has_role(ROLE_MANAGER)) {
        if ($values->field_field_query_status_client[0]["raw"]["value"] != 'approved') {
          $output .= '<a class="btn btn-success btn-xs" href="/query/' . $query_id . '/correction/send">отправить</a>';
        }
      } else {
        if ($values->field_field_query_status_manager[0]["raw"]["value"] == 'processing') {
          $output .= '<a class="btn btn-success btn-xs" href="/query/' . $query_id . '/correction/approve">утвердить</a>';
        }
      }
    }

    if ($values->field_field_query_status_client[0]["raw"]["value"] != 'approved' || $values->field_field_query_status_manager[0]["raw"]["value"] != 'approved') {
      $output .= '<a class="btn btn-default btn-xs" href="/admin/structure/entity-type/query/query/' . $query_id . '/edit">редактировать</a>';
      $output .= '<a class="btn btn-danger btn-xs" href="/admin/structure/entity-type/query/query/' . $query_id . '/delete">удалить</a>';
    }

    $efquery = new EntityFieldQuery();
    $has_correction = $efquery
      ->entityCondition('entity_type', 'query')
      ->fieldCondition('field_month', 'value', $values->field_field_month[0]["raw"]["value"])
      ->fieldCondition('field_nomenklatura', 'target_id', $values->field_field_nomenklatura[0]["raw"]["target_id"])
      ->fieldCondition('field_query_type', 'value', 'correction')
      ->count()
      ->execute();
    if (!$has_correction && $values->field_field_query_status_client[0]["raw"]["value"] == 'approved' && $values->field_field_query_status_manager[0]["raw"]["value"] == 'approved') {
      $output .= '<a class="btn btn-info btn-xs" href="/query/' . $query_id . '/correction/add">создать корректировку</a>';
    }

    return $output;
  }
}
