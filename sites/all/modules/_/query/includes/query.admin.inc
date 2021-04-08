<?php

/**
 * @file
 * Admin UI.
 */

/**
 * Settings.
 */
function query_settings_form($form, &$form_state)
{
  $form['import_1с_enable'] = array(
    '#type' => 'checkbox',
    '#title' => 'Включить импорт из 1с',
    '#default_value' => variable_get('import_1с_enable', 1),
  );

  $form['client_email_enable'] = array(
    '#type' => 'checkbox',
    '#title' => 'Включить e-mail рассылку с уведомлениями (иначе только имитация в лог)',
    '#default_value' => variable_get('client_email_enable', 1),
  );

  return system_settings_form($form);
}
