<?php

/**
 * Description of what my handler does.
 */
class query_handler_field_comment extends views_handler_field
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
    $output = 'нет';
    if (!empty($values->field_field_comment)) {
      $output = '<a data-container="body" data-toggle="popover" data-html="true" data-placement="right" data-content="' . $values->field_field_comment[0]["raw"]["value"] . '">открыть</a>';
    }

    return $output;
  }
}
