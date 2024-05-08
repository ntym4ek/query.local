<?php

function ext_admin_settings_form($form, &$form_state)
{
  $form['import_pr_1с_enable'] = [
    '#title' => 'Производство. Включить импорт из 1с',
    '#type' => 'checkbox',
    '#default_value' => variable_get('import_pr_1с_enable', 0),
  ];
  $form['import_wh_1с_enable'] = [
    '#title' => 'Склад. Включить импорт из 1с',
    '#type' => 'checkbox',
    '#default_value' => variable_get('import_wh_1с_enable', 0),
  ];
  $form['client_email_enable'] = array(
    '#type' => 'checkbox',
    '#title' => 'Включить e-mail рассылку с уведомлениями (иначе только имитация в лог)',
    '#default_value' => variable_get('client_email_enable', 1),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#attributes' => ['class' => ['btn-success']],
    '#value' => 'Сохранить настройки',
  ];

  return system_settings_form($form);
}
