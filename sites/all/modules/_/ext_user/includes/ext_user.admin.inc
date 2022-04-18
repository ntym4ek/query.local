<?php

/**
 * @file
 * Admin UI.
 */

function ext_user_transfer_callback()
{
//  db_delete('locales_source')
//    ->condition('location', '/admin/reports/dblog')
//    ->execute();
//
//  db_delete('locales_source')
//    ->condition('location', '%' . db_like('/update.php?') . '%')
//    ->execute();

  $queue = DrupalQueue::get('ext_user_queue_worker');
  $queue->deleteQueue();


  // todo переназначить типы request и plan
  $ids = [];
  $dbr = db_select('eck_query', 'q');
  $dbr->addField('q', 'id');
//  $dbr->condition('q.id',2425);
  if ($results = $dbr->execute()->fetchCol()) {

    $i = 1;
    foreach ($results as $key => $result) {
//      if ($result != 40239) continue;
      $ids[] = $result;

      if ($key > $i*5000) {
        $queue->createItem($ids);
        $i++;
        $ids = [];
      }

    }
  }
  drupal_set_message('Done ' . $i . ' iterations');


  $dest = strpos($_SERVER["HTTP_REFERER"], $GLOBALS["base_url"]) !== false ? $_SERVER["HTTP_REFERER"] : false;
  if ($dest) {
    drupal_goto($dest);
  }
}



function ext_user_settings_form($form, &$form_state)
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

  return $form;
}
function ext_user_settings_form_submit($form, $form_state)
{
  variable_set('import_pr_1с_enable', $form_state['values']['import_pr_1с_enable']);
  variable_set('import_wh_1с_enable', $form_state['values']['import_wh_1с_enable']);
  variable_set('client_email_enable', $form_state['values']['client_email_enable']);
}
